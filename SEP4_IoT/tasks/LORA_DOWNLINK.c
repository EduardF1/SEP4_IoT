/*
 * LORA_DOWNLINK.c
 *
 * Created: 03/12/2020 22.02.58
 *  Author: Toma
 */ 

#include "LORA_DOWNLINK.h"
#include "macros.h"

//	Private fields for the TCB (task handle), queue handle and MessageBufferHandle
static TaskHandle_t _lora_downlink_task_handle;
static QueueHandle_t _rc_servo_queue;
static MessageBufferHandle_t _downlinkMessageBuffer;

//  Mutex semaphore
static SemaphoreHandle_t _mutexSemphr;

void loraDownlinkTask(void *pvParameters);

void createLoraDownlinkTask(MessageBufferHandle_t downlinkMessageBuffer, QueueHandle_t rc_servo_queue, SemaphoreHandle_t mutexSemphr)
{
	_downlinkMessageBuffer = downlinkMessageBuffer;
	_rc_servo_queue = rc_servo_queue;
	_mutexSemphr = mutexSemphr;
	_lora_downlink_task_handle = NULL;
	
	xTaskCreate(loraDownlinkTask,
	(const portCHAR *) "LoRaWanDownlink",
	configMINIMAL_STACK_SIZE + 100,
	NULL,
	configMAX_PRIORITIES - 1,
	&_lora_downlink_task_handle);
}

void loraDownlinkTask(void *pvParameters)
{
	static lora_driver_payload_t _lora_downlink_payload;
	static int8_t servoValue;
	
	for (;;)
	{
		if (_downlinkMessageBuffer != NULL)
		{
			xMessageBufferReceive(_downlinkMessageBuffer, &_lora_downlink_payload, sizeof(lora_driver_payload_t), portMAX_DELAY);
			
			if (_lora_downlink_payload.len > 0) // Check that we have got the expected bytes
			{
				xSemaphoreTake(_mutexSemphr, portMAX_DELAY);
				// Just for Debug
				printf("DOWN LINK: from port: %d with %d bytes received! \n", _lora_downlink_payload.port_no, _lora_downlink_payload.len);
				// decode the payload into our variables
				servoValue = _lora_downlink_payload.bytes[0];
				printf("VALUE FROM DOWNLINK: %d \n", servoValue);
				xSemaphoreGive(_mutexSemphr);
				
				if (_rc_servo_queue != NULL)
				{
					rcServo_Command_t cmd;
					
					if (servoValue == 20)
					{
						cmd = LOWER;
						xQueueSend(_rc_servo_queue, &cmd, portMAX_DELAY);
					}
					
					if (servoValue == 40)
					{
						cmd = RAISE;
						xQueueSend(_rc_servo_queue, &cmd, portMAX_DELAY);
					}
				}
			}
		}
	}
}