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
const uint8_t PWM_SPEED_SLOW[3] = {150, 255, 200};
const uint8_t PWM_SPEED_FAST[3] = {255, 255, 255};
const uint8_t PWM_MAGNET = 255;


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

const byte POWER_PWM_PIN[4] = {9, 10, 11, 3};
const byte MOTOR_DIR_PIN[3] = {6, 5, 4};


/////////////////////////
// ENUMERATIONS
/////////////////////////

typedef enum {
	ELEVATOR_MOTOR = 0,
	CART_MOTOR = 1,
	LOADER_MOTOR = 2,
	LOADER_MAGNET = 3
} output_group;

typedef enum {
	SLOW,
	FAST
} motor_speed;

typedef enum {
	FORWARD,
	BACKWARD
} motor_dir;


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
 * Affects Power_Output_PWM[], Power_Output_Enabled[], and Motor_Dir[]
 */

void setPowerOutput(output_group output, bool enable);
/*
 * Enables or disables a power output
 *
 * Affects Power_Output_Enabled[]
 * INPUT:  Output in question (0-indexed)
 *         State of being enabled
 */

void setMotorSpeed(output_group motor, motor_speed speed);
/*
 * Sets the speed of a given motor
 *
 * Affects Power_Output_PWM[]
 * INPUT:  Motor (0-indexed)
 *         Motor speed
 */

void setMotorDir(output_group motor, motor_dir dir);
/*
 * Sets the direction of a given motor
 *
 * Affects Motor_Dir[]
 * INPUT:  Motor (0-indexed)
 *         Motor direction
 */

motor_dir getMotorDir(output_group motor);
/*
 * Gets the current direction of a motor
 *
 * INPUT:  Motor (0-indexed)
 * OUTPUT: Direction of motor
 */

bool powerOutputEnabled(output_group output);
/*
 * Gets the state of a power output
 *
 * INPUT:  Output in question (0-indexed)
 * OUTPUT: State of being enabled
 */

bool anyMotorEnabled();
/*
 * Determines if any motor is currently moving
 * Used to determine critical errors
 *
 * OUTPUT: Is any other motor enabled?
 */


/////////////////////////
// INTERNAL FUNCTIONS
/////////////////////////

void setPowerOutputPWM(output_group power_output, uint8_t pwm);
/*
 * Sets the output PWM of a given power output
 *
 * Affects timer registers ORCnx
 * INPUT:  Output in question (0-indexed)
 *         New output PWM value
 */


#endif
