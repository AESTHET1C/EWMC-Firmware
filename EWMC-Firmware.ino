#include "EWMC-Firmware.h"

// Motor + endstop calibration variables
unsigned int Near_Forward[3];       // Time constant for each motor by which endstop should disengage
unsigned int Near_Backward[3];
unsigned int Slowdown_Forward[3];   // Time delay for each motor before slowing
unsigned int Slowdown_Backward[3];
unsigned int Timeout_Forward[3];    // Emergency timeout for each motor
unsigned int Timeout_Backward[3];
sensor_group Endstop_Forward[3];    // The expected endstop for each motor while traveling forward

// Button + endstop engagement cycle counts
unsigned int Sensor_Count[7] = {0, 0, 0, 0, 0, 0, 0};

// Motor state variables
motor_state Motor_State[3] = {INIT, INIT, INIT};
unsigned long Motor_State_Start[3] = {0, 0, 0};
sensor_group Endstop_Front[3];                    // Relative to current motor direction
sensor_group Endstop_Back[3];                     // Relative to current motor direction

// Audio state variables
audio_state Audio_State = WAIT;
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
	delay(BUTTON_DEBOUNCE_DELAY);

	// Try to get most up-to-date calibration data
	runCalibration();

	// Make sure all motors are in correct initial states
	for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {

		// Assign Endstop_Front[] and Endstop_Back[]
		sensor_group Endstop_X = (sensor_group)((Motor * 2) + ENDSTOP_1);
		sensor_group Endstop_Y = (sensor_group)(Endstop_X + 1);

		if((Endstop_X == Endstop_Forward[Motor]) != (getMotorDir((output_group)Motor) == BACKWARD)) {
			Endstop_Front[Motor] = Endstop_X;
			Endstop_Back[Motor] = Endstop_Y;
		}
		else {
			Endstop_Front[Motor] = Endstop_Y;
			Endstop_Back[Motor] = Endstop_X;
		}

		// Set motor states
		if(sensorEngaged(Endstop_Front[Motor])) {
			changeMotorState((output_group)Motor, DELAY_POST_CHANGE);
		}
		else if(sensorEngaged(Endstop_Back[Motor])) {
			changeMotorState((output_group)Motor, IDLE);
		}
		else {
			changeMotorState((output_group)Motor, MOVE_END);
			setMotorSpeed((output_group) Motor, FAST);
			setPowerOutput((output_group)Motor, true);
			Motor_State_Start[Motor] = millis();
		}
	}
}

