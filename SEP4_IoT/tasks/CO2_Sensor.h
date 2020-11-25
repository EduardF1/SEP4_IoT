/*
 * CO2_Sensor.h
 *
 * Created: 23/11/2020 00:59:45
 *  Author: fisch
 */ 

#ifndef CO2_SENSOR_H_
#define CO2_SENSOR_H_


#include <ATMEGA_FreeRTOS.h>
//	Event groups interface dependency
#include <event_groups.h>
//	Task dependency
#include <task.h>
//	Standard int lib. dependency
#include <stdint.h>
//	CO2 sensor dependency
#include <mh_z19.h>


//	Callback function for new values measured by the CO2 sensor
//	ppm - value to be returned
void CO2Sensor_callBack(uint16_t ppm);

//	Function to initialize the CO2 driver
void setupCO2Driver();

//	Function to create the CO2 sensor task, takes 2 event group 
//	pointers as arguments for measurement and data retrieval
void createCO2SensorTask(EventGroupHandle_t pvEventHandleMeasure, EventGroupHandle_t pvEventHandleNewData);

//	Function to retrieve the last measured CO2 value
uint16_t getCO2;

#endif	CO2_SENSOR_H_
