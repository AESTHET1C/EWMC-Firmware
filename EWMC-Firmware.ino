#include "EWMC-Firmware.h"
#include "power.h"
#include "error.h"
#include "audio.h"

// Motor + Endstop calibration variables
unsigned int Near_Forward[3];       // Time constant for each motor by which endstop should disengage
unsigned int Near_Backward[3];
unsigned int Slowdown_Forward[3];   // Time delay for each motor before slowing
unsigned int Slowdown_Backward[3];
unsigned int Timeout_Forward[3];    // Emergency timeout for each motor
unsigned int Timeout_Backward[3];
sensor_group Endstop_Forward[3];    // The expected endstop for each motor, assuming it travels forward

// Motor state variables
motor_state Motor_State[3] = {INIT, INIT, INIT};
unsigned long Motor_State_Start[3] = {0, 0, 0};
sensor_group Endstop_Front[3];
sensor_group Endstop_Back{3];

// Audio state variables
audio_state Audio_State = IDLE;
audio_clip Audio_Last_Clip = AUDIO_BEEP;
unsigned long Audio_Delay_Start = 0;
unsigned int Audio_Delay_Length = AUDIO_MIN_BUTTON_DELAY;

void setup() {

	// Do some basic MCU initialization
	readSavedCalibrationData();
	initInputs();
	initAudio();
	initErrors();
	initPowerOutputs();

	// Wait for arcade button to be released
	while(sensorEngaged(BUTTON)) {
		// Do nothing
	}
	delay(DEBOUNCE_DELAY[BUTTON]);

	// Try to get most up-to-date calibration data
	runCalibration();

	// Make sure all motors are in correct state
	for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
		if(sensorEngaged(Endstop_Front[Motor])) {
			changeMotorState(Motor, DELAY_POST_CHANGE);
		}
		else if(sensorEngaged(Endstop_Back[Motor])) {
			changeMotorState(Motor, IDLE);
		}
		else {
			changeMotorState(Motor, MOVE_END);
			setPowerOutput(Motor, true);
		}
	}
}

