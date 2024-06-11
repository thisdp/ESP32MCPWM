#include "ESP32MCPWM.h"
ESP32MCPWM mcpwm(0);	//Unit 0
constexpr int8_t pinA = 32;
constexpr int8_t pinB = 33;

void setup(){
  ESP32MCPWMTimer &timer = mcpwm.getTimer(0);	//Unit 0, Timer 0
  timer.attachPin(pinA,0);	//Phase A
  timer.attachPin(pinB,1);	//Phase B
  timer.begin(2000,true);	//2KHz, Complementary Mode
  timer.setDutyCycle(50);	//50% Duty Cycle
  timer.start();			//Start Timer
}

void loop(){
	
}