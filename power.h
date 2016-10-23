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

typedef enum motor_speed {
	SLOW,
	FAST
}


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
 * Affects Power_Output_Enabled[] and Motor_Dir[]
 */

void setPowerOutput(byte output, bool enable);
/*
 * Enables or disables a power output
 *
 * Affects Power_Output_Enabled[]
 * INPUT:  Output in question (0-indexed)
 *         State of being enabled
 */

void setMotorSpeed(byte motor, motor_speed speed);
/*
 * Sets the speed of a given motor
 *
 * Affects timer registers TODO
 * INPUT:  Motor (0-indexed)
 *         Motor speed
 */

void setMotorDir(byte motor, motor_dir dir);
/*
 * Sets the direction of a given motor
 *
 * Affects Motor_Dir[]
 * INPUT:  Motor (0-indexed)
 *         Motor direction
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
 * INPUT:  Current [known stationary] motor (0-indexed)
 * OUTPUT: Is any other motor enabled?
 */


/////////////////////////
// INTERNAL FUNCTIONS
/////////////////////////

void setPowerOutputPWM(byte power_output, uint8_t pwm);
/*
 * Sets the output PWM of a given power output
 *
 * Affects timer registers TODO
 * INPUT:  Output in question (0-indexed)
 *         New output PWM value
 */


#endif