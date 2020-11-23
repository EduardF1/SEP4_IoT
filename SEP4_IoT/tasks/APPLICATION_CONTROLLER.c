/*
 * application_controller.c
 *
 * Created: 23/11/2020 00:52:06
 *  Author: fisch
 */ 

//	Add event group dependency
#include <event_groups.h>
#include "APPLICATION_CONTROLLER.h"

//	Private fields (for the event groups that
//	will be used to synchronize the sensor measurements
//	and data retrieval
static EventGroupHandle_t pvEventHandleMeasure;
static EventGroupHandle_t pvEventHandleNewData;

//	define creation of application controller
void createApplicationControllerTask(){
	
	//	initialize measurement and retrieval event groups
	pvEventHandleMeasure = xEventGroupCreate();
	pvEventHandleNewData = xEventGroupCreate();
	
	
	//	declare CO2 task creation
	createCO2SensorTask(pvEventHandleMeasure, pvEventHandleNewData);
	//	declare TEMP_HUM task creation
	createTEMP_HUMTask(pvEventHandleMeasure, pvEventHandleNewData);
}
