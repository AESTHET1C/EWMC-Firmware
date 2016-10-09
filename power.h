/* Power Output Handling Module
 *
 * Used to initialize, control, and manage the power output capabilities of the EWMC hardware
 *
 * This includes three motors and an electromagnet. All outputs are PWM-enabled.
 * The three motors also allow for directionality control.
 *
 * It is expected that motors are disabled for a short while before and after switching directions.
 * These delays are to be handled elsewhere, but their time constants can be found here.
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
const byte PWM_SPEED_SLOW = [TODO];
const byte PWM_SPEED_FAST = [TODO];
const byte PWM_MAGNET = [TODO];

// Motor direction change delays
const int MOTOR_DIR_DELAY_PRE = 100;
const int MOTOR_DIR_DELAY_POST = 150;


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

const byte MOTOR_DIR_PIN[3] = {4, 5, 6};
const byte POWER_PWM_PIN[4] = {9, 10, 11, 3};


/////////////////////////
// ENUMS
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

void enablePowerOutput(byte output);
/*
 * Enables a power output
 *
 * Affects Power_Output_Enabled
 * INPUT:  Output in question (0-indexed)
 */

void disablePowerOutput(byte output);
/*
 * Disables a power output
 *
 * Affects Power_Output_Enabled
 * INPUT:  Output in question (0-indexed)
 */

void setPowerOutputPWM(byte motor, byte power);
/*
 * Sets the output power of a given power output
 *
 * INPUT:  Output in question (0-indexed)
 *         New output power
 */

void setMotorDir(byte motor, motor_dir dir);
/*
 * Sets the direction of a given motor
 *
 * Affects Motor_Dir
 * INPUT:  Motor (0-indexed)
 *         New direction
 */

bool powerOutputEnabled(byte output);
/*
 * Gets the state of a power output
 *
 * INPUT:  Output in question (0-indexed)
 * OUTPUT: State of being enabled
 */

motor_dir getMotorDir(byte motor);
/*
 * Gets the current direction of a motor
 *
 * INPUT:  Motor (0-indexed)
 * OUTPUT: Direction of motor
 */


#endif