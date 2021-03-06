/* EWMC Firmware
 *
 * Allows for interactive control of the coal mine module
 *
 * See the EWMC Firmware documentation for details.
 *
 * State machines are used to keep track of all motors independently, using non-blocking code.
 * The electromagnet is grouped in operation with the clamshell motor.
 *
 * Written by Alex Tavares <tavaresa13@gmail.com>
 */

#ifndef main_h
#define main_h
#include <arduino.h>
#include <EEPROM.h>
#include "src/power.h"
#include "src/error.h"
#include "src/audio.h"

/////////////////////////
// CONFIGURATION VARIABLES
/////////////////////////

// Debounce and anti-noise configuration
// SENSOR_REQUIRED_COUNT is the number of consecutive cycles a sensor (or button) must be engaged
// for it to register
// This is the prevent electrical noise from falsely triggering endstops and the arcade button
const unsigned int BUTTON_DEBOUNCE_DELAY = 100;
const unsigned int SENSOR_REQUIRED_COUNT = 5;

// Motor state delays
const unsigned int RELAY_PRE_CHANGE_DELAY = 250;
const unsigned int RELAY_POST_CHANGE_DELAY = 250;
const unsigned int MOTOR_IDLE_DELAY[3] = {3000, 1000, 1000};
const unsigned int CAL_STAGE_DELAY = 3000;

// Audio playback delays
const unsigned int AUDIO_MIN_DELAY = 3000;
const unsigned int AUDIO_MAX_DELAY = 10000;
const unsigned int AUDIO_MIN_BUTTON_DELAY = 5000;

// Calibration default values
const unsigned int CAL_TIMEOUT[3] = {25000, 20000, 20000};
const unsigned int CAL_NEAR[3] = {2500, 2000, 2000};

const byte NEAR_FACTOR = 10;      // Percentage of expected travel time before no longer near endstop
const byte SLOWDOWN_FACTOR = 97;  // Percentage of expected travel time before slowing
const byte TIMEOUT_FACTOR = 125;  // Percentage of expected travel time before timeout
const unsigned int TIMEOUT_BUFFER = 1000;  // Number of extra milliseconds on top of TIMEOUT_FACTOR


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

// Pin list
const byte ENDSTOP_1_PIN = A5;
const byte ENDSTOP_2_PIN = A4;
const byte ENDSTOP_3_PIN = A3;
const byte ENDSTOP_4_PIN = A2;
const byte ENDSTOP_5_PIN = A1;
const byte ENDSTOP_6_PIN = A0;
const byte BUTTON_PIN = 12;


/////////////////////////
// ENUMERATIONS
/////////////////////////

// Available sensors
typedef enum {
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
} sensor_group;

// Motor operation states
typedef enum {
	INIT,
	IDLE,
	MOVE_START,
	MOVE,
	MOVE_END,
	DELAY_PRE_CHANGE,
	DELAY_POST_CHANGE,
	SAFETY_REVERSE_ENDSTOP_FAIL,
	SAFETY_REVERSE_ENDSTOP_EARLY,
	FAULTED
} motor_state;

// Audio operation states
typedef enum {
	WAIT,
	DELAY,
	PLAY
} audio_state;


/////////////////////////
// EEPROM POINTERS
/////////////////////////

uint16_t EEPROM_REF_FORWARD_PTR = 0x000;
uint16_t EEPROM_REF_BACKWARD_PTR = 0x006;
uint16_t EEPROM_ENDSTOP_FORWARD_PTR = 0x00C;
uint16_t EEPROM_NEAR_FACTOR_PTR = 0x00F;
uint16_t EEPROM_SLOWDOWN_FACTOR_PTR = 0x010;
uint16_t EEPROM_TIMEOUT_FACTOR_PTR = 0x011;
uint16_t EEPROM_TIMEOUT_BUFFER_PTR = 0x012;


/////////////////////////
// INTERNAL FUNCTIONS
/////////////////////////

void setup();
/*
 * Initializes all modules and runs calibration routine
 * Runs automatically on program startup
 *
 * Once calibration is complete, all initial motor states are set, and the main program loop begins.
 */

void loop();
/*
 * Handles the main state machines
 * Automatically loops endlessly after setup()
 *
 * Global state arrays are used to determine operation of each motor independently of the others.
 * The clamshell loader's electromagnet is controlled within this state machine, according to
 * the clamshell loader motor's state.
 *
 * Audio state is handled in a similar way, using global state variables. Its operation remains
 * entirely independent of motor states.
 */

void initInputs();
/*
 * Initializes input pins
 * Must be called once at startup
 *
 * Initialization involves endstop and button pin configuration.
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
 * Affects Near_Forward[], Near_Backward[], Slowdown_Forward[], Slowdown_Backward[],
 *         Timeout_Forward[], Timeout_Backward[], Endstop_Forward[], Motor_State[],
 *         Motor_State_Start[], Endstop_Front[], and Endstop_Back[]
 */

void readSavedCalibrationData();
/*
 * Reads calibration data from EEPROM to global variables
 *
 * Global constants are not utilized when recalling stored data. If desired near, slowdown, and
 * timeout factors were changed since last calibration, calibration must be repeated to update
 * these values.
 *
 * Affects Near_Forward[], Near_Backward[], Slowdown_Forward[], Slowdown_Backward[],
 *         Timeout_Forward[], Timeout_Backward[], and Endstop_Forward[]
 */

void saveCalibrationData(unsigned int ref_time_forward[3], unsigned int ref_time_backward[3]);
/*
 * Saves calibration data to EEPROM
 *
 * Both global variables and reference times are used to calculate stored data.
 * Reference travel times for each motor are stared, in addition to calibration constants.
 * Endstop_Forward[] is also saved.
 *
 * Affects locations 0x000 to 0x011 (inclusive) of EEPROM
 * INPUT:  Array of forward reference times
 *         Array of backward reference times
 */

bool sensorEngaged(sensor_group sensor);
/*
 * Gets the state of a given sensor
 *
 * INPUT:  Sensor group to check
 * OUTPUT: State of being engaged
 */

void changeMotorState(output_group motor, motor_state state);
/*
 * Changes the state of a motor, handling all the tricky bits
 *
 * This includes enabling/disabling outputs, updating Motor_State_Start[],
 * and changes to direction and speed.
 *
 * The loader electromagnet is disabled upon any fault conditions of the loader motor. However,
 * The loader electromagnet must be enabled outside this function.
 *
 * Error codes are not flagged by this function.
 *
 * Affects Motor_State[motor], Motor_State_Start[motor], Endstop_Front[motor], and Endstop_Back[motor]
 * INPUT:  Motor to change state (0-indexed)
 *         State to change to
 */

void reverseMotor(output_group motor);
/*
 * Reverses the direction of a given motor
 *
 * Affects Endstop_Front[motor] and Endstop_Back[motor]
 * INPUT:  Motor to reverse (0-indexed)
 */

void assertCriticalError();
/*
 * Flags a critical error and halts all motors
 *
 * Affects Motor_State[] and Motor_State_Start[]
 */


#endif
