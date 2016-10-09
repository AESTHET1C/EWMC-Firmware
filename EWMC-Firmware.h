/* Main Module
 *
 * Allows for interactive control of the coal mine module
 *
 * See the EWMC Firmware documentation for details.
 *
 * State machines are used to keep track of all motors independently, using non-blocking code.
 * Each controlled device uses its own switch statement(s) and enum(s) for this, running in a loop.
 * Button presses are handled individually by each controlled device.
 * The electromagnet is grouped in operation with the clamshell motor.
 *
 * Written by Alex Tavares <tavaresa13@gmail.com>
 */

#ifndef main_h
#define main_h
#include <arduino.h>

/////////////////////////
// CONFIGURATION VARIABLES
/////////////////////////

// Calibration routine
const int CAL_DELAY = 3000;     // Delay between stage 1 and 2
const int CAL_TIMEOUT = 10000;  // Default motor timeout for stage 2

// Motor operation
const int ENDSTOP_DELAY = 1000;  // Delay between motor movement to error 10 eligibility


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

// Pin list
const byte ENDSTOP_1_PIN = A0;
const byte ENDSTOP_2_PIN = A1;
const byte ENDSTOP_3_PIN = A3;
const byte ENDSTOP_4_PIN = A4;
const byte ENDSTOP_5_PIN = A5;
const byte ENDSTOP_6_PIN = A6;
const byte BUTTON_PIN = 12;


/////////////////////////
// ENUMS
/////////////////////////

// Available sensors
typedef enum sensor_group {
	ENDSTOP_NONE,
	ENDSTOP_1,
	ENDSTOP_2,
	ENDSTOP_3,
	ENDSTOP_4,
	ENDSTOP_5,
	ENDSTOP_6,
	ENDSTOP_MOTOR_1,
	ENDSTOP_MOTOR_2,
	ENDSTOP_MOTOR_3,
	ENDSTOP_ANY,
	BUTTON
};

// Calibration (stage 1) states
typedef enum cal_state {
	WAIT,
	MANUAL_1,
	MANUAL_2,
	MANUAL_3,
	DELAY,
	AUTO_1,
	AUTO_2
};

// Motor general operation states
typedef enum motor_state {
	FAULTED,
	FORWARD,
	DELAY_1,
	REVERSE,
	DELAY_2,
	PASSED
};


/////////////////////////
// INTERNAL FUNCTIONS
/////////////////////////

void setup();
/*
 * Initializes all modules and runs calibration routine
 * Runs automatically on program startup
 *
 * TODO
 */

void loop();
/*
 * Handles the main state machines
 * Automatically loops endlessly after setup()
 *
 * TODO
 */

void initInputs();
/*
 * Initializes input pins
 * Must be called once at startup
 *
 * Initialization involves pin configuration.
 */

void runCalibration();
/*
 * Runs the calibration routine and saves updated calibration variables to EEPROM
 * Should be called only once at startup; it contains its own looping state machine
 *
 * Calibration takes place in two stages; stage 1 is a manual checking of endstop functionality
 * and stage 2 uses automated motor movement to determine endstop location and motor speeds.
 * The process is fully explained in the Firmware documentation.
 *
 * The calibration routine can be exited at any time by pressing the arcade button.
 * Calibration variables are not altered if the routine is aborted.
 *
 * Affects Motor_Direction[], Motor_Timeout[], and Motor_Endstops[]
 */

bool sensorEngaged(sensor_group sensor);
/*
 * Gets the state of a given sensor
 *
 * INPUT:  Sensor group to check
 * OUTPUT: State of being engaged
 */

void readCalibration();
/*
 * Reads calibration data from EEPROM to global variables
 *
 * Affects Motor_Timeout[3] and Motor_Endstops[6]
 */

void saveCalibration(int timeouts[6], byte location_endstops[6]);
/*
 * Saves calibration data to EEPROM
 * 
 * INPUT:  Array of motor timeout constants
 *         Array of expected endstop for each motor/direction pair
 */


#endif