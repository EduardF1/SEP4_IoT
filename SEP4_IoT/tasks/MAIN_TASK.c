/*
 * MAIN_TASK.c
 *
 * Created: 27/11/2020 03.21.36
 *  Author: Toma
 */ 

#include "MAIN_TASK.h"
#include "macros.h"
#include "LORA_PAYLOAD.h"
#include "CO2_Sensor.h"
#include "TEMP_HUM_SENSOR.h"
#include "LORA.h"

static TaskHandle_t main_task_task_handle = NULL;  // TaskHandle for the task
static EventGroupHandle_t pvEventHandleMeasure;    // Event group handle for the measure bits
static EventGroupHandle_t pvEventHandleNewData;    // Event group handle for the new data ready bits
static QueueHandle_t sendingQueue;                 // Queue used to send the Lora payload to the Lora up link task
static SemaphoreHandle_t main_taskSyncSemphr;      // Semaphore for blocking the main task until Lora is connected to the up link

// Main task function declaration
void mainTask(void *pvParameters);

void createMainTask()
{
	main_taskSyncSemphr = xSemaphoreCreateBinary(); // Create a binary semaphore
	
	pvEventHandleMeasure = xEventGroupCreate();   // Create an event group that triggers the sensors to measure
	pvEventHandleNewData = xEventGroupCreate();   // Create an event group that indicates that measurements are taken from the sensors
	
	sendingQueue = xQueueCreate(1, sizeof(lora_driver_payload_t));    // Create the Queue that sends the payload to Lora up link
	
	createLoraTask(sendingQueue, main_taskSyncSemphr);                // Create the Lora Task
	createCO2SensorTask(pvEventHandleMeasure, pvEventHandleNewData);  // Create the CO2 sensor task
	createTEMP_HUMTask(pvEventHandleMeasure, pvEventHandleNewData);   // Create the temperature and humidity sensor task
	
	// Create the main task in FreeRTOS
	xTaskCreate(mainTask,                         // function that implements the task body
	(const portCHAR *) "Application Controller",  // task name
	configMINIMAL_STACK_SIZE + 100,               // task stack size
	NULL,                                         // pvParameters
	configMAX_PRIORITIES - 3,                     // Task priority
	&main_task_task_handle);                      // Task handle
}

// Task function body
void mainTask(void *pvParameters)
{
	for (;;)
	{
		xSemaphoreTake(main_taskSyncSemphr, portMAX_DELAY); // Try to take the semaphore if it is given by the Lora up link Task
		
		// Set the bits in the HandleMeasure event group so that the sensors can start measuring
		xEventGroupSetBits(pvEventHandleMeasure, ALL_MEASURE_BITS);
		
		// Gets the value of all ready bits in the NewData event group
		EventBits_t bits = xEventGroupWaitBits(pvEventHandleNewData, ALL_READY_BITS, pdTRUE, pdTRUE, ONE_MINUTE_DELAY);
		
		// Check if all bits are set in NewData EventGroup, if yes then retrieve the latest measurements from the sensors
		if ((bits & ALL_READY_BITS) == ALL_READY_BITS)
		{
			uint16_t co2Measurement = getCO2();           // Get latest CO2 ppm value
			int16_t temperature = getTemperature();       // Get latest temperature value
			uint16_t humidity = getHumidity();            // Get latest humidity value
			
			setCo2PpmSensorData(co2Measurement);          // Set CO2 ppm value to the Lora payload
			setTemperatureSensorData(temperature);        // Set temperature value to the Lora payload
			setHumiditySensorData(humidity);              // Set humidity value to the Lora payload
			
			lora_driver_payload_t lora_payload = getLoraPayload(LORA_PAYLOAD_PORT_NO);  // Get the payload with the assigned port number
			
			// Print the new measurements from the sensors
			printf(" TEMP: %d \n HUMIDITY: %d \n CO2: %d \n", temperature, humidity, co2Measurement);
			
			xQueueSend(sendingQueue, (void *) &lora_payload, portMAX_DELAY); // Send the payload to the Lora up link task Queue
		}
		
		vTaskDelay(ONE_MINUTE_DELAY);  // Wait 1 minute, then loop over again
	}
}