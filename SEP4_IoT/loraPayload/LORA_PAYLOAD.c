/*
 * LORA_PAYLOAD.c
 *
 * Created: 26/11/2020 19.31.30
 *  Author: Toma
 */ 

#include "LORA_PAYLOAD.h"

static uint16_t co2ppm;
static uint16_t _humidity;
static int16_t _temperature;
static int8_t _shaftStatus;

void setCo2PpmSensorData(uint16_t ppm)
{
	co2ppm = ppm;
}

void setHumiditySensorData(uint16_t humidity)
{
	_humidity = humidity;
}

void setTemperatureSensorData(int16_t temperature)
{
	_temperature = temperature;
}

void setCurrentShaftStatus(int8_t shaftStatus)
{
	_shaftStatus = shaftStatus;
}

lora_driver_payload_t getLoraPayload(uint8_t port_no)
{
	lora_driver_payload_t _uplink_payload = {.len = 8, .bytes = {0}, .port_no = port_no};
	
	_uplink_payload.bytes[0] = _temperature >> 8;
	_uplink_payload.bytes[1] = _temperature & 0xFF;
	_uplink_payload.bytes[2] = _humidity >> 8;
	_uplink_payload.bytes[3] = _humidity & 0xFF;
	_uplink_payload.bytes[4] = co2ppm >> 8;
	_uplink_payload.bytes[5] = co2ppm & 0xFF;
	_uplink_payload.bytes[6] = _shaftStatus >> 8;
	_uplink_payload.bytes[7] = _shaftStatus & 0xFF;
	
	return _uplink_payload;
}