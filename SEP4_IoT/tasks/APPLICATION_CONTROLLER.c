/*
 * application_controller.c
 *
 * Created: 23/11/2020 00:52:06
 *  Author: fisch
 */ 

#include <event_groups.h>


static EventGroupHandle_t pvEventHandleMeasure;
static EventGroupHandle_t pvEventHandleNewData;

//	define creation of application controller
void createApplicationControllerTask(){
	
	//	initialize measurement and retrieval event groups
	pvEventHandleMeasure = xEventGroupCreate();
	pvEventHandleNewData = xEventGroupCreate();
	
	
	//	declare CO2 task creation
	createCO2SensorTask(pvEventHandleMeasure, pvEventHandleNewData);
}
