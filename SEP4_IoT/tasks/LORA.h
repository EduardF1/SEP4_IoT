/*
* LORA.h
*
* Created: 27/11/2020 19:44:17
*  Authors: Eduard, Toma
*/


#ifndef LORA_H_
#define LORA_H_


//	LoRa driver dependency
#include <lora_driver.h>
//	Lora transceiver interface dependency for accessing macro
#include <hal_defs.h>
//	Function pointers (IRegisterAccess) dependency
#include <ihal.h>
//	Standard I/O dependency
#include <stdio.h>
//	LED driver dependency
#include <status_leds.h>
#include <ATMEGA_FreeRTOS.h>
//	FreeRTOS queue dependency
#include <queue.h>
//	FreeRTOS semaphore dependency
#include <semphr.h>
//	Task dependency
#include <task.h>

//	Function to setup the Lora driver
void setUpLoraDriver();

//	Function to create the Lora task
void createLoraTask(QueueHandle_t sendingQueue,
                    MessageBufferHandle_t downlinkMessageBuffer,
                    QueueHandle_t rc_servo_queue,
                    SemaphoreHandle_t main_taskSyncSemphr,
					SemaphoreHandle_t mutexSemphr);


#endif /* LORA_H_ */