void loop() {
	// TODO
	// Add electromagnet functionality

	for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
		switch(Motor_State[Motor]) {
			case IDLE:
				if(!sensorEngaged(Endstop_Back[Motor])) {
					flagError(Endstop_Back[Motor]);
					Motor_State[Motor] = FAULTED;
				}
				else if(sensorEngaged(BUTTON)) {
					changeMotorState(Motor, MOVE_START);
				}
				break;
			case MOVE_START:
				if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir(Motor) == FORWARD) ? Near_Forward[Motor] : Near_Backward[Motor])) {
					assertCriticalError();
				}
				else if(!sensorEngaged(Endstop_Back[Motor])) {
					changeMotorState(Motor, MOVE);
				}
				break;
			case MOVE:
				unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

				if(sensorEngaged(Endstop_Back[Motor]) && (Elapsed_Time >= DEBOUNCE_DELAY[Endstop_Back[Motor]])) {
					assertCriticalError();
				}
				else if(sensorEngaged(Endstop_Front[Motor])) {
					assertCriticalError();
				}
				else if(Elapsed_Time >= ((getMotorDir(Motor) == FORWARD) ? Slowdown_Forward[Motor] : Slowdown_Backward[Motor])) {
					changeMotorState(Motor, MOVE_END);
				}
				break;
			case MOVE_END:
				if(sensorEngaged(Endstop_Back[Motor])) {
					assertCriticalError();
				}
				else if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir(Motor) == FORWARD) ? Timeout_Forward[Motor] : Timeout_Backward[Motor])) {
					changeMotorState(Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
					flagError(Endstop_Front[Motor]);
				}
				else if(sensorEngaged(Endstop_Front[Motor])) {
					changeMotorState(Motor, DELAY_PRE_CHANGE);
				}
				break;
			case DELAY_PRE_CHANGE:
				unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

				if((!sensorEngaged(Endstop_Front[Motor])) && (Elapsed_Time >= DEBOUNCE_DELAY[Endstop_Front[Motor]])) {
					changeMotorState(Motor, FAULTED);
					flagError(Endstop_Front[Motor]);
				}
				else if(Elapsed_Time >= RELAY_PRE_CHANGE_DELAY) {
					changeMotorState(Motor, DELAY_POST_CHANGE);
				}
				break;
			case DELAY_POST_CHANGE:
				if(!sensorEngaged(Endstop_Back[Motor])) {
					changeMotorState(Motor, FAULTED);
					flagError(Endstop_Back[Motor]);
				}
				else if((millis() - Motor_State_Start[Motor]) >= (RELAY_POST_CHANGE_DELAY + MOTOR_IDLE_DELAY[Motor])) {
					changeMotorState(Motor, IDLE);
				}
				break;
			case SAFETY_REVERSE_ENDSTOP_FAIL:
				if(sensorEngaged(Motor + ENDSTOP_MOTOR_1)) {
					assertCriticalError();
				}
				else if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir(Motor) == FORWARD) ? Near_Forward[Motor] : Near_Backward[Motor])) {
					changeMotorState(Motor, FAULTED);
				}
				break;
			case SAFETY_REVERSE_ENDSTOP_EARLY:
				if(sensorEngaged(Endstop_Front[Motor])) {
					assertCriticalError();
				}
				else if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir(Motor) == FORWARD) ? Near_Forward[Motor] : Near_Backward[Motor])) {
					if(sensorEngaged(Endstop_Back[Motor])) {
						assertCriticalError();
					}
					else {
						changeMotorState(Motor, FAULTED);
					}
				}
				break;
			default:
			case FAULTED:
				setPowerOutput(Motor, false);
				break;
		}
		if(sensorEngaged(Endstop_Forward[Motor]) && sensorEngaged(Endstop_Backward[Motor]) && anyMotorEnabled()) {
			assertCriticalError();
		}
	}

	switch(Audio_State) {
		case IDLE:
			if(sensorEngaged(BUTTON)) {
				Audio_Delay_Length = random(AUDIO_MIN_BUTTON_DELAY, AUDIO_MAX_DELAY);
				Audio_State = DELAY;
			}
			break;
		case DELAY:
			if((millis() - Audio_Delay_Start) > Audio_Delay_Length) {
				if(sensorEngaged(BUTTON)) {
					audio_clip Next_Clip = Audio_Last_Clip;
					while(Next_Clip != Audio_Last_Clip) {
						Next_Clip = random(AUDIO_EXPLOSION, AUDIO_COUGH_2);
					}
					playAudio(Next_Clip);
					Audio_Last_Clip = Next_Clip;

					Audio_Delay_Length = random(AUDIO_MIN_DELAY, AUDIO_MAX_DELAY);
					Audio_State = PLAY;
				}
				else {
					Audio_State = IDLE;
				}
			}
			break;
		case PLAY:
			if(!audioPlaying()) {
				Audio_Delay_Start = millis();
				Audio_State = DELAY;
			}
			break;
	}

	handleErrorCodeDisplay();
}

void initInputs() {
	pinMode(ENDSTOP_1_PIN, INPUT_PULLUP);
	pinMode(ENDSTOP_2_PIN, INPUT_PULLUP);
	pinMode(ENDSTOP_3_PIN, INPUT_PULLUP);
	pinMode(ENDSTOP_4_PIN, INPUT_PULLUP);
	pinMode(ENDSTOP_5_PIN, INPUT_PULLUP);
	pinMode(ENDSTOP_6_PIN, INPUT_PULLUP);
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	return;
}