void loop() {
	for(byte Sensor = 0; Sensor <= ENDSTOP_6; Sensor++) {
		if(sensorEngaged(Sensor)) {
			if(Sensor_Count[Sensor] < SENSOR_REQUIRED_COUNT) {
				Sensor_Count[Sensor] += 1;
			}
		}
		else {
			Sensor_Count[Sensor] = 0;
		}
	}

	for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
		switch(Motor_State[Motor]) {
			case IDLE: {
				if(Sensor_Count[BUTTON] == SENSOR_REQUIRED_COUNT) {
					changeMotorState((output_group)Motor, MOVE_START);
					if(Motor == LOADER_MOTOR) {
						setPowerOutput(LOADER_MAGNET, true);
					}
				}
				break;
			}
			case MOVE_START: {
				if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir((output_group)Motor) == FORWARD) ? Near_Forward[Motor] : Near_Backward[Motor])) {
					changeMotorState((output_group)Motor, MOVE);
				}
				break;
			}
			case MOVE: {
				unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

				if(Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) {
					assertCriticalError();
				}
				else if(Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) {
					changeMotorState((output_group)Motor, SAFETY_REVERSE_ENDSTOP_EARLY);
					flagError(Motor + 7);
				}
				else if(Elapsed_Time >= ((getMotorDir((output_group)Motor) == FORWARD) ? Slowdown_Forward[Motor] : Slowdown_Backward[Motor])) {
					changeMotorState((output_group)Motor, MOVE_END);
				}
				break;
			}
			case MOVE_END: {
				if(Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) {
					assertCriticalError();
				}
				else if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir((output_group)Motor) == FORWARD) ? Timeout_Forward[Motor] : Timeout_Backward[Motor])) {
					changeMotorState((output_group)Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
					flagError(Endstop_Front[Motor]);
				}
				else if(Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) {
					changeMotorState((output_group)Motor, DELAY_PRE_CHANGE);
					if(Motor == LOADER_MOTOR) {
						setPowerOutput(LOADER_MAGNET, false);
					}
				}
				break;
			}
			case DELAY_PRE_CHANGE: {
				if((millis() - Motor_State_Start[Motor]) >= RELAY_PRE_CHANGE_DELAY) {
					changeMotorState((output_group)Motor, DELAY_POST_CHANGE);
				}
				break;
			}
			case DELAY_POST_CHANGE: {
				if((millis() - Motor_State_Start[Motor]) >= (RELAY_POST_CHANGE_DELAY + MOTOR_IDLE_DELAY[Motor])) {
					changeMotorState((output_group)Motor, IDLE);
				}
				break;
			}
			case SAFETY_REVERSE_ENDSTOP_FAIL: {
				if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir((output_group)Motor) == FORWARD) ? Near_Forward[Motor] : Near_Backward[Motor])) {
					changeMotorState((output_group)Motor, FAULTED);
				}
				break;
			}
			case SAFETY_REVERSE_ENDSTOP_EARLY: {
				if((millis() - Motor_State_Start[Motor]) >= ((getMotorDir((output_group)Motor) == FORWARD) ? Near_Forward[Motor] : Near_Backward[Motor])) {
					if(Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) {
						assertCriticalError();
					}
					else {
						changeMotorState((output_group)Motor, FAULTED);
					}
				}
				break;
			}
			default:
			case FAULTED: {
				setPowerOutput((output_group)Motor, false);
				setMotorDir((output_group)Motor, FORWARD);
				if(Motor == LOADER_MOTOR) {
					setPowerOutput(LOADER_MAGNET, false);
				}
				break;
			}
		}
		if((Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) && (Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) && anyMotorEnabled()) {
			assertCriticalError();
		}
	}

	switch(Audio_State) {
		case WAIT: {
			if(Sensor_Count[BUTTON] == SENSOR_REQUIRED_COUNT) {
				Audio_Delay_Length = random(AUDIO_MIN_BUTTON_DELAY, AUDIO_MAX_DELAY);
				Audio_Delay_Start = millis();
				Audio_State = DELAY;
			}
			break;
		}
		case DELAY: {
			if((millis() - Audio_Delay_Start) > Audio_Delay_Length) {
				if(Sensor_Count[BUTTON] == SENSOR_REQUIRED_COUNT) {
					audio_clip Next_Clip = Audio_Last_Clip;
					while(Next_Clip == Audio_Last_Clip) {
						Next_Clip = (audio_clip)random(AUDIO_EXPLOSION, AUDIO_COUGH_2);
					}
					playAudio(Next_Clip);
					Audio_Last_Clip = Next_Clip;

					Audio_Delay_Length = random(AUDIO_MIN_DELAY, AUDIO_MAX_DELAY);
					Audio_State = PLAY;
				}
				else {
					Audio_State = WAIT;
				}
			}
			break;
		}
		case PLAY: {
			if(!audioPlaying()) {
				Audio_Delay_Start = millis();
				Audio_State = DELAY;
			}
			break;
		}
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

	{  // Stage 1, Step 1: Disengage all endstops
		bool Wait = true;
		while(Wait) {
			Wait = false;
			clearErrors();

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
				clearErrors();
				return;
			}
		}
	}

	{  // Stage 1, Steps 2-4: Manually engage all endstops group by group
		for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
			sensor_group Sensor_Group_Init = (sensor_group)(Motor + ENDSTOP_MOTOR_1);
			sensor_group Sensor_Group_Next = (sensor_group)(Sensor_Group_Init + 1);
			sensor_group Sensor_A = (sensor_group)((Motor * 2) + ENDSTOP_1);
			sensor_group Sensor_B = (sensor_group)(Sensor_A + 1);

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

				handleErrorCodeDisplay();
				if(sensorEngaged(BUTTON)) {
					return;
				}
			}
		}
	}

	{  // Stage 1, Step 5: Disengage all endstops and press arcade button
		bool Wait = true;
		while(Wait) {
			Wait = false;
			clearErrors();

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

		delay(BUTTON_DEBOUNCE_DELAY);
		while(sensorEngaged(BUTTON)){
			handleErrorCodeDisplay();
		}

		beep();
	}

	{  // Stage 1, Step 6: Delay to avoid hand crushage
		unsigned long Cal_Delay_Start = millis();
		while((millis() - Cal_Delay_Start) < CAL_STAGE_DELAY) {
			// Do nothing
		}
	}

	unsigned int Timeout_Forward_Buffer[3];
	unsigned int Timeout_Backward_Buffer[3];
	sensor_group Endstop_Forward_Buffer[3] = {ENDSTOP_1, ENDSTOP_3, ENDSTOP_5};

	{  // Stage 2, Step 1: Slowly cycle each motor to endstops
		Motor_State[ELEVATOR_MOTOR] = INIT;
		setPowerOutput(ELEVATOR_MOTOR, true);
		Motor_State_Start[ELEVATOR_MOTOR] = millis();
		Motor_State[CART_MOTOR] = INIT;
		setPowerOutput(CART_MOTOR, true);
		Motor_State_Start[CART_MOTOR] = millis();
		Motor_State[LOADER_MOTOR] = INIT;
		setPowerOutput(LOADER_MOTOR, true);
		Motor_State_Start[LOADER_MOTOR] = millis();
		while((Motor_State[ELEVATOR_MOTOR] != IDLE) || (Motor_State[CART_MOTOR] != IDLE) || (Motor_State[LOADER_MOTOR] != IDLE)) {
			for(byte Sensor = 0; Sensor <= ENDSTOP_6; Sensor++) {
				if(sensorEngaged(Sensor)) {
					if(Sensor_Count[Sensor] < SENSOR_REQUIRED_COUNT) {
						Sensor_Count[Sensor] += 1;
					}
				}
				else {
					Sensor_Count[Sensor] = 0;
				}
			}

			for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
				switch(Motor_State[Motor]) {
					default:
					case FAULTED: {
						setPowerOutput((output_group)Motor, false);
						setMotorDir((output_group)Motor, FORWARD);
						break;
					}
					case INIT: {
						sensor_group Endstop_X = (sensor_group)((Motor * 2) + ENDSTOP_1);
						sensor_group Endstop_Y = (sensor_group)(Endstop_X + 1);

						if((millis() - Motor_State_Start[Motor]) >= CAL_TIMEOUT[Motor]) {
							changeMotorState((output_group)Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
							flagError(Endstop_X);
							flagError(Endstop_Y);
						}
						else if(Sensor_Count[Endstop_X] == SENSOR_REQUIRED_COUNT) {
							changeMotorState((output_group)Motor, DELAY_PRE_CHANGE);
							Endstop_Forward_Buffer[Motor] = Endstop_X;
							Endstop_Front[Motor] = Endstop_X;
							Endstop_Back[Motor] = Endstop_Y;
						}
						else if(Sensor_Count[Endstop_Y] == SENSOR_REQUIRED_COUNT) {
							changeMotorState((output_group)Motor, DELAY_PRE_CHANGE);
							Endstop_Forward_Buffer[Motor] = Endstop_Y;
							Endstop_Front[Motor] = Endstop_Y;
							Endstop_Back[Motor] = Endstop_X;
						}
						break;
					}
					case DELAY_PRE_CHANGE: {
						if((millis() - Motor_State_Start[Motor]) >= RELAY_PRE_CHANGE_DELAY) {
							changeMotorState((output_group)Motor, DELAY_POST_CHANGE);
						}
						break;
					}
					case DELAY_POST_CHANGE: {
						if((millis() - Motor_State_Start[Motor]) >= RELAY_POST_CHANGE_DELAY) {
							changeMotorState((output_group)Motor, ((getMotorDir((output_group)Motor) == BACKWARD) ? MOVE_START : IDLE));
						}
						break;
					}
					case MOVE_START: {
						if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
							changeMotorState((output_group)Motor, MOVE);
						}
						break;
					}
					case MOVE: {
						unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

						if(Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) {
							assertCriticalError();
						}
						else if(Elapsed_Time >= CAL_TIMEOUT[Motor]) {
							changeMotorState((output_group)Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
							flagError(Endstop_Front[Motor]);
						}
						else if(Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) {
							changeMotorState((output_group)Motor, DELAY_PRE_CHANGE);
							Timeout_Forward_Buffer[Motor] = Elapsed_Time;
							Timeout_Backward_Buffer[Motor] = Elapsed_Time;
						}
						break;
					}
					case IDLE: {
						break;
					}
					case SAFETY_REVERSE_ENDSTOP_FAIL: {
						if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
							changeMotorState((output_group)Motor, FAULTED);
						}
						break;
					}
				}
				if((Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) && (Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) && anyMotorEnabled()) {
					assertCriticalError();
				}
			}

			handleErrorCodeDisplay();
			if(sensorEngaged(BUTTON)) {
				assertCriticalError();
			}
		}
	}

	unsigned int Reference_Time_Forward[3];
	unsigned int Reference_Time_Backward[3];

	{  // Stage 2, Step 2: Quickly cycle each motor to endstops
		Motor_State[ELEVATOR_MOTOR] = MOVE_START;
		setPowerOutput(ELEVATOR_MOTOR, true);
		Motor_State_Start[ELEVATOR_MOTOR] = millis();
		Motor_State[CART_MOTOR] = MOVE_START;
		setPowerOutput(CART_MOTOR, true);
		Motor_State_Start[CART_MOTOR] = millis();
		Motor_State[LOADER_MOTOR] = MOVE_START;
		setPowerOutput(LOADER_MOTOR, true);
		Motor_State_Start[LOADER_MOTOR] = millis();
		while((Motor_State[ELEVATOR_MOTOR] != IDLE) || (Motor_State[CART_MOTOR] != IDLE) || (Motor_State[LOADER_MOTOR] != IDLE)) {
			for(byte Sensor = 0; Sensor <= ENDSTOP_6; Sensor++) {
				if(sensorEngaged(Sensor)) {
					if(Sensor_Count[Sensor] < SENSOR_REQUIRED_COUNT) {
						Sensor_Count[Sensor] += 1;
					}
				}
				else {
					Sensor_Count[Sensor] = 0;
				}
			}

			for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
				switch(Motor_State[Motor]) {
					default:
					case FAULTED: {
						setPowerOutput((output_group)Motor, false);
						setMotorDir((output_group)Motor, FORWARD);
						break;
					}
					case MOVE_START: {
						if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
							changeMotorState((output_group)Motor, MOVE);
						}
						break;
					}
					case MOVE: {
						unsigned int Elapsed_Time = (millis() - Motor_State_Start[Motor]);

						if(Elapsed_Time >= ((getMotorDir((output_group)Motor) == FORWARD) ? Timeout_Forward_Buffer[Motor] : Timeout_Backward_Buffer[Motor])) {
							changeMotorState((output_group)Motor, SAFETY_REVERSE_ENDSTOP_FAIL);
							flagError(Endstop_Front[Motor]);
						}
						else if(Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) {
							changeMotorState((output_group)Motor, DELAY_PRE_CHANGE);
							if(getMotorDir((output_group)Motor) == FORWARD) {
								Reference_Time_Forward[Motor] = Elapsed_Time;
							}
							else {
								Reference_Time_Backward[Motor] = Elapsed_Time;
							}
						}
						break;
					}
					case DELAY_PRE_CHANGE: {
						if((millis() - Motor_State_Start[Motor]) >= RELAY_PRE_CHANGE_DELAY) {
							changeMotorState((output_group)Motor, DELAY_POST_CHANGE);
						}
						break;
					}
					case DELAY_POST_CHANGE: {
						if((millis() - Motor_State_Start[Motor]) >= RELAY_POST_CHANGE_DELAY) {
							changeMotorState((output_group)Motor, ((getMotorDir((output_group)Motor) == BACKWARD) ? MOVE_START : IDLE));
						}
						break;
					}
					case IDLE: {
						break;
					}
					case SAFETY_REVERSE_ENDSTOP_FAIL: {
						if((millis() - Motor_State_Start[Motor]) >= CAL_NEAR[Motor]) {
							changeMotorState((output_group)Motor, FAULTED);
						}
						break;
					}
				}
				if((Sensor_Count[Endstop_Front[Motor]] == SENSOR_REQUIRED_COUNT) && (Sensor_Count[Endstop_Back[Motor]] == SENSOR_REQUIRED_COUNT) && anyMotorEnabled()) {
					assertCriticalError();
				}
			}

			handleErrorCodeDisplay();
		}
	}

	{  // Calibration complete, update calibration variables
		for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
			Near_Forward[Motor] = ((((unsigned long) Reference_Time_Forward[Motor]) * NEAR_FACTOR) / 100);
			Near_Backward[Motor] = ((((unsigned long) Reference_Time_Backward[Motor]) * NEAR_FACTOR) / 100);
			Slowdown_Forward[Motor] = ((((unsigned long) Reference_Time_Forward[Motor]) * SLOWDOWN_FACTOR) / 100);
			Slowdown_Backward[Motor] = ((((unsigned long) Reference_Time_Backward[Motor]) * SLOWDOWN_FACTOR) / 100);
			Timeout_Forward[Motor] = (((((unsigned long) Reference_Time_Forward[Motor]) * TIMEOUT_FACTOR) / 100) + TIMEOUT_BUFFER);
			Timeout_Backward[Motor] = (((((unsigned long) Reference_Time_Backward[Motor]) * TIMEOUT_FACTOR) / 100) + TIMEOUT_BUFFER);
			Endstop_Forward[Motor] = Endstop_Forward_Buffer[Motor];
		}
		saveCalibrationData(Reference_Time_Forward, Reference_Time_Backward);
		beep();
		beep();
		return;
	}
}

