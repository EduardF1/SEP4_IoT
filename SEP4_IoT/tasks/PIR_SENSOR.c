/*
 * PIR_SENSOR.c
 *
 * Created: 10/12/2020 11.27.25
 *  Author: Toma
 */ 

#include "PIR_SENSOR.h"
#include "macros.h"

static TaskHandle_t pirSensorTaskHandle;
static uint16_t peopleCount;
static hcsr501_p hcsr501_Instance = NULL;

void pirSensor_task(void* pvParameters);              // Declare the task function

static void setUp_pirSensor()
{
	hcsr501_Instance = hcsr501_create(&PORTE, PE5);   // Create a new instance of the driver
}

void createPIRsensorTask()
{
	peopleCount = 0;
	pirSensorTaskHandle = NULL;
	
	setUp_pirSensor();                                // Call set up
	
	xTaskCreate(pirSensor_task,                       // Create the FreeRTOS task
	(const portCHAR *) "PIR_SENSOR",
	configMINIMAL_STACK_SIZE,
	NULL,
	configMAX_PRIORITIES - 3,
	&pirSensorTaskHandle);
}

void pirSensor_task(void* pvParameters)
{
	for (;;)
	{
		if (hcsr501_Instance != NULL)                        // If there is an instance of the driver
		{
			if (hcsr501_isDetecting(hcsr501_Instance))       // If the driver is detecting movement
			{
				status_leds_shortPuls(led_ST3);              // Blink the yellow status LED
				peopleCount++;                               // Increment counter
				vTaskDelay(THREE_SECOND_DELAY);              // Wait 3 sec otherwise the counter will increment too fast
			}
		}
	}
}

uint16_t getPeopleCount()
{
	return peopleCount;
}