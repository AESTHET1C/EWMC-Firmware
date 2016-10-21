/* EWMC Firmware
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

const unsigned int DEBOUNCE_DELAY = 100;

// Motor stage delays
const unsigned int MOTOR_DIR_DELAY_PRE = 100;
const unsigned int MOTOR_DIR_DELAY_POST = 150;
const unsigned int MOTOR_SAFETY_REVERSE = 2000;
const unsigned int MOTOR_SAFETY_REVERSE_ERR_10_DELAY = 1000;

// Calibration delays
const unsigned int CAL_STAGE_DELAY = 3000;
const unsigned int CAL_TIMEOUT = 10000;
const unsigned int CAL_ERR_10_DELAY = 1000;

const byte TIMEOUT_FACTOR = 150;  // Percentage of expected travel time before timeout
const byte SLOWDOWN_FACTOR = 95;  // Percentage of expected travel time before slowing
const byte ERR_10_FACTOR = 10;


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
// ENUMERATIONS
/////////////////////////

// Available sensors
typedef enum sensor_group {
	BUTTON = 0,
	ENDSTOP_1 = 1,
	ENDSTOP_2 = 2,
	ENDSTOP_3 = 3,
	ENDSTOP_4 = 4,
	ENDSTOP_5 = 5,
	ENDSTOP_6 = 6,
	ENDSTOP_NONE = 7,
	ENDSTOP_MOTOR_1 = 8,
	ENDSTOP_MOTOR_2 = 9,
	ENDSTOP_MOTOR_3 = 10,
	ENDSTOP_ANY = 11
};

// Motor general operation states
typedef enum motor_state {
	IDLE,
	FORWARD_START,
	FORWARD,
	FORWARD_NEAR,
	DELAY_PRE_F_B,
	DELAY_POST_F_B,
	BACKWARD_START,
	BACKWARD,
	BACKWARD_NEAR,
	DELAY_PRE_B_F,
	DELAY_POST_B_F,
	SAFETY_REVERSE_FAILURE,
	SAFETY_REVERSE_EARLY,
	FAULTED
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