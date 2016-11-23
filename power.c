#include "power.h"

uint8_t Power_Output_PWM[4];
bool Power_Output_Enabled[4];
motor_dir Motor_Dir[3];

void initPowerOutputs() {

	// Prepare PWM outputs
	for(byte Output = 0; Output < 4; Output++) {
		Power_Output_Enabled[Output] = false;
		setPowerOutputPWM(Output, 0);
	}
	for(byte Motor = 3; Motor < 3; Motor++) {
		setMotorSpeed(Motor, SLOW);
	}

	// Configure PWM registers
	TCCR1A = B10100001;
	TCCR1B = B00000100;
	TCCR2A = B10100001;
	TCCR2B = B00000110;

	// Prepare direction outputs
	for(byte Motor = 0; Motor < 3; Motor++) {
		setMotorDir(Motor, FORWARD);
	}

	// Drive output pins
	for(byte Output = 0; Output < 4; Output++) {
		pinMode(POWER_PWM_PIN[Output], OUTPUT);
	}
	for(byte Motor = 0; Motor < 3; Motor++) {
		pinMode(MOTOR_DIR_PIN[Motor], OUTPUT);
	}

	return;
}

void setPowerOutput(byte output, bool enable) {
	Power_Output_Enabled[output] = enable;
	if(enable) {
		setPowerOutputPWM(output, Power_Output_PWM[output]);
	}
	else {
		setPowerOutputPWM(output, 0);
	}
	return;
}

void setMotorSpeed(byte motor, motor_speed speed) {
	Power_Output_PWM[motor] = ((speed == SLOW) ? PWM_SPEED_SLOW[motor] : PWM_SPEED_FAST[motor]);
	if(Power_Output_Enabled[motor]) {
		setPowerOutputPWM(motor, Power_Output_PWM[motor]);
	}
	return;
}

void setMotorDir(byte motor, motor_dir dir){
	digitalWrite(MOTOR_DIR_PIN[motor], ((dir == BACKWARD) ? LOW : HIGH));
	Motor_Dir[motor] = dir;
	return;
}

motor_dir getMotorDir(byte motor) {
	return(Motor_Dir[motor]);
}

bool powerOutputEnabled(byte output) {
	return(Power_Output_Enabled[output]);
}

bool anyMotorEnabled() {
	for(byte Test_Motor = 0; Test_Motor < 3; Test_Motor++) {
		if(powerOutputEnabled(Test_Motor)) {
			return true;
		}
	}
	return false;
}

void setPowerOutputPWM(byte power_output, uint8_t pwm) {
	switch(power_output) {
		case 0:
			OCR1AL = pwm;
			break;
		case 1:
			OCR1BL = pwm;
			break;
		case 2:
			OCR2A = pwm;
			break;
		case 3:
			OCR2B = pwm;
			break;
		default:
			break;
	}
	return;
}