void runCalibration() {

	// Stage 1, Step 1: Disengage all endstops
	bool Wait = false;
	while(Wait) {
		Wait = false;
		clearErrorCodes();

		if(sensorEngaged(ENDSTOP_MOTOR_1)) {
			flagError(7);
			Wait = true;
		}
		if(sensorEngaged(ENDSTOP_MOTOR_2)) {
			flagError(8);
			Wait = true;
		}
		if(sensorEngaged(ENDSTOP_MOTOR_3)) {
			flagError(9);
			Wait = true;
		}

		handleErrorCodeDisplay();
		if(sensorEngaged(BUTTON)) {
			return;
		}
	}

	// Stage 1, Steps 2-4: Manually engage all endstops group by group
	for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
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

	// Stage 1, Step 5: Disengage all endstops and press arcade button
	bool Wait = false;
	while(Wait) {
		Wait = false;
		clearErrorCodes();

		if(sensorEngaged(ENDSTOP_MOTOR_1)) {
			flagError(7);
			Wait = true;
		}
		if(sensorEngaged(ENDSTOP_MOTOR_2)) {
			flagError(8);
			Wait = true;
		}
		if(sensorEngaged(ENDSTOP_MOTOR_3)) {
			flagError(9);
			Wait = true;
		}

		if(!sensorEngaged(BUTTON)) {
			Wait = true;
		}

		handleErrorCodeDisplay();
	}

	delay(DEBOUNCE_DELAY[BUTTON]);
	while(sensorEngaged(BUTTON)){
		// Do nothing
	}

	beep();

	// Stage 1, Step 6: Delay to avoid hand crushage
	unsigned long Cal_Delay_Start = millis();
	while((millis() - Cal_Delay_Start) < CAL_STAGE_DELAY) {
		// Do nothing
	}

	// Stage 2, Step 1: Slowly cycle each motor to endstops
	unsigned int Timeout_Forward_Buffer[3];
	unsigned int Timeout_Backward_Buffer[3];
	sensor_group Endstop_Forward_Buffer[3] = {ENDSTOP_1, ENDSTOP_3, ENDSTOP_5};

	Motor_State = {INIT, INIT, INIT};
	setPowerOutput(0, true);
	Motor_State_Start[0] = millis();
	setPowerOutput(1, true);
	Motor_State_Start[1] = millis();
	setPowerOutput(2, true);
	Motor_State_Start[2] = millis();
	while(Motor_State != {IDLE, IDLE, IDLE}) {
		for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
			switch(Motor_State[Motor]) {
				default:
				case FAULTED:
					setPowerOutput(Motor, false);
					break;
				case INIT:
					sensor_group Endstop_X = ((Motor * 2) + ENDSTOP_1);
					sensor_group Endstop_Y = (Endstop_X + 1);

					if((millis() - Motor_State_Start[Motor]) >= CAL_TIMEOUT[Motor]) {
						changeMotorState(Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
						flagError(Endstop_X);
						flagError(Endstop_Y);
					}
					else if(sensorEngaged(Endstop_X)) {
						changeMotorState(Motor, DELAY_PRE_CHANGE);
						Endstop_Forward_Buffer[Motor] = Endstop_X;
						Endstop_Front[Motor] = Endstop_X;
						Endstop_Back[Motor] = Endstop_Y;
					}
					else if(sensorEngaged(Endstop_Y)) {
						changeMotorState(Motor, DELAY_PRE_CHANGE);
						Endstop_Forward_Buffer[Motor] = Endstop_Y;
						Endstop_Front[Motor] = Endstop_Y;
						Endstop_Back[Motor] = Endstop_X;
					}
					break;
				case DELAY_PRE_CHANGE:
					unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

					if((!sensorEngaged(Endstop_Front[Motor])) && (Elapsed_Time >= DEBOUNCE_DELAY[Endstop_Front[Motor]])) {
						changeMotorState(Motor, FAULTED);
						flagError(Endstop_Front[Motor]);
					}
					else if(Elapsed_Time >= RELAY_PRE_CHANGE_DELAY) {
						changeMotorState(Motor, DELAY_POST_CHANGE);
					}
					break;
				case DELAY_POST_CHANGE:
					if(!sensorEngaged(Endstop_Back[Motor])) {
						changeMotorState(Motor, FAULTED);
						flagError(Endstop_Back[Motor]);
					}
					else if((millis() - Motor_State_Start[Motor]) >= RELAY_POST_CHANGE_DELAY) {
						changeMotorState(Motor, ((getMotorDir(Motor) == BACKWARD) ? MOVE_START : IDLE));
					}
					break;
				case MOVE_START:
					if(!sensorEngaged(Endstop_Back[Motor])) {
						changeMotorState(Motor, MOVE);
					}
					else if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
						assertCriticalError();
					}
					break;
				case MOVE:
					unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

					if((sensorEngaged(Endstop_Back[Motor])) && (Elapsed_Time >= DEBOUNCE_DELAY[Endstop_Back[Motor]])) {
						assertCriticalError();
					}
					else if(Elapsed_Time >= CAL_TIMEOUT[Motor]) {
						changeMotorState(Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
						flagError(Endstop_Front[Motor]);
					}
					else if(sensorEngaged(Endstop_Front[Motor])) {
						changeMotorState(Motor, DELAY_PRE_CHANGE);
						Timeout_Forward_Buffer[Motor] = Elapsed_Time;
						Timeout_Backward_Buffer[Motor] = Elapsed_Time;
					}
					break;
				case IDLE:
					if(!sensorEngaged(Endstop_Back[Motor])) {
						flagError(Endstop_Back[Motor]);
						Motor_State[Motor] = FAULTED;
					}
					break;
				case SAFETY_REVERSE_ENDSTOP_FAIL:
					if(sensorEngaged(Motor + ENDSTOP_MOTOR_1)) {
						assertCriticalError();
					}
					else if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
						changeMotorState(Motor, FAULTED);
					}
					break;
			}
			if(sensorEngaged(Endstop_Forward[Motor]) && sensorEngaged(Endstop_Backward[Motor]) && anyMotorEnabled()) {
				assertCriticalError();
			}
		}

		handleErrorCodeDisplay();
		if(sensorEngaged(BUTTON)) {
			assertCriticalError();
		}
	}

	// Stage 2, Step 2: Quickly cycle each motor to endstops
	unsigned int Reference_Time_Forward[3];
	unsigned int Reference_Time_Backward[3];

	Motor_State = {MOVE_START, MOVE_START, MOVE_START};
	setPowerOutput(0, true);
	Motor_State_Start[0] = millis();
	setPowerOutput(1, true);
	Motor_State_Start[1] = millis();
	setPowerOutput(2, true);
	Motor_State_Start[2] = millis();
	while(Motor_State != {IDLE, IDLE, IDLE}) {
		for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
			switch(Motor_State[Motor]) {
				default:
				case FAULTED:
					setPowerOutput(Motor, false);
					break;
				case MOVE_START:
					if(!sensorEngaged(Endstop_Back[Motor])) {
						changeMotorState(Motor, MOVE);
					}
					else if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
						assertCriticalError();
					}
					break;
				case MOVE:
					unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

					if((sensorEngaged(Endstop_Back[Motor])) && (Elapsed_Time >= DEBOUNCE_DELAY[Endstop_Back[Motor]])) {
						assertCriticalError();
					}
					else if(Elapsed_Time >= ((getMotorDir(Motor) == FORWARD) ? Timeout_Forward_Buffer[Motor] : Timeout_Backward_Buffer[Motor])) {
						changeMotorState(Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
						flagError(Endstop_Front[Motor]);
					}
					else if(sensorEngaged(Endstop_Front[Motor])) {
						changeMotorState(Motor, DELAY_PRE_CHANGE);
						if(getMotorDir(Motor) == FORWARD) {
							Reference_Time_Forward[Motor] = Elapsed_Time;
						}
						else {
							Reference_Time_Backward[Motor] = Elapsed_Time;
						}
					}
					break;
				case DELAY_PRE_CHANGE:
					unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

					if((!sensorEngaged(Endstop_Front[Motor])) && (Elapsed_Time >= DEBOUNCE_DELAY[Endstop_Front[Motor]])) {
						changeMotorState(Motor, FAULTED);
						flagError(Endstop_Front[Motor]);
					}
					else if(Elapsed_Time >= RELAY_PRE_CHANGE_DELAY) {
						changeMotorState(Motor, DELAY_POST_CHANGE);
					}
					break;
				case DELAY_POST_CHANGE:
					if(!sensorEngaged(Endstop_Back[Motor])) {
						changeMotorState(Motor, FAULTED);
						flagError(Endstop_Back[Motor]);
					}
					else if((millis() - Motor_State_Start[Motor]) >= RELAY_POST_CHANGE_DELAY) {
						changeMotorState(Motor, ((getMotorDir(Motor) == BACKWARD) ? MOVE_START : IDLE));
					}
					break;
				case IDLE:
					if(!sensorEngaged(Endstop_Back[Motor])) {
						flagError(Endstop_Back[Motor]);
						Motor_State[Motor] = FAULTED;
					}
					break;
				case SAFETY_REVERSE_ENDSTOP_FAIL:
					if(sensorEngaged(Motor + ENDSTOP_MOTOR_1)) {
						assertCriticalError();
					}
					else if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
						changeMotorState(Motor, FAULTED);
					}
					break;
			}
			if(sensorEngaged(Endstop_Forward[Motor]) && sensorEngaged(Endstop_Backward[Motor]) && anyMotorEnabled()) {
				assertCriticalError();
			}
		}
	}

	// Calibration complete, update calibration variables
	for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
		Near_Forward[Motor] = ((((unsigned long) Reference_Time_Forward[Motor]) * NEAR_FACTOR) / 100);
		Near_Backward[Motor] = ((((unsigned long) Reference_Time_Backward[Motor]) * NEAR_FACTOR) / 100);
		Slowdown_Forward[Motor] = ((((unsigned long) Reference_Time_Forward[Motor]) * SLOWDOWN_FACTOR) / 100);
		Slowdown_Backward[Motor] = ((((unsigned long) Reference_Time_Backward[Motor]) * SLOWDOWN_FACTOR) / 100);
		Timeout_Forward[Motor] = ((((unsigned long) Reference_Time_Forward[Motor]) * TIMEOUT_FACTOR) / 100);
		Timeout_Backward[Motor] = ((((unsigned long) Reference_Time_Backward[Motor]) * TIMEOUT_FACTOR) / 100);
		Endstop_Forward[Motor] = Endstop_Forward_Buffer[Motor];
	}
	saveCalibrationData(Reference_Time_Forward, Reference_Time_Backward);
	return;
}