void readSavedCalibrationData() {
	unsigned int Ref_Time_Forward[3];
	unsigned int Ref_Time_Backward[3];

	// Read data from EEPROM
	byte Near_Factor = EEPROM.read(EEPROM_NEAR_FACTOR_PTR);
	byte Slowdown_Factor = EEPROM.read(EEPROM_SLOWDOWN_FACTOR_PTR);
	byte Timeout_Factor = EEPROM.read(EEPROM_TIMEOUT_FACTOR_PTR);
	unsigned int Timeout_Buffer = EEPROM.read(EEPROM_TIMEOUT_BUFFER_PTR);
	Timeout_Buffer += (((unsigned int) EEPROM.read(EEPROM_TIMEOUT_BUFFER_PTR + 1)) << 8);
	for(byte Offset = 0; Offset <= LOADER_MOTOR; Offset++) {
		Ref_Time_Forward[Offset] = EEPROM.read(EEPROM_REF_FORWARD_PTR + (Offset * 2));
		Ref_Time_Forward[Offset] += (((unsigned int) (EEPROM.read(EEPROM_REF_FORWARD_PTR + (Offset * 2) + 1))) << 8);
		Ref_Time_Backward[Offset] = EEPROM.read(EEPROM_REF_BACKWARD_PTR + (Offset * 2));
		Ref_Time_Backward[Offset] += (((unsigned int) (EEPROM.read(EEPROM_REF_BACKWARD_PTR + (Offset * 2) + 1))) << 8);
		Endstop_Forward[Offset] = (sensor_group)EEPROM.read(EEPROM_ENDSTOP_FORWARD_PTR + Offset);
	}

	// Update calibration variables
	for(byte Motor = 0; Motor <= LOADER_MOTOR; Motor++) {
		Near_Forward[Motor] = ((((unsigned long) Ref_Time_Forward[Motor]) * Near_Factor) / 100);
		Near_Backward[Motor] = ((((unsigned long) Ref_Time_Backward[Motor]) * Near_Factor) / 100);
		Slowdown_Forward[Motor] = ((((unsigned long) Ref_Time_Forward[Motor]) * Slowdown_Factor) / 100);
		Slowdown_Backward[Motor] = ((((unsigned long) Ref_Time_Backward[Motor]) * Slowdown_Factor) / 100);
		Timeout_Forward[Motor] = (((((unsigned long) Ref_Time_Forward[Motor]) * Timeout_Factor) / 100) + Timeout_Buffer);
		Timeout_Backward[Motor] = (((((unsigned long) Ref_Time_Backward[Motor]) * Timeout_Factor) / 100) + Timeout_Buffer);
	}

	return;
}

