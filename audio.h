/* ISD1700 Handler Module
 *
 * Used to initialize, manage, and communicate with the ISD1700 chip
 *
 * This includes configuring pins on startup, sending SPI commands,
 * and keeping track of playback state.
 *
 * Note that only one audio clip is capable of playing at a time. All audio clips,
 * with the exception of the beep, are expected to be of equal length. If this is not the case,
 * set AUDIO_DURATION to match the longest clip duration.
 *
 * Written by Alex Tavares <tavaresa13@gmail.com>
 */

#ifndef audio_h
#define audio_h
#include <arduino.h>

/////////////////////////
// CONFIGURATION VARIABLES
/////////////////////////

// Audio clip durations
const int AUDIO_BEEP_DURATION = [TODO];
const int AUDIO_DURATION = [TODO];       // All clips other than beep


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

const byte SPI_SCLK_PIN = 0;
const byte SPI_MOSI_PIN = 1;
const byte SPI_SS_PIN = 8;


/////////////////////////
// ENUMS
/////////////////////////

// Available audio clips
typedef enum audio_clip {
	AUDIO_BEEP = 0,
	AUDIO_1 = 1,
	AUDIO_2 = 2,
	AUDIO_3 = 3
};

// Audio playback states
typedef enum audio_state {
	READY,
	PLAYING
};


/////////////////////////
// ISD1700 CONSTANTS
/////////////////////////

// ISD1700 commands list
const byte ISD_STOP = 0x02;
const byte ISD_WR_APC2 = 0x65;
const byte ISD_SET_PLAY = 0x80;

// ISD1700 configuration data
const uint16_t ISD_AUDIO_CONFIG = B0000010010100000;

// ISD1700 audio pointer array
// In format [START_1], [END_1], [START_2], etc...
const uint16_t ISD_AUDIO_PTR[8] = {
	[TODO], [TODO],
	[TODO], [TODO],
	[TODO], [TODO],
	[TODO], [TODO]
};


/////////////////////////
// AVAILABLE FUNCTIONS
/////////////////////////

void initAudio();
/*
 * Initializes audio playback
 * Must be called at startup
 *
 * Initialization involves setting status variables and pin configurations.
 * The ISD1700 configuration register is also set.
 *
 * Affects Audio_State
 */

void playAudio(audio_clip sound);
/*
 * Plays an audio clip
 *
 * Affects Audio_Start, Audio_Duration, and Audio_State
 * INPUT:  Clip to play
 */

void stopAudio();
/*
 * Stops the currently playing audio clip
 *
 * Affects Audio_State
 */

void beep();
/*
 * Plays the beep audio clip
 *
 * Affects Audio_Start, Audio_Duration, and Audio_State
 */

bool audioPlaying();
/*
 * Gets the status of audio playback
 *
 * Affects Audio_State
 * OUTPUT: State of audio playing
 */


/////////////////////////
// INTERNAL FUNCTIONS
/////////////////////////

void configureAudio(uint16_t configuration);
/*
 * Configures the ISD1700 device
 * Can be used to change volume
 *
 * INPUT:  configuration bytes to use
 */

void sendByte(byte transmission);
/*
 * Sends a single byte via SPI to the ISD1700
 * SPI_SS pin must still be manually set before and after a command
 *
 * INPUT:  Byte to transmit
 */

byte getByte(uint16_t input, byte byteSelect);
/*
 * Gets a single byte out of a two-byte input
 * Used with sendByte() to send SPI commands to the ISD1700
 *
 * INPUT:  Two-byte input
 *         Selection of output byte (0-indexed)
 * OUTPUT: Selected byte
 */


#endif