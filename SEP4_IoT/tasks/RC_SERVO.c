/*
 * RC_SERVO.c
 *
 * Created: 02/12/2020 21:46:43
 *  Author: fisch
 */ 

#include "RC_SERVO.h"
#include "macros.h"

//	Declare RC_SERVO task TCB
static TaskHandle_t RC_SERVOAcuatorTaskHandle;
//	Declare RC_SERVO queue (for receiving commands)
static QueueHandle_t _rc_servo_queue;

static SemaphoreHandle_t _taskSyncSemphr;
//	Flag to verify the state/position of the servo
static bool isRaised;

void rc_servoActuatorTask(void* pvParameters);

static void setupRC_SERVO()
{
	rc_servo_create();	//	Create and initialize the driver (see rc_servo.h)
	rc_servo_setPosition(0,0);	//	reset the driver upon setup
}

//	Function to create the RC_SERVO task externally
void createRC_SERVOTask(QueueHandle_t rc_servo_queue, SemaphoreHandle_t taskSyncSemphr)
{
	isRaised = false;	//	initialize the flag with false (default value)
	_rc_servo_queue = rc_servo_queue;	//	pass the given(argument) queue to the local/module queue field
	_taskSyncSemphr = taskSyncSemphr;	//	pass the given(argument) semaphore to the field semaphore
	RC_SERVOAcuatorTaskHandle = NULL;
	
	setupRC_SERVO();	//	initialize the driver
	
	//	Create the RC_SERVO task
	xTaskCreate(
	rc_servoActuatorTask,	//	Function that defines the task
	(const portCHAR*) "RC_SERVO",	//	Task name
	configMINIMAL_STACK_SIZE,	//	Stack size (in words)
	NULL,						
	configMAX_PRIORITIES - 3,	//	Task priority
	&RC_SERVOAcuatorTaskHandle);	//	TCB
}

//	Task function body definition
void rc_servoActuatorTask(void* pvParameters)
{
	for(;;)
	{
		static rcServo_Command_t command;	//	private local variable to trigger the servo
		
		if(_rc_servo_queue != NULL)	//	if the servo queue exists
		{
			if(xQueueReceive(_rc_servo_queue, &command, portMAX_DELAY) == pdPASS)	//	if there is a command to receive from the queue
			{
				if(command == RAISE)	//	raise/open shaft
				{
					
					if(isRaised == false)	//	if closed/lowered
					{
						rc_servo_setPosition(0,100);	//	adjust postion to {100 - fully to the right}
						isRaised = true;	//	set flag to true
					}
					else
					{
						xSemaphoreTake(_taskSyncSemphr, portMAX_DELAY);
						printf("SHAFT IS ALREADY RAISED\n");
						xSemaphoreGive(_taskSyncSemphr);
					}
				}
				
				else if(command == LOWER)
				{
					if(isRaised == true)
					{
						rc_servo_setPosition(0, -100);	//	adjust position to {-100 - fully to the left}
							isRaised = false;	//	set flag to false after lowering it
					}
					else
					{
						xSemaphoreTake(_taskSyncSemphr, portMAX_DELAY);
						printf("SHAFT IS ALREADY LOWERED\n");
						xSemaphoreGive(_taskSyncSemphr);
					}
				}
			}
		}
	}
}