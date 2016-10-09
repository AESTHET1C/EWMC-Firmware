#include "EWMC-Firmware.h"
#include "power.h"
#include "error.h"
#include "audio.h"

int Timeouts[6];           // Emergency timeouts for each motor + direction pair
byte Location_Endstop[6];  // The expected endstop for each motor + direction pair

void setup() {
	readCalibration();
	initInputs();
	initAudio();
	initErrors();
	initPowerOutputs();

	// Don't start if arcade button is pressed
	while(sensorEngaged(BUTTON)) {
		// Do nothing
	}

	delay(100);
	runCalibration();
	// TODO
	// Change motor direction if endstops engaged
}

void loop() {
	// TODO
}

void initInputs() {
	// TODO
	return;
}

bool sensorEngaged(sensor) {
	switch(sensor){
		case ENDSTOP_1:
			return(!digitalRead(ENDSTOP_1_PIN));
		case ENDSTOP_2:
			return(!digitalRead(ENDSTOP_2_PIN));
		case ENDSTOP_3:
			return(!digitalRead(ENDSTOP_3_PIN));
		case ENDSTOP_4:
			return(!digitalRead(ENDSTOP_4_PIN));
		case ENDSTOP_5:
			return(!digitalRead(ENDSTOP_5_PIN));
		case ENDSTOP_6:
			return(!digitalRead(ENDSTOP_6_PIN));
		case ENDSTOP_MOTOR_1:
			return(sensorEngaged(ENDSTOP_1) || sensorEngaged(ENDSTOP_2));
		case ENDSTOP_MOTOR_2:
			return(sensorEngaged(ENDSTOP_3) || sensorEngaged(ENDSTOP_4));
		case ENDSTOP_MOTOR_3:
			return(sensorEngaged(ENDSTOP_5) || sensorEngaged(ENDSTOP_6));
		case ENDSTOP_ANY:
			return(sensorEngaged(ENDSTOP_MOTOR_1) || sensorEngaged(ENDSTOP_MOTOR_2) || sensorEngaged(ENDSTOP_MOTOR_3));
		case ENDSTOP_NONE:
			return(!sensorEngaged(ENDSTOP_ANY));
		case BUTTON:
			return(!digitalRead(BUTTON_PIN));
		default:
			return false;
	}
}

void runCalibration() {
	cal_state Cal_State = WAIT;

	// Stage 1 variables
	sensor_group Cal_Manual_Endstop_State = ENDSTOP_NONE;
	long Cal_Delay_Start;

	// Stage 2 variables
	int Timeouts_Buffer[3] = {CAL_TIMEOUT, CAL_TIMEOUT, CAL_TIMEOUT};
	byte Location_Endstop_Buffer[6];
	motor_state Cal_Auto_State[3] = {FORWARD, FORWARD, FORWARD};

	while(!sensorEngaged(BUTTON)) {
		switch(Cal_State) {
			case WAIT:
				bool Wait = false;

				clearErrorCodes();
				if(sensorEngaged(ENDSTOP_MOTOR_1)) {
					setErrorCode(7);
					Wait = true;
				}
				if(sensorEngaged(ENDSTOP_MOTOR_2)) {
					setErrorCode(8);
					Wait = true;
				}
				if(sensorEngaged(ENDSTOP_MOTOR_3)) {
					setErrorCode(9);
					Wait = true;
				}
				if(!continueWaiting) {
					Cal_State = MANUAL_1;
				}
				break;
			case MANUAL_1:
				switch(Cal_Manual_Endstop_State) {
					case ENDSTOP_NONE:
						if(sensorEngaged(ENDSTOP_MOTOR_1)) {
							if(sensorEngaged(ENDSTOP_1)) {
								Cal_Manual_Endstop_State = ENDSTOP_1;
							}
							else {
								Cal_Manual_Endstop_State = ENDSTOP_2;
							}
							beep();
						}
						break;
					case ENDSTOP_1:
						if(sensorEngaged(ENDSTOP_2)) {
							Cal_Manual_Endstop_State = ENDSTOP_NONE;
							Cal_State = MANUAL_2;
							beep();
							beep();
						}
						break;
					case ENDSTOP_2:
						if(sensorEngaged(ENDSTOP_1)) {
							Cal_Manual_Endstop_State = ENDSTOP_NONE;
							Cal_State = MANUAL_2;
							beep();
							beep();
						}
						break;
				}
				break;
			case MANUAL_2:
				switch(Cal_Manual_Endstop_State) {
					case ENDSTOP_NONE:
						if(sensorEngaged(ENDSTOP_MOTOR_2)) {
							if(sensorEngaged(ENDSTOP_3)) {
								Cal_Manual_Endstop_State = ENDSTOP_3;
							}
							else {
								Cal_Manual_Endstop_State = ENDSTOP_4;
							}
							beep();
						}
						break;
					case ENDSTOP_3:
						if(sensorEngaged(ENDSTOP_4)) {
							Cal_Manual_Endstop_State = ENDSTOP_NONE;
							Cal_State = MANUAL_3;
							beep();
							beep();
						}
						break;
					case ENDSTOP_4:
						if(sensorEngaged(ENDSTOP_3)) {
							Cal_Manual_Endstop_State = ENDSTOP_NONE;
							Cal_State = MANUAL_3;
							beep();
							beep();
						}
						break;
				}
				break;
			case MANUAL_3:
				switch(Cal_Manual_Endstop_State) {
					case ENDSTOP_NONE:
						if(sensorEngaged(ENDSTOP_MOTOR_3)) {
							if(sensorEngaged(ENDSTOP_5)) {
								Cal_Manual_Endstop_State = ENDSTOP_5;
							}
							else {
								Cal_Manual_Endstop_State = ENDSTOP_6;
							}
							beep();
						}
						break;
					case ENDSTOP_5:
						if(sensorEngaged(ENDSTOP_6)) {
							Cal_State = DELAY;
							beep();
							beep();
							beep();
							Cal_Delay_Start = millis();
						}
						break;
					case ENDSTOP_6:
						if(sensorEngaged(ENDSTOP_5)) {
							Cal_State = DELAY;
							beep();
							beep();
							beep();
							Cal_Delay_Start = millis();
						}
						break;
				}
				break;
			case DELAY:
				if((millis() - Cal_Delay_Start) >= CAL_DELAY) {
					Cal_State = AUTO_1;
				}
				break;
			case AUTO_1:
				switch(Cal_Auto_State[0]) {
					// TODO
				}
				// TODO
				break;
			case AUTO_2:
				// TODO
				break;
		}
	}
	return;
}

void readCalibration() {
	// TODO
	// Read calibration variables from EEPROM
	return;
}

void saveCalibration(timeout_array[6], endstops_array[6]) {
	// TODO
	// Save calibration variables to EEPROM
	return;
}