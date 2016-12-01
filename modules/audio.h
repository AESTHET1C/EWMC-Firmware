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

const unsigned int BEEP_DELAY = 150;

// Audio clip durations
const unsigned int AUDIO_DURATION[] = {
	100,
	2553,
	2506,
	854,
	1000
};


/////////////////////////
// PIN DEFINITIONS
/////////////////////////

const byte SPI_SCLK_PIN = 0;
const byte SPI_MOSI_PIN = 1;
const byte SPI_SS_PIN = 8;


/////////////////////////
// ENUMERATIONS
/////////////////////////

// Available audio clips
typedef enum {
	AUDIO_BEEP = 0,
	AUDIO_EXPLOSION = 1,
	AUDIO_CANARY = 2,
	AUDIO_COUGH_1 = 3,
	AUDIO_COUGH_2 = 4
} audio_clip;

/////////////////////////
// ISD1700 CONSTANTS
/////////////////////////

const byte ISD_POWER_UP_DELAY = 50;

// Commands list
const byte ISD_PU = 0x01;
const byte ISD_WR_APC2 = 0x65;
const byte ISD_SET_PLAY = 0x80;

// Configuration data
const uint16_t ISD_APC_DEFAULT_CONFIG = ((B00000100 << 8) + B10100000);

// Audio pointer arrays
const uint16_t ISD_AUDIO_START_PTR[5] = {
	0x010,
	0x011,
	0x028,
	0x03F,
	0x047
};
const uint16_t ISD_AUDIO_STOP_PTR[5] = {
	0x010,
	0x027,
	0x03E,
	0x046,
	0x04F
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

void initISD();
/*
 * Initializes the ISD1700 device
 */

void playAudio(audio_clip sound);
/*
 * Plays an audio clip without blocking additional code from running
 *
 * Affects Audio_Start, Audio_Duration, and Audio_State
 * INPUT:  Clip to play
 */

void beep();
/*
 * Plays the BEEP audio clip
 *
 * This function waits for the beep to complete, then delays an additional BEEP_DELAY milliseconds.
 *
 * Affects Audio_Start and Audio_Duration
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

void configAudio(uint16_t configuration);
/*
 * Configures the ISD1700 device
 * Can be used to change volume
 *
 * INPUT:  configuration bytes to use
 */

void sendByte(uint8_t transmission);
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
