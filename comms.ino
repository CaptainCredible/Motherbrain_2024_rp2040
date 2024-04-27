

 
//should I just add midi notes to the existing tracksBuffer?


volatile unsigned long MIDItimeOutDeadline = 0;  // to store the deadline for midi message
volatile bool waitingForMIDITimeOut = false;  // are we waiting for more midi notes to come in?
volatile unsigned long i2cTimeout = 0;
byte i2cTimeoutDuration = 20;
volatile bool isTimedOut = false;
volatile bool sentAMidiBuffer = false;

// COMMS I2C
volatile bool isSending = false;
void sendTracksBuffer() {
  interruptUbit();
}

void interruptUbit() {
  ////debugln("intteruptUbit()");
  if (!isSending) {
    isSending = true;
    digitalWrite(interruptPin, LOW);  //start by telling microbit to request track
    i2cTimeout = millis();
    isTimedOut = false;
  }
}

void requestEvent() {  //this is what happens when the microbit asks for a message
  //debug("requestEvent() ");
  if (sentAMidiBuffer) {  //this used to be only midi buffer, but is also used by other functions that need to send immediately
    //debugln("sent midiBuffer");
    I2C_writeAnything(midiTracksBuffer16x8);
    clearMidiTracksBuffer();
  } else {
    I2C_writeAnything(tracksBuffer16x8);
    //debug("sent tracksBuffer ");
    //debugln(currentStep);
  }
  isSending = false;
  digitalWrite(interruptPin, HIGH);
  isTimedOut = true;
}

// INTERRUPT uBit //
void handleI2CTimeout() {
  if (millis() - i2cTimeout > i2cTimeoutDuration && isTimedOut == false) {
    ////debugln("handleI2cTimeout()");
    digitalWrite(interruptPin, HIGH);
    i2cTimeout == millis();
    isTimedOut = true;
    isSending = false;
  }
}

//MIDI COMMS
unsigned long lastMIDIMessageTime = 0;
int USBReceiveTimeOutThresh = 5;  //timeframe to wait for midi notes
void HandleUsbNoteOn(byte channel, byte note, byte velocity) {
  //debugln("handleUSBNoteOn()");
  if (velocity > 0) {
    if (channel < 9) {
      lastMIDIMessageTime = millis();  //start the timer
      waitingForMIDITimeOut = true;
      if (isPoly[channel]) {                                // if this midi channel corresponds to a polyphonic orchestra channel
        if (note < 16) {                                    //make sure we dont overflow
          bitSet(midiTracksBuffer16x8[channel - 1], note);  //set corresponding bit in corresponding uint16_t in the buffer to be sent
          addSparkle(15, channel - 1, trackColors[channel - 1][0], trackColors[channel - 1][1], trackColors[channel - 1][2], sparkleLifespan);
        }
      }
      /*
    //MONO TRACK CODE:
    else {  //if this midi channel is controlling a monophonic (127 note) orchestra channel
      if (channel == 8) {  //these are high bits on ints 6 and 7 in the buffer
        midiTracksBuffer16x8[channel - 1] = midiTracksBuffer16x8[channel - 1] & 0b0000000011111111;  // use bitmask to clear any previous values held in the most significant bits, leaving LSB alone
        midiTracksBuffer16x8[channel - 1] = midiTracksBuffer16x8[channel - 1] | (note << 8);         //shift note value left by 8 and compound (logical or) it to the rest
      } else if (channel == 7) {                                                                     //if it is tracks 8 or 10 they are the least significant bits
        midiTracksBuffer16x8[channel] = midiTracksBuffer16x8[channel] & 0b1111111100000000;          // use bitmask to clear any previous values held in the least significant bits, leaving MSB alone
        midiTracksBuffer16x8[channel] = midiTracksBuffer16x8[channel] | note;                        // compound (logical or) the int in the buffer with the note we want to add
      }
    }
    */
    }
  }
}

unsigned long idleTimer = 0;
void checkUSBMidiTimout() {
  uint16_t timeSinceMIDIMessage = millis() - lastMIDIMessageTime;
  if (waitingForMIDITimeOut) {             //if we are waiting to see if there are any more messages for this step
    //debugln(timeSinceMIDIMessage);
    if (timeSinceMIDIMessage>USBReceiveTimeOutThresh) {  //if we timed out
      waitingForMIDITimeOut = false;
      //debugln("usbmidi Timed Out");
      sendUsbMidiPackage();
      //idleTimer = millis();
    }
  } else {
    //debug("idle");
    //debugln(millis() - idleTimer);
  }
}

void sendUsbMidiPackage() {
  //debugln("sendUSBMidiPackage()");
  if (!isSending) {
    midiTracksBuffer16x8[8] = 200;  //200 shall be the magic number
    sentAMidiBuffer = true;         //flag the fact that we are sending midi buffer
    interruptUbit();
  }
}

void clearTracksBuffer() {
  for (int i = 0; i < 8; i++) {
    tracksBuffer16x8[i] = 0;
    //debugln("cleared buffer");
  }
}

void clearMidiTracksBuffer() {  //also sets sentAMidiBuffer to false
  //debugln("clearMidiTracksBuffer()");
  //debugln();
  for (byte i = 0; i < 8; i++) {  //for every channel entry in buffer
    midiTracksBuffer16x8[i] = 0;  // clear buffer
  }
  sentAMidiBuffer = false;  //set flag back to normal buffers.
}

void hijackUSBMidiTrackBuffer(byte val, byte slot) {  // to use for preview buttons
  //debugln("hijackMidiTracksBuffer");
  if (!waitingForMIDITimeOut) {
    clearMidiTracksBuffer();
    bitSet(midiTracksBuffer16x8[slot], val);  //set corresponding bit in corresponding int in the buffer to be sent
    sendUsbMidiPackage();
  }
}

void HandleUsbNoteOff(byte note, byte velocity, byte channel) {
}

void midiClockStep() {
  currentStep = (currentStep + 1) % lastStep;
  //
}

void debugTracksBuffer() {
  debug("tracksBuffer = ");
  for (int i = 0; i < 10; i++) {
    debug(tracksBuffer16x8[i]);
    debug(" ");
  }
  debugln();
}

void debugMidiTracksBuffer() {
  //debug("midiTracksBuffer = ");
  printAsBinary(midiTracksBuffer16x8, 10);
}

void printAsBinary(volatile uint16_t *array, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    uint16_t number = array[i];
    for (int bit = 15; bit >= 0; --bit) {
      if (number & (1 << bit)) {
        //debug("1");
      } else {
        //debug("0");
      }
    }
    if (i < size - 1) {
      //debug(" ");
    }
  }
  //debugln();
}
