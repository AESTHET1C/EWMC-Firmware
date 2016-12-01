#Overview of Error Codes

During normal operation, the status (red) LED on the EWMC board remains off. However, if the Firmware detects an error, this LED is used to show the detected error.

Error codes are implemented by blinking the LED, where the number of consecutive blinks determines the error code. Each blink takes one quarter second, and an error code is displayed every two-and-a-half seconds. Therefore, an error code of 10 will result in a constantly blinking LED.

If multiple errors are detected, they will be shown in ascending order.

#Errors 1-6
###Overview
+ An endstop failed to engage
	+ Error 1 corresponds to endstop #1
	+ Error 2 corresponds to endstop #2
	+ etc...

###Trigger Conditions
+ The corresponding motor takes more time than expected to engage the endstop in question
+ The endstop in question was disengaged while the corresponding motor was stationary

###Potential Causes
+ The endstop in question has failed
+ The motor in question needs to be re-calibrated
+ The endstop was disengaged manually

###Action Taken by Firmware
+ The corresponding motor is reversed briefly if it was in motion
+ The corresponding motor is disabled until reset

###What To Do
+ Verify the endstop in question is fully plugged in
+ Verify the endstop in question is positioned correctly
+ Replace or readjust the endstop in question if needed
+ Reset the EWMC board
+ Recalibrate if needed

#Errors 7-9
###Overview
+ An endstop engaged early
	+ Error 7 corresponds to the elevator
	+ Error 8 corresponds to the mine cart
	+ Error 9 corresponds to the loader

###Trigger Conditions
+ The corresponding motor takes less time than expected to engage the endstop it was traveling toward
+ An endstop is engaged during calibration stage 1, step 1 or 5.

###Potential Causes
+ The motor in question needs to be re-calibrated
+ The endstop was engaged manually
+ The endstop has not yet been manually disengaged

###Action Taken by Firmware
+ The corresponding motor is reversed briefly if it was in motion
+ The corresponding motor is disabled until reset if it was in motion

###What To Do
+ Reset the EWMC board
+ Recalibrate if needed

#Error 10 (Critical error)
###Overview
+ At least one endstop was engaged erroneously
+ An unrecoverable error was triggered

###Trigger Conditions
+ Any pair of endstops engages simultaneously while any motor is active
+ Any endstop engages while the corresponding motor is traveling in the opposite direction
	+ Any endstop fails to disengage in a reasonable time while the corresponding motor is traveling in the opposite direction
+ Either endstop engages while the corresponding motor is reversing during errors 1-6
+ The arcade button was pressed during stage 2 of calibration

###Potential Causes
+ The endstops are plugged in incorrectly
+ The endstop was engaged manually
+ The arcade button was pressed during stage 2 of calibration

###Action Taken by Firmware
+ All motors are disabled until reset

###What To Do
+ Verify the endstops are plugged into the correct places
+ Reset the EWMC board
+ Recalibrate if needed