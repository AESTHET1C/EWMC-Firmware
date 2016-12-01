#Overview of the Firmware

The Eli Whitney Mine Controller ("EWMC") is an ATmega 328P-powered board that attaches to the rear of the coal mine module within the Eli Whitney Museum's seasonal train display and controls various features of the module. The EWMC Firmware is specific to both the EWMC board and the coal mine module itself. The Firmware allows public interaction with the coal mine module in the following ways:

+ Offer a single arcade button for the public to press
+ Raise or lower the mine shaft elevator when the arcade button is pressed
+ Continue raising and lowering the mine shaft elevator while the arcade button is being pressed
+ Move the mine cart back and forth while the arcade button is being pressed
+ Raise and lower the clamshell loader's bucket when the arcade button is pressed
+ Power the electromagnet within the clamshell loader's bucket to [pretend to] pick up objects
+ Play multiple short sound effects at random intervals while the arcade button is being pressed

The Firmware also utilizes dual endstop sensors per motor along with an optional startup calibration routine to prevent potentially erroneous or destructive motor control. In the event that a malfunction does happen, the Firmware will disable the operation of the affected motor and display an error code on the EWMC board's LED.


#Setup Checklist
This checklist assumes the EWMC board is already screwed to the back of the coal mine module, and that all motors and endstops, in addition to the speaker and arcade button connector, are already attached to the board. For information on electrical connections and their locations, see "**Wiring Connections**".

---

- [ ] Move the coal mine module and clamshell loader into place on the train table
- [ ] Attach all strings and chains to their respective winches and tensioners
- [ ] Plug in the clamshell loader's electromagnet
- [ ] Verify all motors are not at the ends of their movement range (no endstops should be triggered)
- [ ] Plug in connector D15 to the train table and apply power
- [ ] Complete the calibration routine (See "**Calibration**")
- [ ] Verify proper operation


#Calibration

**Note:** Calibration is required after both initial connection of the board and replacment of components (motors, endstops, etc...) due to failure. No action by the staff is needed when the module is simply powered after having been off overnight.

**Note:** The calibration routine is skipped when the arcade button is pressed.

Calibration is used to ensure endstops are plugged in correctly and are working, and to establish sanity checks for normal operation. Upon completion of calibration, these values are stored to non-volatile memory. If calibration is skipped, the last known values are used. The calibration routine is started immediately upon the reset of the EWMC board and has two stages:

###Stage 1: Manual Endstop Engagement
1. Disengage all endstops. Elevator endstops should be a priority.
2. Manually engage an endstop (A or B) for the elevator. A beep will play upon successful engagement.
3. Manually engage the opposing endstop for the elevator. Two beeps will play upon successful engagement.
4. Repeat steps 2 and 3 for the mine cart, then the loader. Once completed, calibration cannot be skipped.
5. Disengage all endstops and press the arcade button. A final beep will play when the button is released.
6. Remove hands from all motor paths.

This stage is used to verify endstops are working and plugged into the correct locations. If an endstop does not beep upon engagement, verify the speaker and endstop are working and plugged into the correct locations.

Calibration continues to stage 2 after a 3-second delay.

###Stage 2: Automated Calibration:

**Note:** While safety timers are in place, proper motor movement should be verified during this stage. All motors are immediately halted and a critical error is asserted if the arcade button is pressed.

1. Each motor slowly cycles to each endstop.
2. Each motor cycles at full speed to each endstop.
3. Calibration data is stored in non-volatile memory.

This stage determines the locations of each endstop and the average travel time of each motor in both directions. These values are used during normal operation to determine erroneous endstop operation and motor failures.

The Firmware proceeds to normal operation after calibration.


#Wiring Connections

The four high-power connections are located on the bottom right of the EWMC board. From left to right, they are:

+ Elevator motor
+ Mine cart motor
+ Loader motor
+ Loader electromagnet

---

The eight low-power connections are located on the bottom left of the EWMC board. From left to right, they are:

+ Elevator endstop A
+ Elevator endstop B
+ Mine cart endstop A
+ Mine cart endstop B
+ Loader endstop A
+ Loader endstop B
+ Arcade button
+ Speaker

All low-power connections are in non-polarized vertical pairs. Attaching connectors horizontally may cause damage to the EWMC board or the coal mine module. The two endstops for each motor are interchangeable; they will be automatically detected during calibration.


#Error Codes

See **Error Codes Documentation** for a list of error codes and their operation.
