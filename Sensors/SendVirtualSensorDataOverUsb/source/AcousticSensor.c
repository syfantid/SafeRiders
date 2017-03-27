/**
 * Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licensee�s application development.
 * Fitness and suitability of the example code for any use within Licensee�s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
 * Licensee shall be responsible for conducting the development of its applications as well as integration of parts of the example code into such applications, taking into account the state of the art of technology and any statutory regulations and provisions applicable for such applications. Compliance with the functional system requirements and testing there of (including validation of information/data security aspects and functional safety) and release shall be solely incumbent upon Licensee.
 * For the avoidance of doubt, Licensee shall be responsible and fully liable for the applications and any distribution of such applications into the market.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 *     (3)The name of the author may not be used to
 *     endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
//lint -esym(956,*) /* Suppressing "Non const, non volatile static or external variable" lint warning*/
/* module includes ********************************************************** */

/* system header files */
#include <stdio.h>
#include "BCDS_Basics.h"

/* own header files */
#include "AcousticSensor.h"

/* additional interface header files */
#include "FreeRTOS.h"
#include "timers.h"
#include "BCDS_Assert.h"
#include "XdkSensorHandle.h"

#include "ADC_ih.h"
#include "PTD_portDriver_ih.h"
#include "PTD_portDriver_ph.h"

/* constant definitions ***************************************************** */

/* local variables ********************************************************** */
static xTimerHandle printTimerHandle = (xTimerHandle) 0;/**< variable to store timer handle*/
#define DELAY  UINT32_C(20)                  /**< Timer delay is represented by this macro */
#define TIMERBLOCKTIME  UINT32_C(0xffff)     /**< Macro used to define blocktime of a timer*/
#define STATUS_ERROR INT16_C(-1)              /**< Macro to represent ERROR status */
#define STATUS_SUCCESS INT16_C(0)            /**< Macro to represent SUCCESS status */
#define TIMER_NOT_ENOUGH_MEMORY            (-1L)/**<Macro to define not enough memory error in timer*/
#define TIMER_AUTORELOAD_ON             UINT32_C(1)             /**< Auto reload of timer is enabled*/
#define MILLISECONDS(x) ((portTickType) x / portTICK_RATE_MS)
#define SECONDS(x) ((portTickType) (x * 1000) / portTICK_RATE_MS)


unsigned long max = 1;
unsigned long prev = 0;

/** Enum values that represent step counter modes */
typedef enum acousticSensorMode_e {
	ACOUSTIC_MODE_NORMAL = 0, /**< Step counter NORMAL mode */
	ACOUSTIC_MODE_SENSITIVE = 1, /**< Step counter SENSITIVE mode */
	ACOUSTIC_MODE_ROBUST = 2, /**< Step counter ROBUST mode */
} stepCounterSensorMode_t;

/* inline functions ********************************************************* */

/* local functions ********************************************************** */
/**
 * @brief Gets the step counter value and prints through the USB printf on serial port
 *
 * @param[in] pvParameters Rtos task should be defined with the type void *(as argument)
 */
static void printAcousticSensor(xTimerHandle timerHandle) {
	ADC_singleAcq_t result = ADC_SINGLE_ACQ_DEFAULT;

	// ADC single sample channel 4
	result.adcChannel = adcSingleInpCh4;

	result.initSingle.acqTime = adcAcqTime16;

	ADC_pollSingleData(&result);

	unsigned long noise = (unsigned long) ADC_scaleAdcValue(&result);
	if(noise > max) {
		max = noise;
		printf("MAX: %lu mV\n\r",max);
	}

	if ((int) (noise - prev) >= 100) {
		printf("Previous: %lu\n\r",prev);
		printf("Current: %lu\n\r",noise);
		printf("Max now: %lu\n\r",max);
		char urlFull[50];
		sprintf(urlFull, "/api/v1.0/1/new_peak/?peak=%lu", noise);

		httpCall(urlFull);
	}
	prev = noise;

	/*printf("Max: %lu\n\r",max);*/
//	printf("Noise: %lu\n\r",noise);
	/*printf("ADC Acoustic Voltage: %lu mV\n\r",
			(unsigned long) ADC_scaleAdcValue(&result));*/
}

/* global functions ********************************************************* */

/* API documentation is in the header */
void acousticSensorInit(void) {
	// Sets the supply voltage for the acoustic sensor
	PTD_pinOutSet(PTD_PORT_AKU340_VDD, PTD_PIN_AKU340_VDD);

	ADC_node_t data;
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

	data.adcFreq = 7000000;
	data.registerBaseAddress = ADC0;
	data.cmuClk = cmuClock_ADC0;
	data.init = init;

	ADC_init(&data);

	xTimerHandle timerHandle = xTimerCreate((const char* const ) "My Timer", MILLISECONDS(50), pdTRUE, NULL, printAcousticSensor);

	if (NULL == timerHandle) {
		assert(pdFAIL);
		return;
	}
	BaseType_t timerResult = xTimerStart(timerHandle, MILLISECONDS(10));
	if (pdTRUE != timerResult) {
		assert(pdFAIL);
	}

}

/* API documentation is in the header */
void acousticSensorDeinit(void) {
	Retcode_T stepCounterReturnStatus = (Retcode_T) RETCODE_FAILURE;

	/* Checking if the timer handler is valid (not NULL)*/
	/*Assertion Reason:  "Step counter Timer is not valid." */
	assert(printTimerHandle != NULL);

	/* Deinitialise step counter module */
	stepCounterReturnStatus = StepCounter_deInit(xdkStepCounterSensor_Handle);

	if (RETCODE_OK == stepCounterReturnStatus) {
		/* Stop the created PSC timer */
		if (xTimerStop(printTimerHandle, TIMERBLOCKTIME) == pdFAIL) {
			assert(false);
		}
	} else {
		printf("Step counter deinitialisation failed \n\r");
	}
}

/** ************************************************************************* */

