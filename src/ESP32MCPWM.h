#pragma once
#include "Arduino.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#ifndef ESP32
	#error "this library is only for ESP32"
#endif

class ESP32MCPWM;
class ESP32MCPWMTimer : private mcpwm_config_t{
public:
	ESP32MCPWMTimer(uint8_t MCPWMTimerIndex, ESP32MCPWM* parent);
	void begin(uint32_t freq, bool isComplemented = false);
	void start();
	void stop();
	bool setFrequency(uint32_t freq);
	bool setResolution(uint32_t resolution);
	bool setCompare(uint8_t phase, float compareValue);
	bool setSignalState(uint8_t phase, bool state);
	bool setDutyCycle(uint8_t phase, float duty);
	bool setDutyCycleInUs(uint8_t phase, uint32_t dutyInUs);
	bool setDutyCycle(float duty);
	bool setDutyCycleInUs(uint32_t dutyInUs);
	bool setDutyMode(mcpwm_duty_type_t mode);
	bool setCounterMode(mcpwm_counter_type_t mode);
	bool setDeadTime(mcpwm_deadtime_type_t deadTimeMode, uint32_t rising, uint32_t falling);
	bool resetDeadTime();
	bool attachPin(int8_t pin, uint8_t phase);
	bool attachSyncPin(int8_t pin);
	bool attachFaultPin(int8_t pin);
	bool attachCapturePin(int8_t pin);
private:
	mcpwm_timer_t timerIndex;
	bool phaseComplement;
	ESP32MCPWM *parent;
};

class ESP32MCPWM {
public:
	ESP32MCPWM(uint8_t unit);
	inline mcpwm_unit_t getUnitIndex(){ return unitIndex; };
	inline ESP32MCPWMTimer& getTimer(uint8_t timerIndex) { return timer[timerIndex%3]; };
private:
	ESP32MCPWMTimer timer[3] = {ESP32MCPWMTimer(0,this),ESP32MCPWMTimer(1,this),ESP32MCPWMTimer(2,this)};
	mcpwm_unit_t unitIndex;
};