void saveCalibrationData(unsigned int ref_time_forward[3], unsigned int ref_time_backward[3]) {
	for(byte Offset = 0; Offset <= LOADER_MOTOR; Offset++) {
		EEPROM.write((EEPROM_REF_FORWARD_PTR + (Offset * 2)), (ref_time_forward[Offset] & 0xFF));
		EEPROM.update((EEPROM_REF_FORWARD_PTR + (Offset * 2) + 1), ((ref_time_forward[Offset] >> 8) & 0xFF));
		EEPROM.write((EEPROM_REF_BACKWARD_PTR + (Offset * 2)), (ref_time_backward[Offset] & 0xFF));
		EEPROM.update((EEPROM_REF_BACKWARD_PTR + (Offset * 2) + 1), ((ref_time_backward[Offset] >> 8) & 0xFF));
		EEPROM.update((EEPROM_ENDSTOP_FORWARD_PTR + Offset), Endstop_Forward[Offset]);
	}
	EEPROM.update(EEPROM_NEAR_FACTOR_PTR, NEAR_FACTOR);
	EEPROM.update(EEPROM_SLOWDOWN_FACTOR_PTR, SLOWDOWN_FACTOR);
	EEPROM.update(EEPROM_TIMEOUT_FACTOR_PTR, TIMEOUT_FACTOR);
	EEPROM.update(EEPROM_TIMEOUT_BUFFER_PTR, (TIMEOUT_BUFFER & 0xFF));
	EEPROM.update((EEPROM_TIMEOUT_BUFFER_PTR + 1), ((TIMEOUT_BUFFER >> 8) & 0xFF));
	return;
}

bool sensorEngaged(sensor_group sensor) {
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
			if(motor == LOADER_MOTOR) {
				setPowerOutput(LOADER_MAGNET, false);
			}
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
		case MOVE:
			break;
	}

	if((state != MOVE_END) && (state != MOVE)) {
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
	Motor_State[ELEVATOR_MOTOR] = FAULTED;
	setPowerOutput(ELEVATOR_MOTOR, false);
	Motor_State_Start[ELEVATOR_MOTOR] = millis();
	Motor_State[CART_MOTOR] = FAULTED;
	setPowerOutput(CART_MOTOR, false);
	Motor_State_Start[CART_MOTOR] = millis();
	Motor_State[LOADER_MOTOR] = FAULTED;
	setPowerOutput(LOADER_MOTOR, false);
	Motor_State_Start[LOADER_MOTOR] = millis();
	flagError(CRITICAL_ERROR);
	return;
}
