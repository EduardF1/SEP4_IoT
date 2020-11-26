/*
* main.c
* Author : Eduard
*
*/

#include <stdio.h>
#include <avr/io.h>
#include <ATMEGA_FreeRTOS.h>
#include <FreeRTOSTraceDriver.h>
#include <stdio_driver.h>
#include <serial.h>

#include <event_groups.h>
#include "TEMP_HUM_SENSOR.h"

//	Private event group variables for synchronization of task measurement and data collection
static EventGroupHandle_t pvEventHandleMeasure;
static EventGroupHandle_t pvEventHandleNewData;


void initializeSystem(){
	
	pvEventHandleMeasure = xEventGroupCreate();
	pvEventHandleNewData = xEventGroupCreate();
	createTEMP_HUMTask(pvEventHandleMeasure, pvEventHandleNewData);
}

/*-----------------------------------------------------------*/
int main(void)
{
	
	trace_init();
	stdio_create(ser_USART0);
	printf("Program Started!!\n");
	initializeSystem();
	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.


	while (1)
	{
	}
}

