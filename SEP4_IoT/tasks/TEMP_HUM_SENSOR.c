/*
 * TEMP_HUM_SENSOR.c
 *
 * Created: 23/11/2020 11:07:08
 *  Author: fisch
 */ 

#include "TEMP_HUM_SENSOR.h"
//////////////////////////////////////////////////////////////////////////
//	TO DO: synchronization with measurement and new data event groups
//////////////////////////////////////////////////////////////////////////

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

static SemaphoreHandle_t xTestSemaphore;

//	Function for initializing the driver
static void setupTEMP_HUMDriver(){
		if(HIH8120_OK == hih8120_create())
		{
			xTestSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
			if ( ( xTestSemaphore ) != NULL )
			{
				printf("HIH8120 driver successfully created/n"); // stdio functions are not reentrant (it blocks)- Should normally be protected by MUTEX
				xSemaphoreGive( ( xTestSemaphore ) );  // Make the mutex available for use, by initially "Giving" the Semaphore.
			}
		}
}

/*	A)	Creation of the TEMP_HUM task, takes as arguments two event groups (for synchronization)
	1)	pvEventHandleMeasure - pointer variable for the measurement event group (will be used together with the CO2 task)
	2)	pvEventHandleNewData - pointer variable for the data ready event group (will be used together with the CO2 task)
	
	**** See event_groups.h lines 610 - 688 ****
*/
//	function to create the TEMP_HUM_SENSOR task externally
void createTEMP_HUMTask(EventGroupHandle_t pvEventHandleMeasure, EventGroupHandle_t pvEventHandleNewData){
	_pvEventHandleMeasure = pvEventHandleMeasure;
	_pvEventHandleNewData = pvEventHandleNewData;
	
		//	Initialize private fields with 0
		lastTemperature = 0;
		lastHumidity = 0;
		
		//	Upon task creation, initialize HIH8120
		//	Set task priority to 1 (4 by default - 3)
		//	configMAX priorities: https://www.freertos.org/a00110.html
		//	task priority: https://www.freertos.org/RTOS-task-priority.html
		//	temp_humSensorTask() - function that defines the task
		
		setupTEMP_HUMDriver();
		
		xTaskCreate(
		temp_humSensorTask(),	//	function that implements the task
		(const portCHAR*) "TEMP_HUM",	//	task name
		configMINIMAL_STACK_SIZE,		
		NULL,
		configMAX_PRIORITIES - 3,	
		&_TEMP_HUMSensorTaskHandle);
}

//	function to create the TEMP_HUM_SENSOR task externally
void temp_humSensorTask(){
	for(;;){
		
			hih8120_returnCode = hih8120_wakeup();
			if ( HIH8120_OK != hih8120_returnCode )
			{
				// Something went wrong
				// Investigate the return code further
			}
			vTaskDelay(1000UL/15);	//	delay for 66.(6) ms, the driver needs minimum 50 ms

			hih8120_returnCode = hih8120_measure();
			if ( HIH8120_OK !=  hih8120_returnCode)
			{
			// Something went wrong
			// Investigate the return code further
			}
			vTaskDelay(1000UL/100);	//	delay for 10 ms
			
			//	Get the measurements
			if( hih8120_returnCode == HIH8120_OK)
			{
				lastTemperature = hih8120_getTemperature_x10();	//	return Temperature C [x10], returned value : int16_t
				lastHumidity = hih8120_getHumidityPercent_x10();		//	return Relative humidity % [x10], returned value : uint16_t	
			}
		}
	vTaskDelete(_TEMP_HUMSensorTaskHandle); //	delete the task by passing the TCB to vTaskDelete
}

//	function to return the newest value for the measured temperature from HIH8120 (temperature and humidity driver)
int16_t getTemperature(){
	return lastTemperature;
}

//	function to return the newest value for the measured humidity from HIH8120 (temperature and humidity driver)
uint16_t getHumidity(){
	return lastHumidity;
}