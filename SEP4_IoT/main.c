/*
* main.c
* Author : Eduard, Toma
*
*/

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <ATMEGA_FreeRTOS.h>
#include <FreeRTOSTraceDriver.h>
#include <stdio_driver.h>
#include <serial.h>
#include "MAIN_TASK.h"


int main(void)
{
	DDRA |= _BV(DDA0) | _BV(DDA1) | _BV(DDA2) | _BV(DDA3) | _BV(DDA4) | _BV(DDA5) | _BV(DDA6) | _BV(DDA7);
	PORTA ^= _BV(PA0);
	PORTA ^= _BV(PA1);
	PORTA ^= _BV(PA2);
	PORTA ^= _BV(PA3);
	PORTA ^= _BV(PA4);
	PORTA ^= _BV(PA5);
	PORTA ^= _BV(PA6);
	PORTA ^= _BV(PA7);
	
	trace_init();
	stdio_create(ser_USART0);        // Create stdio driver
	sei();                           // Enable global interrupt
	printf("Program executed! \n");
	createMainTask();                // Creates the main task
	vTaskStartScheduler();           // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}
