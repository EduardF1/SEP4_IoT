/*
 * RC_SERVO.h
 *
 * Created: 02/12/2020 21:22:53
 *  Author: Eduard
 */ 


#ifndef RC_SERVO_H_
#define RC_SERVO_H_

#include <ATMEGA_FreeRTOS.h>
//	Task dependency
#include <task.h>
#include <stdio.h>
//	Queue dependency (queue needed for receiving commands)
#include <queue.h>
//	RC SERVO driver dependency
#include <rc_servo.h>
#include <semphr.h>


//	function to create the RC_SERVO task externally, takes as arguments a queue (to receive commands) and a semaphore for task activation
void createRC_SERVOTask(QueueHandle_t rc_servo_queue, SemaphoreHandle_t taskSyncSemphr);

//	function to return the shaft status {0 - lowered / 1 - raised}
int16_t getShaftStatus();

#endif /* RC_SERVO_H_ */