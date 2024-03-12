
unsigned int i2cFails = 0;

template <typename T> unsigned int I2C_writeAnything (const T& value)
  {
  Wire.write((byte *) &value, sizeof (value));
  return sizeof (value);
  }  // end of I2C_writeAnything

template <typename T> unsigned int I2C_readAnything(T& value)
  {
    byte * p = (byte*) &value;
    unsigned int i;
    for (i = 0; i < sizeof value; i++)
          *p++ = Wire.read();
    return i;
  }  // end of I2C_readAnything

// code that runs on i2c request
void req() { 
  int valueToReturn = abs(currentStep %8);
  Wire1.write(valueToReturn);
  //debugln("gotRequest");
}


// INTERRUPT uBit //

unsigned long i2cTimeout = 0;
byte i2cTimeoutDuration = 20;
bool isTimedOut = false;
void handleI2CTimeout() {
  if (millis() - i2cTimeout > i2cTimeoutDuration && isTimedOut == false) {
    digitalWrite(interruptPin, HIGH);
    i2cTimeout == millis();
    //debugln("i2c timeout");
    isTimedOut = true;
  }
}

void tellUbitToAskForData(){
  digitalWrite(interruptPin, LOW);
    i2cTimeout = millis();
    isTimedOut = false;
    //debugln("interruptUbit");
}

bool isSending = false;
void sendTracksBuffer() {
	Serial.println("tracksBuffer = ");
	for (int i = 0; i < 10; i++) {
		debugln(tracksBuffer16x8[i]);
	}
	digitalWrite(interruptPin, LOW); //start by telling microbit to request track
	isSending = true;
	timeOutStamp = millis();
}

bool sentAMidiBuffer = false;
void sendUsbMidiPackage() {
	midiTracksBuffer16x8[8] = 200; //200 shal be the magic number
	sentAMidiBuffer = true; //flag the fact that we are sending midi buffer
	sendTracksBuffer();
}

void requestEvent() {  //this is what happens when the microbit asks for a message
	if (sentAMidiBuffer) {  //this used to be only midi buffer, but is also used by other functions that need to send immediately
		I2C_writeAnything(midiTracksBuffer16x8);
	}
	else {
		I2C_writeAnything(tracksBuffer16x8);
	}
	//measureI2CSuccessRate();
	isSending = false;
	digitalWrite(interruptPin, HIGH);
	if (sentAMidiBuffer) {											//if we sent a midi buffer
		clearMidiTracksBuffer();								//also sets sent a midi buffer to false
	}
}

void clearMidiTracksBuffer() {									//also sets sentAMidiBuffer to false
	for (byte i = 0; i < 8; i++) {								//for every channel entry in buffer
		midiTracksBuffer16x8[i] = 0;							// clear buffer
	}
	sentAMidiBuffer = false;									//set flag back to normal buffers.
}



#define timeOut  10
void checkTimeOut() {
	if (millis() - timeOutStamp > timeOut && isSending) {
		digitalWrite(interruptPin, HIGH);
		isSending = false;
		//Serial.println(" i2c TIMEOUT! ");
		i2cFails++;
	}
}

void triggerImmediately(byte track, byte note) {
		//hijackUSBMidiTrackBuffer(note, track);
}

void sendMutes() {
	if (!waitingForTimeOut) { // waitingfortimeout is waiting for more midi messages before sending radiomessage
		midiTracksBuffer16x8[9] = isMutedByte;
		//sendUsbMidiPackage(); some kind of hack i guess to send current data
	}
}

//microbit read code
//let myNum = pins.i2cReadNumber(48, NumberFormat.UInt8LE, false);

