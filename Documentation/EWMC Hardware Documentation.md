#Overview of the Hardware

The Eli Whitney Mine Controller ("EWMC") is an ATmega 328P-powered board that attaches to the rear of the coal mine module within the Eli Whitney Museum's seasonal train display and controls various features of the module. It is intended to be run from a single 12 volt power source. The EWMC is intended to control the following elements of the coal mine module:

+ Activate features only when a button is pressed by the public
+ Raise and lower the mine shaft elevator
+ Move the mine cart back and forth
+ Raise and lower the clamshell bucket
+ Power the electromagnet within the clamshell bucket
+ Play multiple short sound effects

Additionally, the EWMC has an on-board LED to display status and error codes to the staff in the event of a malfunction.


#Capabilities

The EWMC contains a socketed Adafruit Pro Trinket 5V and Nuvoton ISD1740 for ease of programming and debugging. It uses several relays and MOSFETs to drive high-power loads. The EWMC features the following elements:

+ Three high-power, reversible outputs
+ One high-power, non-reversible output
+ One speaker output, with up to approximately 25 seconds of prerecorded sounds
+ Seven general-purpose input/outputs (GPIO)
+ One on-board LED

All four high-power outputs are PWM-enabled. They contain snubber diodes to prevent voltage spikes when powering reactive loads.


#Connectivity

**Note:** This section assumes connector contacts and housings are purchased separately.

With the exception of the power connector, the EWMC interfaces with all inputs and outputs through gold-plated 0.1"-pitch male headers. It is suggested to use only high-normal-force, gold-plated female contacts (Molex 16-02-1124 or 16-02-1125) to prevent failures or degraded performance due to vibration or oxidation. Other suitable contacts can be found in the Molex 71851 product series.

The four high-power outputs feature shrouded right-angle headers with latch holders (Molex 70553-0001). While standard non-polarized connectors will fit, it is suggested to use a latched housing (Molex 50-57-9402). 24 AWG (or thicker) wire should be used with these connectors due to current draw.

The seven GPIO connections and speaker share a standard 8x2 right-angle header, where each pin belongs to a vertical pair. The speaker uses the right-most pair. On the remaining seven pairs, the top pin is an unbuffered connection to the ATmega 328P, and the lower pin is ground. To utilize all eight pairs, standard non-polarized housings (Molex 50-57-9002) should be used.


#Power Supply and Ratings

While intended to be run from a 12 volt supply, the EWMC is capable of being run from any voltage between approximately 5.5 and 16 volts. The four high-power outputs use this voltage, while the seven GPIO connections are 5 volt logic via an internal regulator.

The four high-power outputs are rated for 3 amps of continuous current. Peak current is not rated. The seven GPIO outputs are rated for a continuous 40 milliamps per pin. Total current sink on these pins must not exceed 200 milliamps, and the internal regulator can supply up to 150 milliamps.