void readSavedCalibrationData() {
	unsigned int Ref_Time_Forward[3];
	unsigned int Ref_Time_Backward[3];

	// Read data from EEPROM
	byte Near_Factor = EEPROM.read(EEPROM_NEAR_FACTOR_PTR);
	byte Slowdown_Factor = EEPROM.read(EEPROM_SLOWDOWN_FACTOR_PRT);
	byte Timeout_Factor = EEPROM.read(EEPROM_TIMEOUT_FACTOR_PTR);
	for(byte Offset = 0; Offset < 3; Offset++) {
		Ref_Time_Forward[Offset * 2] = EEPROM.read(EEPROM_REF_FORWARD_PTR + (Offset * 2));
		Ref_Time_Forward[Offset * 2] += (((unsigned int) EEPROM.read(EEPROM_REF_FORWARD_PTR + (Offset * 2) + 1)) << 8);
		Ref_Time_Backward[Offset * 2] = EEPROM.read(EEPROM_REF_BACKWARD_PTR + (Offset * 2));
		Ref_Time_Backward[Offset * 2] += (((unsigned int) EEPROM.read(EEPROM_REF_BACKWARD_PTR + (Offset * 2) + 1)) << 8);
		Endstop_Forward[Offset] = EEPROM.read(EEPROM_ENDSTOP_FORWARD_PTR + Offset);
	}

	// Update calibration variables
	for(output_group Motor = ELEVATOR_MOTOR; Motor <= LOADER_MOTOR; Motor++) {
		Near_Forward[Motor] = ((((unsigned long) Ref_Time_Forward[Motor]) * Near_Factor) / 100);
		Near_Backward[Motor] = ((((unsigned long) Ref_Time_Backward[Motor]) * Near_Factor) / 100);
		Slowdown_Forward[Motor] = ((((unsigned long) Ref_Time_Forward[Motor]) * Slowdown_Factor) / 100);
		Slowdown_Backward[Motor] = ((((unsigned long) Ref_Time_Backward[Motor]) * Slowdown_Factor) / 100);
		Timeout_Forward[Motor] = ((((unsigned long) Ref_Time_Forward[Motor]) * Timeout_Factor) / 100);
		Timeout_Backward[Motor] = ((((unsigned long) Ref_Time_Backward[Motor]) * Timeout_Factor) / 100);
	}

	return;
}

