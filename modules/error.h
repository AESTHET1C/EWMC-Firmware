/* Error Code Module
 *
 * Used to initialize and control the status LED on the EWMC hardware to show error codes
 *
 * Error codes are displayed by blinking the corresponding number of times to the error code.
 * If multiple errors are set, each error is displayed in increasing order.
 * The highest error code is a critical error. If it is set, all other errors are cleared,
 * and the status LED blinks constantly. All Firmware operation should be halted if this happens.
 *
 * Each error code is displayed within a "cycle". Each cycle consists of ERROR_CODES "ticks".
 * Each tick lasts for ERROR_TICK_TIME milliseconds. At the beginning of each tick,
 * the LED may or may not turn on to represent a "blink".
 * After ERROR_BLINK_TIME elapses within a tick, the LED will be disabled.
 * The next cycle starts immediately afterwards, with no additional gap. As a result,
 * the error ERROR_CODES will constantly blink.
 *
 * The following diagram assumes ERROR_CODES = 3 and the error code 2 is being displayed repeatedly:
 *
 * |_________           |_________           |                    |_________           |
 * |         |__________|         |_______________________________|         |__________| and so on...
 *  --blink--            --blink--
 *  --------tick-------- --------tick-------- --------tick--------
 * -----------------------------cycle-----------------------------
 *
 * Written by Alex Tavares <tavaresa13@gmail.com>
 */

#ifndef error_h
#define error_h
#include <arduino.h>

/////////////////////////
// CONFIGURATION VARIABLES
/////////////////////////

#define MACRO_ERROR_CODES 10

const byte ERROR_CODES = MACRO_ERROR_CODES;
const byte CRITICAL_ERROR = MACRO_ERROR_CODES;
const unsigned int ERROR_TICK_TIME = 250;
const unsigned int ERROR_BLINK_TIME = 100;


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

const byte ERROR_PIN = 13;


/////////////////////////
// AVAILABLE FUNCTIONS
/////////////////////////

void initErrors();
/*
 * Initializes error code handling
 * Must be called at startup
 *
 * Initialization involves setting status variables and pin configuration.
 *
 * Affects errors[], cycleStart, currentCycle, and currentCycleGoal
 */

void handleErrorCodeDisplay();
/*
 * Updates the EWMC status LED to display error codes
 * Must be placed within a loop that executes regularly
 *
 * Affects cycleStart, currentCycle, and currentCycleGoal
 */

void flagError(byte error);
/*
 * Sets a single error code to true
 *
 * Affects errors[]
 * INPUT:  Error code to set (1-indexed)
 */

void clearErrors();
/*
 * Clears all error codes
 *
 * Affects errors[]
 */


/////////////////////////
// INTERNAL FUNCTIONS
/////////////////////////

byte getBlinksNext(byte blinks_prev);
/*
 * Scans through active errors and determines the next one to display
 * Used by handleErrorCodeDisplay()
 *
 * INPUT:  Previous displayed error (1-indexed)
 * OUTPUT: Next error to display (1-indexed)
 */


#endif
