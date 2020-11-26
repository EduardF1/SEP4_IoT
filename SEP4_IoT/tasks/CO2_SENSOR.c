/*
 * CO2_SENSOR.c
 *
 * Created: 26/11/2020 19.23.49
 *  Author: Toma
 */ 

#include "CO2_Sensor.h"

#define CO2_MEASURE_BIT (1 << 1) //	set bit 1 for measurement event group
#define CO2_READY_BIT (1 << 1)   // set bit 1 for new data event group

// private fields to store the latest measurements
static uint16_t lastCO2ppm;

// private fields (Event group handlers for measurement and data ready bits)
static EventGroupHandle_t _pvEventHandleMeasure;
static EventGroupHandle_t _pvEventHandleNewData;

// MH_Z19 driver return code for verification
static mh_z19_returnCode_t returnCode;

// Task handle for the CO2 task
static TaskHandle_t _CO2SensorTaskHandle;

// The CO2 task function declaration
void co2SensorTask(void *pvParameters);

// CallBack function to set the CO2 ppm
void CO2Sensor_callBack(uint16_t ppm)
{
	lastCO2ppm = ppm;
}

// Create the MH_Z19 driver
void setupCO2Driver()
{
	mh_z19_create(ser_USART3, CO2Sensor_callBack);
}

// Create the CO2 sensor task
void createCO2SensorTask(EventGroupHandle_t pvEventHandleMeasure, EventGroupHandle_t pvEventHandleNewData)
{
	_pvEventHandleMeasure = pvEventHandleMeasure; // Set the EventGroupHandle for measure bits
	_pvEventHandleNewData = pvEventHandleNewData; // Set the EventGroupHandle for new data bits
	lastCO2ppm = 0;
	_CO2SensorTaskHandle = NULL;
	
	setupCO2Driver(); // Call the setup function
	
	// Create the FreeRTOS task
	xTaskCreate(
	co2SensorTask,               //	function that implements the task body
	(const portCHAR *) "CO2",    //	task name
	configMINIMAL_STACK_SIZE,    //	task stack size
	NULL,                        // pvParameters
	configMAX_PRIORITIES - 3,    // Task priority
	&_CO2SensorTaskHandle);      // Task handle
}

// Task function body
void co2SensorTask(void *pvParameters)
{
	for(;;)
	{
		/*	parameters:
		1)	_pvEventHandleMeasure - event group identifier
		2)	CO2_MEASURE_BIT - the bit to wait for
		3)	pdTRUE - clear bits before return
		4)	pdTRUE - wait for all bits to be set
		5)	portMAX_DELAY - delay indefinitely
		*/
		
		xEventGroupWaitBits(_pvEventHandleMeasure, CO2_MEASURE_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
		
		// Take measuring and get the return code
		returnCode = mh_z19_takeMeassuring();
		
		// If return code is OK then set the ready bit
		if (returnCode == MHZ19_OK)
		{
			xEventGroupSetBits(_pvEventHandleNewData, CO2_READY_BIT);
		}
	}
}

// Function to get the latest CO2 ppm
uint16_t getCO2()
{
	return lastCO2ppm;
}