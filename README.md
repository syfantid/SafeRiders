# SafeRiders
SafeRiders' finalist submission at START Hack 2017

Includes functions to collect, analyze and send acoustic data from Bosch XDK (acoustic sensor).

## Inspiration
* Bosch & Volvo challenged us to improve car experience using technology
* 37 kids die on average every year in the U.S.A. due to heatstroke while forgotten in cars; 1 kid every 9 days; 702 kids since 1998
* It can happen to anyone; it's not negligence, it's nature; our memory is intrinsically flawed
* We have the technology and skills to change this; there's no excuse for preventable deaths

## What it does
* Places Bosch sensors near backseats where baby car seats are usually placed
* Detects unusual sounds inside the car
* Ignores noise from the outer environment (horns, barking, etc.)
* Notifies the guardian on their smart phone about an emergency situation
* Provides emergency phone numbers utilizing open APIs
* Adds value for family cars

## How we built it
* Programmed and worked with the Bosch XDK and its acoustic sensor to identify sounds in the car's interior (Includes functions to collect, analyze and send acoustic data from Bosch XDK)
* Deployed a Django back-end infrastructure to sore information and process signals
* Established communication between the sensor and the server, as well as the server and the mobile application, over HTTP
* Developed an Android application to notify the user in case of emergency and provide useful information e.g. emergency phone numbers

## Challenges we ran into
* No interface for the acoustic sensor to work on; we developed it from scratch
* Connectivity problems between the sensor and the WiFi; weak signal
* Minor complications using the IDE
* Sleep deprivation :P 

## Accomplishments that we're proud of
* Managed to complete a full working prototype that demonstrates the full work-flow of the project in less than 48 hours
* Came up with a realistic and immediate applicable solution for eliminating children's deaths caused by heatstroke in cars
* Discover the minimum amount of sleep a brain needs to function :P

## What we learned
* How to programme a sensor set
* How to enable communication between different programs, technologies and devices
* How to adapt ourselves to new environments, ideas and products
* How to work with the community

## What's next for SafeRiders
* Constant improvements and additions to our product to ensure its high quality

