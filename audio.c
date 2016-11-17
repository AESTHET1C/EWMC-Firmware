long Audio_Start;
int Audio_Duration;
audio_state Audio_State = READY;

void initAudio() {
	// TODO
	// Set pins to outputs, using pull-up resistors
	// Set SPI_SS_PIN and SPI_SCLK_PIN high, and SPI_MOSI_PIN low
	configAudio(ISD_APC_DEFAULT_CONFIG);
	return;
}

void playAudio(audio_clip sound) {
	DigitalWrite(SPI_SS_PIN, LOW);
	sendByte(SET_PLAY);
	sendByte(0x00);
	sendByte(getByte(ISD_AUDIO_PTR[sound * 2], 0));
	sendByte(getByte(ISD_AUDIO_PTR[sound * 2], 1));
	sendByte(getByte(ISD_AUDIO_PTR[(sound * 2) + 1], 0));
	sendByte(getByte(ISD_AUDIO_PTR[(sound * 2) + 1], 1));
	sendByte(0x00);
	DigitalWrite(SPI_SS_PIN, HIGH);
	return;
}

void stopAudio() {
	DigitalWrite(SPI_SS_PIN, LOW);
	sendByte(STOP);
	sendByte(0x00);
	DigitalWrite(SPI_SS_PIN, HIGH);
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

void configAudio(uint16_t configuration) {
	DigitalWrite(SPI_SS_PIN, LOW);
	sendByte(WR_APC2);
	sendByte(getByte(configuration, 0));
	sendByte(getByte(configuration, 1));
	DigitalWrite(SPI_SS_PIN, HIGH);
	return;
}

void sendByte(uint8_t transmission) {
	for(byte Bit = 0; Bit < 8; Bit++) {
		DigitalWrite(SPI_SCLK_PIN, LOW);
		DigitalWrite(SPI_MOSI_PIN, ((transmission >> Bit) && 0x01));
		DigitalWrite(SPI_SCLK_PIN, HIGH);
	}
	return;
}

byte getByte(uint16_t input, byte byteSelect) {
	return((input >> (8 * byteSelect)) && 0xFF);
}