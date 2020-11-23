/*
 * temp_hum_sensor.h
 *
 * Created: 23/11/2020 01:09:55
 *  Author: fisch
 */ 

#ifndef	TEMP_HUM_SENSOR_H_
#define TEMP_HUM_SENSOR_H_ 

//	Add driver dependency
#include <hih8120.h>
//	Add event groups dependency
#include <event_groups.h>


/*	A)	Creation of the TEMP_HUM task, takes as arguments two event groups (for synchronization)
	1)	pvEventHandleMeasure - pointer variable for the measurement event group (will be used together with the CO2 task)
	2)	pvEventHandleNewData - pointer variable for the data ready event group (will be used together with the CO2 task)
	
	**** See event_groups.h lines 610 - 688 ****
*/
void createTEMP_HUMTask(EventGroupHandle_t pvEventHandleMeasure, EventGroupHandle_t pvEventHandleNewData);

//	function to return the newest value for the measured temperature from HIH8120 (temperature and humidity driver)
int16_t getTemperature();

//	function to return the newest value for the measured humidity from HIH8120 (temperature and humidity driver)
uint16_t getHumidity();

#endif	TEMP_HUM_SENSOR_H_