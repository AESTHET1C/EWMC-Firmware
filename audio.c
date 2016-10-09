long Audio_Start;
int Audio_Duration;
audio_state Audio_State = READY;

void initAudio() {
	// TODO
	// Set pins to outputs, using pull-up resistors
	// Set SPI_SS_PIN and SPI_SCLK_PIN high, and SPI_MOSI_PIN low
	configureAudio(ISD_AUDIO_CONFIG);
	return;
}

void playAudio(sound) {
	// Set SPI_SS_PIN to LOW
	sendByte(SET_PLAY);
	sendByte(0x00);
	sendByte(getByte(ISD_AUDIO_PTR[sound * 2], 0));
	sendByte(getByte(ISD_AUDIO_PTR[sound * 2], 1));
	sendByte(getByte(ISD_AUDIO_PTR[(sound * 2) + 1], 0));
	sendByte(getByte(ISD_AUDIO_PTR[(sound * 2) + 1], 1));
	sendByte(0x00);
	// Set SPI_SS_PIN to HIGH
	return;
}

void stopAudio() {
	// Set SPI_SS_PIN to LOW
	sendByte(STOP);
	sendByte(0x00);
	// Set SPI_SS_PIN to HIGH
	Audio_State = AUDIO_STATE_READY;
	return;
}

void beep() {
	playAudio(AUDIO_BEEP);
	while(audioPlaying()) {
		// Do nothing
	}
}

bool audioPlaying() {
	if(Audio_State == AUDIO_STATE_PLAYING) {
		if((millis() - Audio_Start) >= Audio_Duration) {
			Audio_State = AUDIO_STATE_READY
		}
	}
	return(Audio_State == AUDIO_STATE_PLAYING);
}

void configureAudio(configuration) {
	// Set SPI_SS_PIN to LOW
	sendByte(WR_APC2);
	sendByte(getByte(configuration, 0));
	sendByte(getByte(configuration, 1));
	// Set SPI_SS_PIN to HIGH
	return;
}

void sendByte(transmission) {
	for(byte i = 0; i < 8; i++) {
		// Set SPI_SCLK_PIN to LOW
		// Set SPI_MOSI_PIN to ((transmission >> i) && 0x01)
		// Set SPI_SCLK_PIN to HIGH
	}
	return;
}

byte getByte(input, byteSelect) {
	return((input >> (8 * byteSelect)) && 0xFF);
}