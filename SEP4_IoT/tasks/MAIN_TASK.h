/*
 * MAIN_TASK.h
 *
 * Created: 27/11/2020 03.21.24
 *  Author: Toma
 */ 


#ifndef MAIN_TASK_H_
#define MAIN_TASK_H_


// Interface dependencies
#include <stdio.h>
#include <avr/io.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <message_buffer.h>
#include <stdio_driver.h>
#include <display_7seg.h>


// Method that creates the main task
void createMainTask();
void countDownToNextLoop();


#endif /* MAIN_TASK_H_ */