void saveCalibrationData(unsigned int ref_time_forward[3], unsigned int ref_time_backward[3]) {
	for(byte Offset = 0; Offset < 3; Offset++) {
		EEPROM.write((EEPROM_REF_FORWARD_PTR + (Offset * 2)), (ref_time_forward[Offset] & 0xFF));
		EEPROM.update((EEPROM_REF_FORWARD_PTR + (Offset * 2) + 1), ((ref_time_forward[Offset] >> 8) & 0xFF));
		EEPROM.write((EEPROM_REF_BACKWARD_PTR + (Offset * 2)), (ref_time_backward[Offset] & 0xFF));
		EEPROM.update((EEPROM_REF_BACKWARD_PTR + (Offset * 2) + 1), ((ref_time_backward[Offset] >> 8) & 0xFF));
		EEPROM.update((EEPROM_ENDSTOP_FORWARD_PTR + Offset), Endstop_Forward[Offset]);
	}
	EEPROM.update(EEPROM_NEAR_FACTOR_PTR, NEAR_FACTOR);
	EEPROM.update(EEPROM_SLOWDOWN_FACTOR_PRT, SLOWDOWN_FACTOR);
	EEPROM.update(EEPROM_TIMEOUT_FACTOR_PTR, TIMEOUT_FACTOR);
	return;
}

