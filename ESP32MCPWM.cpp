#include "ESP32MCPWM.h"
#include <stdio.h>
#include "esp_system.h"
#include "esp_attr.h"

#define debug(fmt, ...) Serial.printf("%s: " fmt "\r\n", __func__, ##__VA_ARGS__)

ESP32MCPWMTimer::ESP32MCPWMTimer(uint8_t MCPWMTimerIndex, ESP32MCPWM* p){
	timerIndex = (mcpwm_timer_t)MCPWMTimerIndex;
	parent = p;
}

void ESP32MCPWMTimer::begin(uint32_t freq, bool isComplemented){
	phaseComplement = isComplemented;
	frequency = freq;
	cmpr_a = 0;
	cmpr_b = 0;
	counter_mode = MCPWM_UP_COUNTER;
	duty_mode = MCPWM_DUTY_MODE_0;
	mcpwm_init(parent->getUnitIndex(), timerIndex, (mcpwm_config_t*)this);
	setDutyMode(MCPWM_DUTY_MODE_0);
}

void ESP32MCPWMTimer::start(){
	mcpwm_start(parent->getUnitIndex(), timerIndex);
}

void ESP32MCPWMTimer::stop(){
	mcpwm_stop(parent->getUnitIndex(), timerIndex);
}

bool ESP32MCPWMTimer::setFrequency(uint32_t freq){
	frequency = freq; 
	mcpwm_init(parent->getUnitIndex(), timerIndex, (mcpwm_config_t*)this);
	return true;
}

bool ESP32MCPWMTimer::setCompare(uint8_t phase, float compareValue){
	switch(phase) {
		case 0:
			cmpr_a = compareValue;
		return true;
		case 1:
			cmpr_b = compareValue;
		return true;
	}
	return false;
}

bool ESP32MCPWMTimer::setDutyCycle(uint8_t phase, float duty){
	mcpwm_set_duty(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)phase, duty);
	return true;
}

bool ESP32MCPWMTimer::setDutyCycleInUs(uint8_t phase, uint32_t dutyInUs){
	mcpwm_set_duty_in_us(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)phase, dutyInUs);
	return true;
}

bool ESP32MCPWMTimer::setDutyCycle(float duty){
	mcpwm_set_duty(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)0, duty);
	mcpwm_set_duty(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)1, duty);
	return true;
}

bool ESP32MCPWMTimer::setDutyCycleInUs(uint32_t dutyInUs){
	mcpwm_set_duty_in_us(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)0, dutyInUs);
	mcpwm_set_duty_in_us(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)1, dutyInUs);
	return true;
}


bool ESP32MCPWMTimer::setDutyMode(mcpwm_duty_type_t mode){
	duty_mode = mode;
	mcpwm_duty_type_t phaseAMode = mode;
	mcpwm_duty_type_t phaseBMode = mode;
	if(phaseComplement){
		switch(mode){
		case MCPWM_DUTY_MODE_0:
			phaseAMode = MCPWM_DUTY_MODE_0;
			phaseBMode = MCPWM_DUTY_MODE_1;
		break;
		case MCPWM_DUTY_MODE_1:
			phaseAMode = MCPWM_DUTY_MODE_1;
			phaseBMode = MCPWM_DUTY_MODE_0;
		break;
		case MCPWM_HAL_GENERATOR_MODE_FORCE_LOW:
			phaseAMode = MCPWM_HAL_GENERATOR_MODE_FORCE_LOW;
			phaseBMode = MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH;
		break;
		case MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH:
			phaseAMode = MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH;
			phaseBMode = MCPWM_HAL_GENERATOR_MODE_FORCE_LOW;
		break;
		}
	}
	mcpwm_set_duty_type(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)0,phaseAMode);
	mcpwm_set_duty_type(parent->getUnitIndex(), timerIndex, (mcpwm_generator_t)1,phaseBMode);
	return true;
}

bool ESP32MCPWMTimer::setCounterMode(mcpwm_counter_type_t mode){
	counter_mode = mode;
	return true;
}

bool ESP32MCPWMTimer::setSignalState(uint8_t phase, bool state){
	if(phase >= (uint8_t)MCPWM_GEN_MAX) return false;
	if(state){
		mcpwm_set_signal_high(parent->getUnitIndex(),timerIndex,(mcpwm_generator_t)phase);
	}else{
		mcpwm_set_signal_low(parent->getUnitIndex(),timerIndex,(mcpwm_generator_t)phase);
	}
	return true;
}

bool ESP32MCPWMTimer::setDeadTime(mcpwm_deadtime_type_t deadTimeMode, uint32_t rising, uint32_t falling){
	mcpwm_deadtime_enable(parent->getUnitIndex(), timerIndex, deadTimeMode, rising, falling);
	return true;
}

bool ESP32MCPWMTimer::resetDeadTime(){
	mcpwm_deadtime_disable(parent->getUnitIndex(), timerIndex);
	return true;
}

bool ESP32MCPWMTimer::attachPin(int8_t pin, uint8_t phase){
	if(phase >=2) return false;
	int8_t channelPhase = timerIndex*2+phase%2;
	mcpwm_unit_t nUnit = parent->getUnitIndex();
	mcpwm_gpio_init(parent->getUnitIndex(), (mcpwm_io_signals_t)channelPhase, pin);
	return true;
}

bool ESP32MCPWMTimer::attachSyncPin(int8_t pin){
	mcpwm_gpio_init(parent->getUnitIndex(), (mcpwm_io_signals_t)((uint8_t)MCPWM_SYNC_0+(uint8_t)timerIndex), pin);
	return true;
}

bool ESP32MCPWMTimer::attachFaultPin(int8_t pin){
	mcpwm_gpio_init(parent->getUnitIndex(), (mcpwm_io_signals_t)((uint8_t)MCPWM_FAULT_0+(uint8_t)timerIndex), pin);
	return true;
}

bool ESP32MCPWMTimer::attachCapturePin(int8_t pin){
	mcpwm_gpio_init(parent->getUnitIndex(), (mcpwm_io_signals_t)((uint8_t)MCPWM_CAP_0+(uint8_t)timerIndex), pin);
	return true;
}

bool ESP32MCPWMTimer::setResolution(uint32_t resolution){
	mcpwm_timer_set_resolution(parent->getUnitIndex(),timerIndex,resolution);
}

ESP32MCPWM::ESP32MCPWM(uint8_t unit){
	unitIndex = (mcpwm_unit_t)unit;
}