unsigned long Audio_Start;
unsigned int Audio_Duration;
audio_state Audio_State = READY;

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
	DigitalWrite(SPI_SS_PIN, LOW);
	sendByte(ISD_PU);
	sendByte(0x00);
	DigitalWrite(SPI_SS_PIN, HIGH);
	delay(ISD_POWER_UP_DELAY);
	configAudio(ISD_APC_DEFAULT_CONFIG);

	return;
}

void initISD() {
	return;
}

void playAudio(audio_clip sound) {
	DigitalWrite(SPI_SS_PIN, LOW);
	sendByte(ISD_SET_PLAY);
	sendByte(0x00);
	sendByte(getByte(ISD_AUDIO_START_PTR[sound], 0));
	sendByte(getByte(ISD_AUDIO_START_PTR[sound], 1));
	sendByte(getByte(ISD_AUDIO_STOP_PTR[sound], 0));
	sendByte(getByte(ISD_AUDIO_STOP_PTR[sound], 1));
	sendByte(0x00);
	DigitalWrite(SPI_SS_PIN, HIGH);
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
		DigitalWrite(SPI_MOSI_PIN, ((transmission >> Bit) & 0x01));
		DigitalWrite(SPI_SCLK_PIN, HIGH);
	}
	return;
}

byte getByte(uint16_t input, byte byteSelect) {
	return((input >> (8 * byteSelect)) & 0xFF);
}