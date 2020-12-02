/*
* LORA.h
*
* Created: 27/11/2020 19:44:17
*  Author: Eduard
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

//	Function to create the Lora task, takes a queue and a semaphore as arguments
void createLoraTask(QueueHandle_t sendingQueue, SemaphoreHandle_t main_taskSyncSemphr);


#endif /* LORA_H_ */