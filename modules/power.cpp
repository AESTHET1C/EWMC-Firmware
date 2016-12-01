#include "power.h"

uint8_t Power_Output_PWM[4];
bool Power_Output_Enabled[4];
motor_dir Motor_Dir[3];

void initPowerOutputs() {

	// Prepare PWM outputs
	for(byte Output = 0; Output <= LOADER_MAGNET; Output++) {
		Power_Output_Enabled[Output] = false;
		setPowerOutputPWM((output_group)Output, 0);
	}
	for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
		setMotorSpeed((output_group)Motor, SLOW);
	}

	// Configure PWM registers
	TCCR1A = B10100001;
	TCCR1B = B00000100;
	TCCR2A = B10100001;
	TCCR2B = B00000110;

	// Prepare direction outputs
	for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
		setMotorDir((output_group)Motor, FORWARD);
	}

	// Drive output pins
	for(byte Output = 0; Output <= LOADER_MAGNET; Output++) {
		pinMode(POWER_PWM_PIN[Output], OUTPUT);
	}
	for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
		pinMode(MOTOR_DIR_PIN[Motor], OUTPUT);
	}

	return;
}

void setPowerOutput(output_group output, bool enable) {
	Power_Output_Enabled[output] = enable;
	if(enable) {
		setPowerOutputPWM(output, Power_Output_PWM[output]);
	}
	else {
		setPowerOutputPWM(output, 0);
	}
	return;
}

void setMotorSpeed(output_group motor, motor_speed speed) {
	Power_Output_PWM[motor] = ((speed == SLOW) ? PWM_SPEED_SLOW[motor] : PWM_SPEED_FAST[motor]);
	if(Power_Output_Enabled[motor]) {
		setPowerOutputPWM(motor, Power_Output_PWM[motor]);
	}
	return;
}

void setMotorDir(output_group motor, motor_dir dir){
	digitalWrite(MOTOR_DIR_PIN[motor], ((dir == BACKWARD) ? LOW : HIGH));
	Motor_Dir[motor] = dir;
	return;
}

motor_dir getMotorDir(output_group motor) {
	return(Motor_Dir[motor]);
}

bool powerOutputEnabled(output_group output) {
	return(Power_Output_Enabled[output]);
}

bool anyMotorEnabled() {
	for(byte Test_Motor = 0; Test_Motor <= LOADER_MOTOR; Test_Motor++) {
		if(powerOutputEnabled((output_group)Test_Motor)) {
			return true;
		}
	}
	return false;
}

void setPowerOutputPWM(output_group power_output, uint8_t pwm) {
	switch(power_output) {
		case ELEVATOR_MOTOR:
			OCR2A = pwm;
			break;
		case CART_MOTOR:
			OCR1BL = pwm;
			break;
		case LOADER_MOTOR:
			OCR1AL = pwm;
			break;
		case LOADER_MAGNET:
			OCR2B = pwm;
			break;
		default:
			break;
	}
	return;
}
