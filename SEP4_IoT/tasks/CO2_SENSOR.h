/*
 * CO2_SENSOR.h
 *
 * Created: 26/11/2020 19.23.35
 *  Author: Toma
 */ 


#ifndef CO2_SENSOR_H_
#define CO2_SENSOR_H_

// Interface dependencies
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>
#include <task.h>
#include <stdint.h>
#include <mh_z19.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_driver.h>
#include <avr/io.h>


// Call back function used for the MH_Z19 driver
void CO2Sensor_callBack(uint16_t ppm);

// Method to setup the MH_Z19 driver
void setupCO2Driver();

/*	A)	Creation of the CO2 task, takes as arguments two event groups (for synchronization)
	1)	pvEventHandleMeasure - variable for the measurement event group (will be used together with the TEMP_HUM task)
	2)	pvEventHandleNewData - variable for the data ready event group (will be used together with the TEMP_HUM task)
	
	**** See event_groups.h lines 610 - 688 ****
*/
void createCO2SensorTask(EventGroupHandle_t pvEventHandleMeasure, EventGroupHandle_t pvEventHandleNewData);

// function to return the newest value for the measured CO2 from the MH_Z19 driver
uint16_t getCO2();


#endif /* CO2_SENSOR_H_ */