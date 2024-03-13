unsigned int i2cFails = 0;

template<typename T> unsigned int I2C_writeAnything(const T& value) {
  Wire1.write((byte*)&value, sizeof(value));
  return sizeof(value);
}  // end of I2C_writeAnything

template<typename T> unsigned int I2C_readAnything(T& value) {
  byte* p = (byte*)&value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++)
    *p++ = Wire1.read();
  return i;
}  // end of I2C_readAnything

// code that runs on i2c request
void req() {
  int valueToReturn = abs(currentStep % 8);
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

void tellUbitToAskForData() {
  digitalWrite(interruptPin, LOW);
  i2cTimeout = millis();
  isTimedOut = false;
  //debugln("interruptUbit");
}

bool isSending = false;
void sendTracksBuffer() {
  digitalWrite(interruptPin, LOW);  //start by telling microbit to request track
  isSending = true;
  i2cTimeout = millis();
}

bool sentAMidiBuffer = false;
void sendUsbMidiPackage() {
  midiTracksBuffer16x8[8] = 200;  //200 shal be the magic number
  sentAMidiBuffer = true;         //flag the fact that we are sending midi buffer
  sendTracksBuffer();
  //debugln("sendUSBMIDI");
}

void requestEvent() {     //this is what happens when the microbit asks for a message
  if (sentAMidiBuffer) {  //this used to be only midi buffer, but is also used by other functions that need to send immediately
    debug("midiTracksBuffer = ");
    for (int i = 0; i < 10; i++) {
      debug(midiTracksBuffer16x8[i]);
      debug(" ");
    }
    debugln();
    I2C_writeAnything(midiTracksBuffer16x8);

  } else {

    debug("tracksBuffer = ");
    for (int i = 0; i < 10; i++) {
      debug(tracksBuffer16x8[i]);
      debug(" ");
    }
    debugln();
    I2C_writeAnything(tracksBuffer16x8);
  }
  //measureI2CSuccessRate();
  isSending = false;
  digitalWrite(interruptPin, HIGH);
  if (sentAMidiBuffer) {      //if we sent a midi buffer
    clearMidiTracksBuffer();  //also sets sent a midi buffer to false
  }
}

void clearMidiTracksBuffer() {    //also sets sentAMidiBuffer to false
  for (byte i = 0; i < 8; i++) {  //for every channel entry in buffer
    midiTracksBuffer16x8[i] = 0;  // clear buffer
  }
  sentAMidiBuffer = false;  //set flag back to normal buffers.
}

#define timeOut 10
void checkTimeOut() {  //check i2c
  if (millis() - i2cTimeout > timeOut && isSending) {
    digitalWrite(interruptPin, HIGH);
    isSending = false;
    Serial.println(" i2c TIMEOUT! ");
    i2cFails++;
  }
}

void triggerImmediately(byte track, byte note) {
  //hijackUSBMidiTrackBuffer(note, track);
}

void hijackUSBMidiTrackBuffer(byte val, byte slot) {
  if (!waitingForTimeOut) {
    clearMidiTracksBuffer();
    bitSet(midiTracksBuffer16x8[slot], val);  //set corresponding bit in corresponding int in the buffer to be sent
    sendUsbMidiPackage();
  }
}

void preHandleUSBNoteOn(byte inChannel, byte inNumber, byte inVelocity) {
  ////Serial.println("MIDI");
  if (inVelocity > 0) {
    HandleUsbNoteOn(inNumber, inVelocity, inChannel);
  } else {
    HandleUsbNoteOff(inNumber, inVelocity, inChannel);
  }
};

void HandleUsbNoteOn(byte note, byte velocity, byte channel) {
  
  if (channel < 9) {
    timeOutDeadline = millis() + USBReceiveTimeOutThresh;  //start the timer
    waitingForTimeOut = true;
    debug("MIDI CHANNEL: ");
    debug(channel);
    debug("  NOTE: ");
    debugln(note);
    if (isPoly[channel]) {  // if this midi channel corresponds to a polyphonic orchestra channel    
      if (note < 16) {                                //make sure we dont overflow
        Serial.println("PENG");   /// HERE IS PROBLEM AAAAAAA!!!!!!!???????? IS MIDITRACKSBUFFER NOT BEING WRITTEN TO ?
        bitSet(midiTracksBuffer16x8[channel], note);  //set corresponding bit in corresponding int in the buffer to be sent
        
      }
    } else {  //if this midi channel is controlling a monophonic (127 note) orchestra channel
      //determine witch bits we are using
      //Serial.println(channel);
      //Serial.println("MONO CHANNEL");

      if (channel == 8) {  //these are high bits on ints 6 and 7 in the buffer    // HERE IS THE ERROR!!!!?

        midiTracksBuffer16x8[channel - 1] = midiTracksBuffer16x8[channel - 1] & 0b0000000011111111;  // use bitmask to clear any previous values held in the most significant bits, leaving LSB alone
        //Serial.print("cleared = ");
        //Serial.println(midiTracksBuffer16x8[channel], BIN);
        midiTracksBuffer16x8[channel - 1] = midiTracksBuffer16x8[channel - 1] | (note << 8);  //shift note value left by 8 and compound (logical or) it to the rest
                                                                                              //Serial.print("8 = ");
                                                                                              //Serial.println(midiTracksBuffer16x8[channel], BIN);
      } else if (channel == 7) {                                                              //if it is tracks 8 or 10 they are the least significant bits
        midiTracksBuffer16x8[channel] = midiTracksBuffer16x8[channel] & 0b1111111100000000;   // use bitmask to clear any previous values held in the least significant bits, leaving MSB alone
        midiTracksBuffer16x8[channel] = midiTracksBuffer16x8[channel] | note;                 // compound (logical or) the int in the buffer with the note we want to add
                                                                                              //Serial.print("7 = ");
                                                                                              //Serial.println(midiTracksBuffer16x8[channel], BIN);
      }
      //debugInt(midiTracksBuffer16x8[7]);
    }
  }
}

void checkUSBMidiTimout() {
  if (waitingForTimeOut) {             //if we are waiting to see if there are any more messages for this step
    if (millis() > timeOutDeadline) {  //if we timed out
      sendUsbMidiPackage();

      waitingForTimeOut = false;
    }
  }
}

void HandleUsbNoteOff(byte note, byte velocity, byte channel) {
}

/*
void sendMutes() {
	if (!waitingForTimeOut) { // waitingfortimeout is waiting for more midi messages before sending radiomessage
		midiTracksBuffer16x8[9] = isMutedByte;
		//sendUsbMidiPackage(); some kind of hack i guess to send current data
	}
}
*/
