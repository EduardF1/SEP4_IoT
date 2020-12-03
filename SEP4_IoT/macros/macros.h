/*
* macros.h
*
* Created: 27/11/2020 03.15.26
*  Author: Toma
*/


#ifndef MACROS_H_
#define MACROS_H_


// Declare global macros to use for convenience
#define LORA_appEUI "CC079E3308C9825F"
#define LORA_appKEY "6E4724962ED591537C67297734655ED2"
#define LORA_PAYLOAD_PORT_NO 2                                     // Lora payload port number
#define TEMP_HUM_MEASURE_BIT (1 << 0)                              // set bit 0 for measurement event group
#define CO2_MEASURE_BIT (1 << 1)                                   // set bit 1 for measurement event group
#define ALL_MEASURE_BITS (TEMP_HUM_MEASURE_BIT | CO2_MEASURE_BIT)  // all measure bits
#define TEMP_HUM_READY_BIT (1 << 0)                                // set bit 0 for new data event group
#define CO2_READY_BIT (1 << 1)                                     // set bit 1 for new data event group
#define ALL_READY_BITS (TEMP_HUM_READY_BIT | CO2_READY_BIT)        // all ready bits
#define ONE_SECOND_DELAY pdMS_TO_TICKS(1000UL)
#define TWO_SECOND_DELAY pdMS_TO_TICKS(2000UL)
#define TEN_SECOND_DELAY pdMS_TO_TICKS(10000UL)
#define ONE_MINUTE_DELAY pdMS_TO_TICKS(60000UL)
#define FIVE_MINUTE_DELAY pdMS_TO_TICKS(300000UL)

//	Enum variable to determine the position of the RC SERVO actuator
//	Lower - 0 / Raise - 1
typedef enum rcServo_Command{
	LOWER,
	RAISE
}rcServo_Command_t;


#endif /* MACROS_H_ */