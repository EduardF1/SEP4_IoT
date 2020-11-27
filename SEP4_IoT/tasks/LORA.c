
/*
* LORA.c
*
* Created: 27/11/2020 18:51:13
*  Author: Eduard
*/


#include "macros.h"
#include "LORA.h"

//	Private fields for the TCB (task handle) and queue handle
static TaskHandle_t _lora_uplink_task_handle;
static QueueHandle_t _sendingQueue;

//	Control semaphore
static SemaphoreHandle_t _main_task_SyncSemphr;
//	Lora Driver return code (private field)
static lora_driver_returnCode_t rc;
//	Buffer 
static char _out_buf[100];

void loraUplinkTask(void *pvParameters);


void setUpLoraDriver()
{
	status_leds_slowBlink(led_ST2);
	
	// Factory reset the transceiver
	printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));
	
	// Configure to EU868 LoRaWAN standards
	printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);
	printf("Get HWEUI >%s<: %s\n",lora_driver_mapReturnCodeToText(rc), _out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf,
	lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf)));

	// Save all the MAC settings in the transceiver
	printf("Save mac >%s<\n",lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

	// Enable Adaptive Data Rate
	printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LORA_OTAA);
		printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

		if ( rc != LORA_ACCEPTED)
		{
			// Make the red led pulse to tell something went wrong
			status_leds_longPuls(led_ST1); // OPTIONAL
			// Wait 5 sec and lets try again
			vTaskDelay(pdMS_TO_TICKS(5000UL));
		}
		else
		{
			break;
		}
	} while (--maxJoinTriesLeft);

	if (rc == LORA_ACCEPTED)
	{
		// Connected to LoRaWAN :-)
		// Make the green led steady
		status_leds_ledOn(led_ST2); // OPTIONAL
	}
	else
	{
		// Something went wrong
		// Turn off the green led
		status_leds_ledOff(led_ST2); // OPTIONAL
		// Make the red led blink fast to tell something went wrong
		status_leds_fastBlink(led_ST1); // OPTIONAL

		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
}

void createLoraTask(QueueHandle_t pQueue, SemaphoreHandle_t main_taskSyncSemphr){
	
	_sendingQueue = pQueue;
	_main_task_SyncSemphr = main_taskSyncSemphr;
	_lora_uplink_task_handle = NULL;
	
	hal_create(7);	//	give the LED task priority 7
	lora_driver_create(LORA_USART, NULL);
	
	xTaskCreate(loraUplinkTask,
	(const portCHAR *) "LoRaWanUplink",
	configMINIMAL_STACK_SIZE + 200,
	NULL,
	configMAX_PRIORITIES - 1,
	&_lora_uplink_task_handle);
}

void loraUplinkTask(void *pvParameters){
	
	//	Hardware rest of LoRaWAN transceiver
	lora_driver_resetRn2483(1);	//	set reset state to 1 (active)
	vTaskDelay(2);	//	delay for 2 ms
	lora_driver_resetRn2483(0);	//	set reset state to 0 (released)
	//	Give it a chance to wakeup
	vTaskDelay(150);	//	delay for 150 ms
	
	lora_driver_flushBuffers();	//	get rid of the first version string from module after reset!
	
	setUpLoraDriver();	//	setup the driver upon task creation
	
	vTaskDelay(ONE_SECOND_DELAY);	//	delay for 1 S
	
	static lora_driver_payload_t _lora_uplink_payload;	//	declare the payload
	
	for(;;)
	{
		if(rc == LORA_ACCEPTED || rc == LORA_OK || rc == LORA_MAC_TX_OK || rc == LORA_MAC_RX || rc == LORA_NO_FREE_CH || rc == LORA_MAC_ERROR)
		{
			
		}
		
		if (_sendingQueue != NULL)
		{
			if(xQueueReceive(_sendingQueue, &_lora_uplink_payload, portMAX_DELAY) == pdPASS)	
			{
				status_leds_shortPuls(led_ST4);
				
				//	Send an upload message to the LoRaWAN, arguments: [confirmed true: Send confirmed, else unconfirmed.], payload pointer to payload to be sent.
				rc = lora_driver_sendUploadMessage(false, &_lora_uplink_payload);
				printf("Upload Message >%s<\n", lora_driver_mapReturnCodeToText(rc));
			}
		}
	}
	vTaskDelete(_lora_uplink_task_handle);
}