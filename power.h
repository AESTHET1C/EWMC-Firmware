/* Power Output Handling Module
 *
 * Used to initialize, control, and manage the power output capabilities of the EWMC hardware
 *
 * This includes three motors and an electromagnet. All outputs are PWM-enabled.
 * The three motors also allow for directionality control.
 *
 * It is expected that motors are disabled for a short while before and after switching directions.
 * These delays are to be handled elsewhere.
 *
 * Written by Alex Tavares <tavaresa13@gmail.com>
 */

#ifndef power_h
#define power_h
#include <arduino.h>

/////////////////////////
// CONFIGURATION VARIABLES
/////////////////////////

// PWM presets
const uint8_t PWM_SPEED_SLOW[3] = [TODO];
const uint8_t PWM_SPEED_FAST[3] = [TODO];
const uint8_t PWM_MAGNET = [TODO];


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

const byte MOTOR_DIR_PIN[3] = {4, 5, 6};
const byte POWER_PWM_PIN[4] = {9, 10, 11, 3};


/////////////////////////
// ENUMERATIONS
/////////////////////////

typedef enum motor_dir {
	FORWARD,
	BACKWARD
};


/////////////////////////
// AVAILABLE FUNCTIONS
/////////////////////////

void initPowerOutputs();
/*
 * Initializes all four power outputs
 * Must be called at startup
 *
 * Initialization involves setting status variables, pin configuration, and PWM values.
 * Initial motor directions are also set.
 *
 * Affects Power_Output_Enabled and Motor_Dir
 */

void setPowerOutput(byte output, bool enable);
/*
 * Enables or disables a power output
 *
 * Affects Power_Output_Enabled[]
 * INPUT:  Output in question (0-indexed)
 */

void setPowerOutputPWM(byte motor, byte power);
/*
 * Sets the output power of a given power output
 *
 * INPUT:  Output in question (0-indexed)
 *         New output power
 */

void reverseMotor(byte motor);
/*
 * Reverses the direction of a given motor
 *
 * Affects Motor_Dir[]
 * INPUT:  Motor (0-indexed)
 */

void setMotorDir(byte motor, motor_dir dir);
/*
 * Sets the direction of a given motor
 *
 * Affects Motor_Dir[]
 * INPUT:  Motor (0-indexed)
 *         New direction
 */

motor_dir getMotorDir(byte motor);
/*
 * Gets the current direction of a motor
 *
 * INPUT:  Motor (0-indexed)
 * OUTPUT: Direction of motor
 */

bool powerOutputEnabled(byte output);
/*
 * Gets the state of a power output
 *
 * INPUT:  Output in question (0-indexed)
 * OUTPUT: State of being enabled
 */

bool otherMotorsEnabled(byte motor);
/*
 * Determines if any other motors are currently moving
 * Used to determine critical errors
 *
 * INPUT:  Current [known stationary] motor
 * OUTPUT: Is any motor enabled?
 */


#endif