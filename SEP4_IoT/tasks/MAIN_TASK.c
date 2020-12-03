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
#include "LORA_DOWNLINK.h"
#include "RC_SERVO.h"

static TaskHandle_t main_task_task_handle = NULL;          // TaskHandle for the task
static EventGroupHandle_t pvEventHandleMeasure;            // Event group handle for the measure bits
static EventGroupHandle_t pvEventHandleNewData;            // Event group handle for the new data ready bits
static QueueHandle_t sendingQueue;                         // Queue used to send the Lora payload to the Lora up link task
static QueueHandle_t rc_servo_queue;                       // Queue used to send servo commands to the RC_SERVO task
static SemaphoreHandle_t main_taskSyncSemphr;              // Semaphore for blocking the main task until Lora is connected to the up link
static SemaphoreHandle_t mutexSemphr;                      // Semaphore for printing the servo commands
static MessageBufferHandle_t downlinkMessageBuffer;        // Message buffer for the Lora down link
static const size_t downlinkMessageBufferSizeBytes = 30;   // The message buffer size in bytes
static size_t remainingHeapSpace;                          // Variable to hold the remaining heap space of the program

// Main task function declaration
void mainTask(void *pvParameters);

void createMainTask()
{
	main_taskSyncSemphr = xSemaphoreCreateBinary();        // Create a binary semaphore
	mutexSemphr = xSemaphoreCreateMutex();                 // Create a Mutex semaphore
	
	pvEventHandleMeasure = xEventGroupCreate();   // Create an event group that triggers the sensors to measure
	pvEventHandleNewData = xEventGroupCreate();   // Create an event group that indicates that measurements are taken from the sensors
	
	sendingQueue = xQueueCreate(1, sizeof(lora_driver_payload_t));    // Create the Queue that sends the payload to Lora up link
	rc_servo_queue = xQueueCreate(1, sizeof(rcServo_Command_t));      // Create the Queue for servo commands
	
	downlinkMessageBuffer = xMessageBufferCreate(downlinkMessageBufferSizeBytes);  // Create the message buffer
	
	if (main_taskSyncSemphr != NULL &&
	    mutexSemphr != NULL &&
		pvEventHandleMeasure != NULL &&
		pvEventHandleNewData != NULL &&
		sendingQueue != NULL &&
		rc_servo_queue != NULL &&
		downlinkMessageBuffer != NULL)
	{
		xSemaphoreGive(mutexSemphr);  // Make the Mutex available for use, by initially "Giving" the Semaphore.
		display_7seg_init(NULL);                                          // Initialize display driver
		display_7seg_powerUp();                                           // Set to power up mode
		remainingHeapSpace = xPortGetFreeHeapSize();                      // Get the total amount of heap space that remains unallocated
		display_7seg_display((float)remainingHeapSpace, 0);               // Display it on the 7 segment display for information
		
		createLoraTask(sendingQueue, downlinkMessageBuffer, main_taskSyncSemphr, mutexSemphr);
		createLoraDownlinkTask(downlinkMessageBuffer, rc_servo_queue, mutexSemphr);
		createCO2SensorTask(pvEventHandleMeasure, pvEventHandleNewData);  // Create the CO2 sensor task
		createTEMP_HUMTask(pvEventHandleMeasure, pvEventHandleNewData);   // Create the temperature and humidity sensor task
		createRC_SERVOTask(rc_servo_queue, mutexSemphr);                  // Create the servo task
		
		// Create the main task in FreeRTOS
		xTaskCreate(mainTask,                         // function that implements the task body
		(const portCHAR *) "Application Controller",  // task name
		configMINIMAL_STACK_SIZE + 100,               // task stack size
		NULL,                                         // pvParameters
		configMAX_PRIORITIES - 3,                     // Task priority
		&main_task_task_handle);                      // Task handle
	}
	else
	{
		// There was not enough heap memory space available.
		printf("There was not enough heap memory space available to create all tasks. \n");
	}
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
			uint8_t shaftStatus = getShaftStatus();       // Get the current shaft status
			
			setCo2PpmSensorData(co2Measurement);          // Set CO2 ppm value to the Lora payload
			setTemperatureSensorData(temperature);        // Set temperature value to the Lora payload
			setHumiditySensorData(humidity);              // Set humidity value to the Lora payload
			setCurrentShaftStatus(shaftStatus);           // Set shaft status to the Lora payload
			
			lora_driver_payload_t lora_payload = getLoraPayload(LORA_PAYLOAD_PORT_NO);  // Get the payload with the assigned port number
			
			// Print the new measurements from the sensors and shaft status
			printf(" TEMP: %d \n HUMIDITY: %d \n CO2: %d \n SHAFT: %d \n", temperature, humidity, co2Measurement, shaftStatus);
			
			xQueueSend(sendingQueue, (void *) &lora_payload, portMAX_DELAY); // Send the payload to the Lora up link task Queue
		}
		
		vTaskDelay(ONE_MINUTE_DELAY);  // Wait 1 minute, then loop over again
		
		remainingHeapSpace = xPortGetFreeHeapSize();          // Get the total amount of heap space that remains unallocated
		display_7seg_display((float)remainingHeapSpace, 0);   // Display it on the 7 segment display for information
	}
}