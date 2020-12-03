/*
 * LORA_DOWNLINK.h
 *
 * Created: 03/12/2020 22.02.42
 *  Author: Toma
 */ 


#ifndef LORA_DOWNLINK_H_
#define LORA_DOWNLINK_H_


#include <ATMEGA_FreeRTOS.h>
//	FreeRTOS queue dependency
#include <queue.h>
//	FreeRTOS semaphore dependency
#include <semphr.h>
//	Task dependency
#include <task.h>
// FreeRTOS Message buffer dependency
#include <message_buffer.h>
//	LoRa driver dependency
#include <lora_driver.h>
//	Standard I/O dependency
#include <stdio.h>

void createLoraDownlinkTask(MessageBufferHandle_t downlinkMessageBuffer, QueueHandle_t rc_servo_queue, SemaphoreHandle_t mutexSemphr);


#endif /* LORA_DOWNLINK_H_ */