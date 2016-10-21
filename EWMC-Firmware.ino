#include "EWMC-Firmware.h"
#include "power.h"
#include "error.h"
#include "audio.h"

unsigned int Timeout_Forward[3];    // Emergency timeout for each motor
unsigned int Timeout_Backward[3];
unsigned int Slowdown_Forward[3];   // Time delay for each motor before slowing
unsigned int Slowdown_Backward[3];
unsigned int Error_10_Forward[3];   // Timeout for each motor before error 10 eligibility
unsigned int Error_10_Backward[3];
sensor_group Endstop_Forward[3];    // The expected endstop for each motor
sensor_group Endstop_Backward[3];

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

	delay(DEBOUNCE_DELAY);
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

	// Stage 1, Step 1: Disengage all endstops
	bool Wait = false;
	while(Wait) {
		Wait = false;
		clearErrorCodes();

		if(sensorEngaged(ENDSTOP_MOTOR_1)) {
			setErrorCode(7, true);
			Wait = true;
		}
		if(sensorEngaged(ENDSTOP_MOTOR_2)) {
			setErrorCode(8, true);
			Wait = true;
		}
		if(sensorEngaged(ENDSTOP_MOTOR_3)) {
			setErrorCode(9, true);
			Wait = true;
		}

		handleErrors();
		if(sensorEngaged(BUTTON)) {
			return;
		}
	}

	// Stage 1, Steps 2-4: Manually engage all endstops group by group
	for(byte Motor = 0; Motor < 3; Motor++) {
		sensor_group Sensor_Group_Init = Motor + 7;
		sensor_group Sensor_Group_Next = Sensor_Init + 1;
		sensor_group Sensor_A = ((Motor * 2) + 1);
		sensor_group Sensor_B = Sensor_A + 1;

		sensor_group Last_Activated_Sensor = Sensor_Group_Init;
		while(Last_Activated_Sensor != Sensor_Group_Next) {
			if(Last_Activated_Sensor == Sensor_Group_Init) {
				if(sensorEngaged(Sensor_A)) {
					Last_Activated_Sensor = Sensor_A;
					beep();
				}
				else if(sensorEngaged(Sensor_B)) {
					Last_Activated_Sensor = Sensor_B;
					beep();
				}
			}
			else if(Last_Activated_Sensor == Sensor_A) {
				if(sensorEngaged(Sensor_B)) {
					Last_Activated_Sensor = Sensor_Group_Next;
					beep();
					beep();
				}
			}
			else {
				if(sensorEngaged(Sensor_A)) {
					Last_Activated_Sensor = Sensor_Group_Next;
					beep();
					beep();
				}
			}

			if(sensorEngaged(BUTTON)) {
				return;
			}
		}
	}
	beep();

	// Stage 1, Step 5: Delay to avoid hand crushage
	unsigned long Cal_Delay_Start = millis();
	while((millis() - Cal_Delay_Start) < CAL_STAGE_DELAY) {
		if(sensorEngaged(BUTTON)) {
			return;
		}
	}

	// Stage 2, Step 1: Slowly cycle each motor to endstops
	unsigned int Timeout_Forward_Buffer[3] = {CAL_TIMEOUT, CAL_TIMEOUT, CAL_TIMEOUT};
	unsigned int Timeout_Backward_Buffer[3] = {CAL_TIMEOUT, CAL_TIMEOUT, CAL_TIMEOUT};
	unsigned int Error_10_Forward_Buffer[3] = {CAL_ERR_10_DELAY, CAL_ERR_10_DELAY, CAL_ERR_10_DELAY};
	unsigned int Error_10_Backward_Buffer[3] = {CAL_ERR_10_DELAY, CAL_ERR_10_DELAY, CAL_ERR_10_DELAY};
	sensor_group Endstop_Forward_Buffer[3] = {ENDSTOP_1, ENDSTOP_3, ENDSTOP_5};
	sensor_group Endstop_Backward_Buffer[3] = {ENDSTOP_2, ENDSTOP_4, ENDSTOP_6};
	motor_state Cal_Motor_State[3] = {FORWARD, FORWARD, FORWARD};
	unsigned long State_Start[3] = {millis(), State_Start[0], State_Start[0]};

	setPowerOutput(0, true);
	setPowerOutput(1, true);
	setPowerOutput(2, true);
	while(Cal_Motor_State != {IDLE, IDLE, IDLE}) {
		for(byte Motor = 0; Motor < 3; Motor++) {
			switch(Cal_Motor_State[Motor]) {
				default:
				case FAULTED:
					setPowerOutput(Motor, false);
					break;
				case FORWARD:
					sensor_group Sensor_A = ((Motor * 2) + 1);
					sensor_group Sensor_B = Sensor_A + 1;

					if((millis() - State_Start[Motor]) >= Timeout_Forward_Buffer[Motor]) {
						setErrorCode(Sensor_A, true);
						setErrorCode(Sensor_B, true);
						reverseMotor(Motor);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = SAFETY_REVERSE_FAILURE;
					}
					else if(sensorEngaged(Sensor_A)) {
						Endstop_Forward_Buffer[Motor] = Sensor_A;
						Endstop_Backward_Buffer[Motor] = Sensor_B;
						setPowerOutput(Motor, false);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = DELAY_PRE_F_B;
					}
					else if(sensorEngaged(Sensor_B)) {
						Endstop_Forward_Buffer[Motor] = Sensor_B;
						Endstop_Backward_Buffer[Motor] = Sensor_A;
						setPowerOutput(Motor, false);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = DELAY_PRE_F_B;
					}
					break;
				case DELAY_PRE_F_B:
					if((sensorEngaged(Endstop_Backward_Buffer[Motor])) && otherMotorsEnabled(Motor)) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if(!sensorEngaged(Endstop_Forward_Buffer[Motor])) {
						setErrorCode(Endstop_Forward_Buffer[Motor], true);
						Cal_Motor_State[Motor] = FAULTED;
					}
					else if((millis() - State_Start[Motor]) >= MOTOR_DIR_DELAY_PRE) {
						setMotorDir(Motor, REVERSE);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = DELAY_POST_F_B;
					}
					break;
				case DELAY_POST_F_B:
					if((sensorEngaged(Endstop_Backward_Buffer[Motor])) && otherMotorsEnabled(Motor)) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if(!sensorEngaged(Endstop_Forward_Buffer[Motor])) {
						setErrorCode(Endstop_Forward_Buffer[Motor], true);
						Cal_Motor_State[Motor] = FAULTED;
					}
					else if((millis() - State_Start[Motor]) >= MOTOR_DIR_DELAY_POST) {
						setPowerOutput(Motor, true);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = BACKWARD_START;
					}
					break;
				case BACKWARD_START:
					if(sensorEngaged(Endstop_Forward_Buffer[Motor]) && sensorEngaged(Endstop_Backward_Buffer[Motor])) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if(((millis() - State_Start[Motor]) >= Error_10_Backward_Buffer[Motor]) || (!sensorEngaged(Endstop_Forward_Buffer[Motor]))) {
						Cal_Motor_State[Motor] = BACKWARD;
					}
					break;
				case BACKWARD:
					if(sensorEngaged(Endstop_Forward_Buffer[Motor])) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if((millis() - State_Start[Motor]) >= Timeout_Backward_Buffer[Motor]) {
						setErrorCode(Endstop_Backward_Buffer[Motor], true);
						reverseMotor(Motor);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = SAFETY_REVERSE_FAILURE;
					}
					else if(sensorEngaged(Endstop_Backward_Buffer[Motor]) {
						setPowerOutput(Motor, false);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = DELAY_PRE_B_F;
					}
					break;
				case DELAY_PRE_B_F:
					if((sensorEngaged(Endstop_Forward_Buffer[Motor])) && otherMotorsEnabled(Motor)) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if(!sensorEngaged(Endstop_Backward_Buffer[Motor])) {
						setErrorCode(Endstop_Backward_Buffer[Motor], true);
						Cal_Motor_State[Motor] = FAULTED;
					}
					else if((millis() - State_Start[Motor]) >= MOTOR_DIR_DELAY_PRE) {
						setMotorDir(Motor, FORWARD);
						State_Start[Motor] = millis();
						Cal_Motor_State[Motor] = DELAY_POST_B_F;
					}
					break;
				case DELAY_POST_B_F:
					if((sensorEngaged(Endstop_Forward_Buffer[Motor])) && otherMotorsEnabled(Motor)) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if(!sensorEngaged(Endstop_Backward_Buffer[Motor])) {
						setErrorCode(Endstop_Backward_Buffer[Motor], true);
						Cal_Motor_State[Motor] = FAULTED;
					}
					else if((millis() - State_Start[Motor]) >= MOTOR_DIR_DELAY_POST) {
						Cal_Motor_State[Motor] = IDLE;
					}
					break;
				case SAFETY_REVERSE_FAILURE:
					if(sensorEngaged(Motor + 8)) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if((millis() - State_Start[Motor]) >= MOTOR_SAFETY_REVERSE) {
						Cal_Motor_State[Motor] = FAULTED;
					}
					break;
				case IDLE:
					if((sensorEngaged(Endstop_Forward_Buffer[Motor])) && otherMotorsEnabled(Motor)) {
						setErrorCode(CRITICAL_ERROR, true);
						Cal_Motor_State[] = {FAULTED, FAULTED, FAULTED};
					}
					else if(!sensorEngaged(Endstop_Backward_Buffer[Motor])) {
						setErrorCode(Endstop_Backward_Buffer[Motor], true);
						Cal_Motor_State[Motor] = FAULTED;
					}
					break;
			}
		}

		handleErrors();
		if(sensorEngaged(BUTTON)) {
			setPowerOutput(0, false);
			setPowerOutput(1, false);
			setPowerOutput(2, false);
			return;
		}
	}

	unsigned int Slowdown_Forward_Buffer[3];
	unsigned int Slowdown_Backward_Buffer[3];

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