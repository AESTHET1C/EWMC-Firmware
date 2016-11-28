#include "audio.h"

unsigned long Audio_Start;
unsigned int Audio_Duration;
bool Audio_Playing = false;

void initAudio() {
	// Prepare SPI outputs
	digitalWrite(SPI_SCLK_PIN, HIGH);
	digitalWrite(SPI_MOSI_PIN, LOW);
	digitalWrite(SPI_SS_PIN, HIGH);

	// Drive SPI pins
	pinMode(SPI_SCLK_PIN, OUTPUT);
	pinMode(SPI_MOSI_PIN, OUTPUT);
	pinMode(SPI_SS_PIN, OUTPUT);

	// Initialize ISD1700 device
	digitalWrite(SPI_SS_PIN, LOW);
	sendByte(ISD_PU);
	sendByte(0x00);
	digitalWrite(SPI_SS_PIN, HIGH);
	delay(ISD_POWER_UP_DELAY);
	configAudio(ISD_APC_DEFAULT_CONFIG);

	return;
}

void initISD() {
	return;
}

void playAudio(audio_clip sound) {

	// Send play command
	digitalWrite(SPI_SS_PIN, LOW);
	sendByte(ISD_SET_PLAY);
	sendByte(0x00);
	sendByte(getByte(ISD_AUDIO_START_PTR[sound], 0));
	sendByte(getByte(ISD_AUDIO_START_PTR[sound], 1));
	sendByte(getByte(ISD_AUDIO_STOP_PTR[sound], 0));
	sendByte(getByte(ISD_AUDIO_STOP_PTR[sound], 1));
	sendByte(0x00);
	digitalWrite(SPI_SS_PIN, HIGH);

	// Update status variables
	Audio_Start = millis();
	Audio_Duration = AUDIO_DURATION[sound];
	Audio_Playing = true;

	return;
}

void beep() {
	playAudio(AUDIO_BEEP);
	while(audioPlaying()) {
		// Do nothing
	}
	delay(BEEP_DELAY);
}

bool audioPlaying() {
	if(Audio_Playing) {
		if((millis() - Audio_Start) >= Audio_Duration) {
			Audio_Playing = false;
		}
	}
	return Audio_Playing;
}

void configAudio(uint16_t configuration) {
	digitalWrite(SPI_SS_PIN, LOW);
	sendByte(ISD_WR_APC2);
	sendByte(getByte(configuration, 0));
	sendByte(getByte(configuration, 1));
	digitalWrite(SPI_SS_PIN, HIGH);
	return;
}

void sendByte(uint8_t transmission) {
	for(byte Bit = 0; Bit < 8; Bit++) {
		digitalWrite(SPI_SCLK_PIN, LOW);
		digitalWrite(SPI_MOSI_PIN, ((transmission >> Bit) & 0x01));
		digitalWrite(SPI_SCLK_PIN, HIGH);
	}
	return;
}

byte getByte(uint16_t input, byte byteSelect) {
	return((input >> (8 * byteSelect)) & 0xFF);
}