bool sensorEngaged(sensor) {
	switch(sensor){
		case BUTTON:
			return(!digitalRead(BUTTON_PIN));
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
		default:
			return false;
	}
}

void changeMotorState(output_group motor, motor_state state) {
	switch(state) {
		default:
		case FAULTED:
		case DELAY_PRE_CHANGE:
			setPowerOutput(motor, false);
			break;
		case SAFETY_REVERSE_ENDSTOP_EARLY:
		case SAFETY_REVERSE_ENDSTOP_FAIL:
			setMotorSpeed(motor, SLOW);
		case DELAY_POST_CHANGE:
			reverseMotor(motor);
			break;
		case MOVE_START:
			setPowerOutput(motor, true);
			break;
		case MOVE_END:
			setMotorSpeed(motor, SLOW);
			break;
		case IDLE:
			setMotorSpeed(motor, FAST);
			break;
	}

	if(state != MOVE_END) {
		Motor_State_Start[motor] = millis();
	}
	Motor_State[motor] = state;
	return;
}

void reverseMotor(output_group motor) {
	if(getMotorDir(motor) == FORWARD) {
		setMotorDir(motor, BACKWARD);
	}
	else {
		setMotorDir(motor, FORWARD);
	}
	sensor_group Temp_Endstop = Endstop_Front[motor];
	Endstop_Front[motor] = Endstop_Back[motor];
	Endstop_Back[motor] = Temp_Endstop;
	return;
}

void assertCriticalError() {
	setPowerOutput(0, false);
	setPowerOutput(1, false);
	setPowerOutput(2, false);
	Motor_State_Start[] = {millis(), Motor_State_Start[0], Motor_State_Start[0]};
	Motor_State[] = {FAULTED, FAULTED, FAULTED};
	flagError(CRITICAL_ERROR);
	return;
}