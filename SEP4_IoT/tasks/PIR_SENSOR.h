/*
 * PIR_SENSOR.h
 *
 * Created: 10/12/2020 11.27.44
 *  Author: Toma
 */ 


#ifndef PIR_SENSOR_H_
#define PIR_SENSOR_H_


#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <status_leds.h>
#include <stdio.h>
#include <stdint.h>
#include <hcSr501.h>

void createPIRsensorTask();    // Method to create the sensor task
uint16_t getPeopleCount();     // Method to get the number of people (number of movements detected by the sensor)


#endif /* PIR_SENSOR_H_ */