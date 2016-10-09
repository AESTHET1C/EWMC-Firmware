#include "power.h"

bool Power_Output_Enabled[4];
motor_dir Motor_Dir[3];

void initPowerOutputs() {
	// TODO
	// configure motor PWM channels, set to slow
	// configure motor pins
	for(byte i = 0; i < 4; i++) {
		Power_Output_Enabled[i] = false;
	}
	for(byte i = 0; i < 3; i++) {
		Motor_Dir[i] = FORWARD;
	}
	return;
}

void enablePowerOutput(output) {
	// TODO
	// Enable motor pin
	Power_Output_Enabled[output] = true;
	return;
}

void disablePowerOutput(output) {
	// TODO
	// Disable motor pin
	Power_Output_Enabled[output] = false;
	return;
}

void setPowerOutputPWM(byte motor, byte power) {
	// TODO
	// Calculate affected timer configuration register
	// Set timer register
	return;
}

void setMotorDir(byte motor, motor_dir dir){
	// TODO
	// Calculate affected direction register
	// Change affected register
	Motor_Dir[motor] = dir;
	return;
}

bool powerOutputEnabled(output) {
	return(Power_Output_Enabled[output]);
}

motor_dir getMotorDir(motor) {
	return(Motor_Dir[motor]);
}