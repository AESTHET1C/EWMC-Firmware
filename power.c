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

void setPowerOutput(byte output, bool enable) {
	// TODO
	// Set motor pin
	Power_Output_Enabled[output] = enable;
	return;
}

void setPowerOutputPWM(byte motor, byte power) {
	// TODO
	// Calculate affected timer configuration register
	// Set timer register
	return;
}

void reverseMotor(byte motor) {
	if(getMotorDir(motor) == FORWARD) {
		setMotorDir(motor, BACKWARD);
	}
	else {
		setMotorDir(motor, FORWARD);
	}
	return;
}

void setMotorDir(byte motor, motor_dir dir){
	// TODO
	// Calculate affected direction register
	// Change affected register
	Motor_Dir[motor] = dir;
	return;
}

motor_dir getMotorDir(byte motor) {
	return(Motor_Dir[motor]);
}

bool powerOutputEnabled(byte output) {
	return(Power_Output_Enabled[output]);
}

bool otherMotorsEnabled(byte motor) {
	for(byte test_motor = 0; test_motor < 3; test_motor++) {
		if((motor != test_motor) && (powerOutputEnabled(test_motor))) {
			return true;
		}
	}
	return false;
}