/* system header files */
#include "BCDS_Basics.h"
#include <stdio.h>
#include <stdlib.h>

#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "BCDS_Assert.h"
#include "PTD_portDriver_ih.h"
#include "PTD_portDriver_ph.h"


/* own header files */
#include "NetworkDiscovery.h"

void httpCall(char *url);

static void scanNetwork(void)
{
    /* local variables */
    Retcode_T retScanStatus[MAX_SIZE_BUFFER];
    WlanConnect_ScanInterval_T scanInterval = SCAN_INTERVAL;
    WlanConnect_ScanList_T scanList;
    char macs[150] = "/api/v1.0/1/nearby_network/?mac=";

    retScanStatus[0] = WlanConnect_ScanNetworks(scanInterval, &scanList);

    if (RETCODE_OK == retScanStatus[0])
    {
        printf("\n\r[NCA] : Hey User! XDK found the following networks:\n\r");

        int counter = 0;
        for (int i = ZERO; i < WLANCONNECT_MAX_SCAN_INFO_BUF; i++)
        {
        	counter++;
        	if(counter == 4) {
        		break;
        	}
            if (ZERO != scanList.ScanData[i].SsidLength)
            {
            	char mac[20] = "\0";
            	for(int j=0; j<6; j++) {
            		sprintf(mac, "%s%x_", mac, scanList.ScanData[i].Bssid[j]); // puts string into buffer
            	}
            	printf("String: %s\n\r",mac);
            	if(counter == 1) {
            		sprintf(macs,"%s%s",macs,mac);
            	} else {
            		sprintf(macs,"%s,%s",macs,mac);
            	}

                printf("[NCA] :  - found SSID number %d is : %s\n\r", i, scanList.ScanData[i].Ssid);

                static_assert((portTICK_RATE_MS != 0), "Tick rate MS is zero");
                vTaskDelay((portTickType) DELAY_500_MSEC / portTICK_RATE_MS);
            }
        }
        printf("Location URL: %s\n\r",macs);
        printf("[NCA] : Finished scan successfully \n\r");
        httpCall(macs);
    }
    else if ((Retcode_T) RETCODE_NO_NW_AVAILABLE == Retcode_getCode(retScanStatus[0]))
    {
        printf("[NCA] : Scan function did not found any network\n\r");
    }
    else /*(RETCODE_FAILURE == retScanStatus[0])*/
    {
        printf("[NCA] : Scan function failed\n\r");
    }
}
