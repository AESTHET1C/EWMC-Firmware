#include "error.h"

bool Error_Status[ERROR_CODES];
long Error_Tick_Start;
byte Error_Tick_Curr = 0;        // Current tick within the cycle (0-indexed)
byte Error_Cycle_Blinks = 0;     // Number of blinks in the current cycle (1-indexed)

void initErrors() {
	// TODO
	clearErrorCodes();
	return;
}

void handleErrors() {
	int Tick_Elapsed_Time = millis() - Error_Tick_Start;

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

void setErrorCode(error) {
	if(error == ERROR_CODES) {  // Critical error
		clearErrorCodes();
	}
	Error_Status[error] = true;
	return;
}

void clearErrorCode(error) {
	Error_Status[error] = false;
	return;
}

void clearErrorCodes() {
	for(byte i = 0; i < ERROR_CODES; i++) {
		Error_Status[i] = false;
	}
	return;
}

byte getBlinksNext(blinks_prev) {
	// TODO
	// Test this
	for(byte i = blinks_prev++; i != blinks_prev; i++) {
		if(i > ERROR_CODES) {
			i = 1;
		}
		if(Error_Status[i - 1]) {
			return i;
		}
	}
	if((blinks_prev != 0) && Error_Status[blinks_prev - 1]) {
		return blinks_prev
	}
	return 0;
}