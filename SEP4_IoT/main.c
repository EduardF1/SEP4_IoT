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
#include "MAIN_TASK.h"

/*-----------------------------------------------------------*/
int main(void)
{
	trace_init();
	stdio_create(ser_USART0);
	printf("Program executed! \n");
	createMainTask();                // Creates the main task
	vTaskStartScheduler();           // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}
