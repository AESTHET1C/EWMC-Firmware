#include "error.h"

bool Error_Status[MACRO_ERROR_CODES];
unsigned long Error_Tick_Start = 0;
byte Error_Tick_Curr = 0;              // Current tick within the cycle (0-indexed)
byte Error_Cycle_Blinks = 0;           // Number of blinks in the current cycle (1-indexed)

void initErrors() {
	clearErrors();
	pinMode(ERROR_PIN, OUTPUT);
	return;
}

void handleErrorCodeDisplay() {
	unsigned int Tick_Elapsed_Time = millis() - Error_Tick_Start;

	if(Tick_Elapsed_Time >= ERROR_TICK_TIME) {
		Error_Tick_Curr += 1;
		if(Error_Tick_Curr == ERROR_CODES) {
			Error_Tick_Curr = 0;
			Error_Cycle_Blinks = getBlinksNext(Error_Cycle_Blinks);
		}
		if(Error_Tick_Curr < Error_Cycle_Blinks) {
			digitalWrite(ERROR_PIN, HIGH);
		}
		Error_Tick_Start = millis();
	}
	else if(Tick_Elapsed_Time >= ERROR_BLINK_TIME) {
		digitalWrite(ERROR_PIN, LOW);
	}
	return;
}

void flagError(byte error) {
	if((error == 0) || (error > CRITICAL_ERROR)) {
		return;
	}
	else if(error == CRITICAL_ERROR) {
		clearErrors();
	}
	Error_Status[error - 1] = true;
	return;
}

void clearErrors() {
	for(byte Error = 0; Error < ERROR_CODES; Error++) {
		Error_Status[Error] = false;
	}
	return;
}

byte getBlinksNext(byte blinks_prev) {
	if(blinks_prev < ERROR_CODES) {
		for(byte Blinks_Test = (blinks_prev + 1); Blinks_Test <= ERROR_CODES; Blinks_Test++) {
			if(Error_Status[Blinks_Test - 1]) {
				return Blinks_Test;
			}
		}
	}
	if(blinks_prev > 0) {
		for(byte Blinks_Test = 1; Blinks_Test <= blinks_prev; Blinks_Test++) {
			if(Error_Status[Blinks_Test - 1]) {
				return Blinks_Test;
			}
		}
	}
	return 0;
}
