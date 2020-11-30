/*
* TEMP_HUM_SENSOR.c
*
* Created: 23/11/2020 11:07:08
*  Author: fisch
*/

#include "TEMP_HUM_SENSOR.h"
#include "macros.h"

//	private fields to store the latest measurements
static int16_t lastTemperature;
static uint16_t lastHumidity;

//	private fields (Event group handlers for measuring and retrieving the data)
static EventGroupHandle_t _pvEventHandleMeasure;
static EventGroupHandle_t _pvEventHandleNewData;

/*	HIH810 driver return code, needs to be verified
continuously
*/
static hih8120_driverReturnCode_t hih8120_returnCode;
//	declare TEMP_HUM_SENSOR task TCB
static TaskHandle_t _TEMP_HUMSensorTaskHandle;

void temp_humSensorTask(void *pvParameters);

/*	A)	Creation of the TEMP_HUM task, takes as arguments two event groups (for synchronization)
1)	pvEventHandleMeasure - pointer variable for the measurement event group (will be used together with the CO2 task)
2)	pvEventHandleNewData - pointer variable for the data ready event group (will be used together with the CO2 task)

**** See event_groups.h lines 610 - 688 ****
*/
//	function to create the TEMP_HUM_SENSOR task externally
void createTEMP_HUMTask(EventGroupHandle_t pvEventHandleMeasure, EventGroupHandle_t pvEventHandleNewData)
{
	_pvEventHandleMeasure = pvEventHandleMeasure;
	_pvEventHandleNewData = pvEventHandleNewData;
	_TEMP_HUMSensorTaskHandle = NULL;
	
	//	Initialize private fields with 0
	lastTemperature = 0;
	lastHumidity = 0;
	
	//	Upon task creation, initialize HIH8120
	//	Set task priority to 1 (4 by default - 3)
	//	configMAX priorities: https://www.freertos.org/a00110.html
	//	task priority: https://www.freertos.org/RTOS-task-priority.html
	//	temp_humSensorTask() - function that defines the task
	
	if(HIH8120_OK == hih8120_create())
	{
		xTaskCreate(
		temp_humSensorTask,	//	function that implements the task body
		(const portCHAR*) "TEMP_HUM",	//	task name
		configMINIMAL_STACK_SIZE,		//	task stack size (in words)
		NULL,	//	parameters
		configMAX_PRIORITIES - 3,	//	priority at which the task is created
		&_TEMP_HUMSensorTaskHandle);	//	task handle (TCB)
	}
}

//	function to create the TEMP_HUM_SENSOR task body
void temp_humSensorTask(void *pvParameters)
{
	for(;;)
	{
		/*	parameters:
		1)	_pvEventHandleMeasure - event group identifier
		2)	TEMP_HUM_MEASURE_BIT - bits waited for
		3)	pdTRUE - set true for clear bits before return
		4)	pdTRUE - set true for wait bits to be set
		5)	portMAX_DELAY - delay indefinitely
		*/
		
		xEventGroupWaitBits(_pvEventHandleMeasure, TEMP_HUM_MEASURE_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
		
		hih8120_returnCode = hih8120_wakeup();	// after wakeup call, > 50 ms are needed for the sensor to be ready
		vTaskDelay(50);	//	delay for 50 ms, the driver needs minimum 50 ms

		hih8120_returnCode = hih8120_measure();	// after measurement call, > 1 ms is needed to feth the result from the sensor
		vTaskDelay(10);	//	delay for 10 ms (a measurement requires > 1 ms to poll the result from the sensor)

		//	Verify HIH8120 return code (HIH8120_OK if the measurement was done)
		//	If the measurement was not done, repeat (try again) for up to 10 attempts

		if(HIH8120_OK != hih8120_returnCode)
		{
			int flag = 0;
			do
			{
				hih8120_returnCode = hih8120_measure();	//	attempt for measurement
				vTaskDelay(10);	//	delay for 10 ms (a measurement requires > 1 ms to poll the result from the sensor)
				flag++;
			} while ((flag < 10) && (HIH8120_TWI_BUSY == hih8120_returnCode));	//	up to 10 times (and if the two wire/I2C interface is busy)
		}
		else if(hih8120_returnCode == HIH8120_OK)	//	if the initial measurement did occur
		{
			lastTemperature = hih8120_getTemperature_x10();	//	return Temperature C [x10], returned value : int16_t
			lastHumidity = hih8120_getHumidityPercent_x10();		//	return Relative humidity % [x10], returned value : uint16_t
			xEventGroupSetBits(_pvEventHandleNewData, TEMP_HUM_READY_BIT);	//	set bit 0 in the New Data event group (Synchronize the new measurement of the HIH8120 sensor)
		}
	}
}

//	function to return the newest value for the measured temperature
int16_t getTemperature()
{
	return lastTemperature;
}

//	function to return the newest value for the measured humidity
uint16_t getHumidity()
{
	return lastHumidity;
}