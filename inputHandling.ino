

byte AY = 11;
byte BY = 12;
byte CY = 13;

byte AX = 20;
byte BX = 19;
byte CX = 18;

byte inGridA = 16;  // left 8x8
byte inGridB = 17;  // right 8x8
unsigned long playButtonDebounceTimer = 0;


void handleButtPress(byte x, byte y) {  //also handles buttrelease
  switch (mode) {
    case instSeqMode:
      instSeqModeButts(x, y);
      break;
    case overviewMode:
      overviewModeButts(x, y);
      break;
    case lastStepMode:
      lastStepModeButts(x, y);
      break;
  }
}

void handleButtRelease(byte x, byte y) {  //also handles buttrelease
  testColor = 0;
  byte flippedY = 8 - y;
  byte noteNumber = 64 + flippedY;
  //MIDI.sendNoteOff(noteNumber, 0, 1);
}


//INVERTS THE Y VALUE
void instSeqModeButts(byte x, byte y) {
  if (shiftState) {
    //debug("x = ");
    //debug(x);
    //debug("  y = ");
    //debugln(y);
    byte slotSelect = x;
    switch (x) {
      case 0:
      case 1:
      case 2:
      case 3:
        
        if (y == 0) {      //top row sequence (page) select buttons
          currentSeq = x;  // Seq 1 2 3 or 4
          numberToDisplay = x + 1;
          textDisplayStartTime = millis();
          displayNumber(x, 4, 3);
        }

        //Save
        if (y == 7) {  //bottom row save buttons
          byte slotToSaveTo = x;
          if (buttStates2D[13][7] == true) {  //is the save button pressed?
            saveCurrentSequenceToFile(slotSelect, currentInst);
          } else {
            if (!utilState) {
              //debug("load this instrument from slot ");
              recallSequenceFromFile(slotSelect, currentInst);
            } else {
              //debug("load all instruments from slot ");
              recallSequenceFromFile(slotSelect, ALLTRACKS);
            }
          }
        }
        break;

      case 4:
      case 5:
      case 6:
      case 7:
      
          if (y == 0) {      //top row sequence (page) select buttons
          currentSeq = x;  // Seq 1 2 3 or 4
          numberToDisplay = x + 1;
          textDisplayStartTime = millis();
          displayNumber(x, 4, 3);
        }
        break;
      case 8:  //clear
        break;

      case 9:  //randomize
        if (shiftState && y == 7) {
          randomSeq();
        }
        break;

      case 10:
        break;

      case 11:
        break;

      case 12:
        if (y == 7) {
          clearTrack(currentSeq, currentInst);
        }
        break;

      case 13:  //solo
        break;

      case 14:  //mute
        break;

      case 15:  //sel / trig

        addSporkle(15, y, 200, 200, 200, 0, 0, 0, 500);
        addSporkle(14, y, 100, 100, 100, 0, 0, 0, 500);
        addSporkle(13, y, 30, 30, 30, 0, 0, 0, 500);
        addSporkle(12, y, 5, 5, 5, 0, 0, 0, 500);

        triggerMidiNote((7 - y) + viewOffset, currentInst);
        bitSet(midiTracksBuffer16x8[currentInst], (7 - y) + viewOffset);  //set corresponding bit in corresponding uint16_t in the buffer to be sent
        waitingForMIDITimeOut = false;

        sendUsbMidiPackage();

        break;
    }
  } else {  // if shift is not held in
    byte invertedY = (GRIDROWS - 1) - y;
    byte invertedOffsetY = invertedY + viewOffset;
    //debug("toggling note ");
    //debug(invertedOffsetY);
    toggleNote(currentSeq, currentInst, x, invertedOffsetY);  //toggle the note stored in seq 0, intrument 0, step x, note y
    //debug(" it is now ");
    //debugln(getNote(currentSeq, currentInst, x, invertedOffsetY));
    if (!getNote(currentSeq, currentInst, x, invertedOffsetY)) {  //if we just turned this note off
      removeSparkle(x, y);                                        // stop the sparkle
    }
    testColor = 255;
    byte flippedY = 8 - y;
    byte noteNumber = 64 + flippedY;
  }
}

void lastStepModeButts(byte x, byte y){
  lastStep = x+1;
}

const byte muteSoloRow = 14;
const byte selectPreviewRow = 15;
void overviewModeButts(byte x, byte y) {
  byte trackSelect = 0;
  byte slotSelect = x;  // Assign values within the cases
  switch (x) {
    case selectPreviewRow:  //15
      currentInst = y;
      mode = instSeqMode;
      break;

    case muteSoloRow:  //14
      if (shiftState) toggleSolo(y);
      if (!shiftState) toggleMute(y);
      break;

    case 13:
      if (y == 0) {
        if(shiftState){
          mode = lastStepMode;
        }
      }
      break;

    case 12:  //clear
      if (shiftState) {
        if (utilState) {
          if (y == 7) clearInst(currentSeq, ALLTRACKS);
        } else {
          clearInst(currentSeq, y);
        }
      }
      break;

    case 11:
      // transpose is handled from rotaryEncoder
      break;

    case 10:
      // Slip is handled from Rotary encoder
      break;

    case 9:
      //if (shiftState) generateSequence(DRUMSEQ, MINORSCALE, currentSeq, y); //void generateSequence(bool seqType, byte scaleType, uint64_t* outArray, size_t outArraySize) {
      break;

    case 8:  // song mode (use rotary to switche between all or poly)
      break;

    case 7:
    case 6:
    case 5:
    case 4:
    case 3:
    case 2:
    case 1:
    case 0:
      trackSelect = y;
      if (utilState) {
        trackSelect = 8;
      }
      if (buttStates2D[13][7] == true) {  //is the save/load button pressed?
        if (shiftState) {
          saveCurrentSequenceToFile(slotSelect, trackSelect);
          for (byte i = 0; i < 8; i++) {
            addSporkle(x, i, 50, 0, 0, 0, 0, 0, sparkleLifespan);  // make that pixel sparkle for 500ms, also invert Y axis
          }
          for (byte i = 0; i < 15; i++) {
            addSporkle(i, y, 50, 0, 0, 0, 0, 0, sparkleLifespan);  // make that pixel sparkle for 500ms, also invert Y axis
          }

        } else {                      // if shift is not held in it means load
          if (utilState && y == 7) {  // if all button is held in and we are using borrom row
            for (int i = 0; i < 8; i++) {
              recallSequenceFromFile(slotSelect, i);
            }
          } else {
            recallSequenceFromFile(slotSelect, y);
            for (byte i = 0; i < 8; i++) {
              addSporkle(x, i, 0, 50, 50, 0, 0, 0, sparkleLifespan);  // make that pixel sparkle for 500ms, also invert Y axis
            }
            for (byte i = 0; i < 15; i++) {
              addSporkle(i, y, 0, 50, 50, 0, 0, 0, sparkleLifespan);  // make that pixel sparkle for 500ms, also invert Y axis
            }
          }
        }
      } else {             // if load/save button is not held in _________-
        if (y == 0) {      // if top row page select buttons
          currentSeq = x;  // Seq 1 2 3 or 4
          numberToDisplay = x + 1;
          textDisplayStartTime = millis();
        }
        //displayNumber(x, 4, 3);
      }
      break;

    default:
      break;
  }
  debugln(buttStates2D[13][7]);
}


void setupGridPins() {
  pinMode(AX, OUTPUT);  //AX
  pinMode(BX, OUTPUT);  //BX
  pinMode(CX, OUTPUT);  //CX

  pinMode(AY, OUTPUT);  //AY
  pinMode(BY, OUTPUT);  //BY
  pinMode(CY, OUTPUT);  //CY

  pinMode(inGridA, INPUT);  //inGridA
  pinMode(inGridB, INPUT);  //inGridA
}

void setXplex(int number) {
  // Convert the number to binary and write the corresponding values to the pins
  digitalWrite(AX, (number & 0b001));
  digitalWrite(BX, (number & 0b010) >> 1);
  digitalWrite(CX, (number & 0b100) >> 2);
}

void setYplex(int number) {
  digitalWrite(AY, (number & 0b001));
  digitalWrite(BY, (number & 0b010) >> 1);
  digitalWrite(CY, (number & 0b100) >> 2);
}

void scanGrid() {
  for (byte y = 0; y < 8; y++) {
    setYplex(y);
    for (byte x = 0; x < 16; x++) {  // Combining both grids into one loop
      setXplex(x % 8);
      bool scanningA = x < 8;  // Check if x is in the left grid (0-7) or right grid (8-15), less than 8 means true
      byte offsetX = x;
      byte pinToRead = inGridA;  // default pinToRead to inGridA
      if (!scanningA) {          // if we are scanning B
        pinToRead = inGridB;     // change pinToRead to inGridB if are on the second grid
      }
      if (digitalRead(pinToRead)) {       //check inGridA pin or inGridB pin depending on what we set pinToRead to
        if (!buttStates2D[offsetX][y]) {  // if it was not pushed previously
          buttStates2D[offsetX][y] = true;
          handleButtPress(offsetX, y);
        }
      } else {
        if (buttStates2D[offsetX][y]) {  // if it was previously pushed in
          buttStates2D[offsetX][y] = false;
          handleButtRelease(offsetX, y);
        }
      }
    }
  }
}




unsigned int oldRawKnobVal = 0;
unsigned int oldKnobVal = 0;
//bool oldUtilState = false
void scanButtsNKnobs() {
  shiftState = !digitalRead(SHIFTPIN);
  if (shiftState) {
    scrollTimer = millis();
  }
  rotarySwitchState = !digitalRead(swPin);
  utilState = !digitalRead(UTILPIN);
  if (utilState != oldUtilState) {
    oldUtilState = utilState;
    if (utilState) {
      if (mode == overviewMode) {
        //mode = instSeqMode;
      } else {
        mode = overviewMode;
      }
      //debugln("butt pressed");
    } else {
      //debugln("butt released");
    }
  }

  if (millis() > playButtonDebounceTimer) {
    playButtonState = !digitalRead(PLAYPAUSEPIN);
    if (playButtonState != oldPlayButtonState) {
      playButtonDebounceTimer = millis() + 30;        // Update the time of state change
      handlePlayButtonStateChange();         // Handle the state change
      oldPlayButtonState = playButtonState;  // Update the old state only after confirming a state change
    }
  }
}

void handlePlayButtonStateChange() {
  if (playButtonState) {  // when button goes from off to on
    //check if we are under midi clock:
    if (!midiClockRunning) {
      if (!shiftState) {
        playing = !playing;
      } else {
        playing = true;
      }
      if (!playing) {
        currentStep = -1;
        HWMIDI.sendStop();
      } else if (playing) {
        HWMIDI.sendStart();
        HWMIDI.sendClock();
        counter24ppqn = 0;
        lastStepTime = micros();
        currentStep = 0;
        handleStep(currentStep);
      }
    } else {
      if (shiftState) {
        //HWMIDI.sendStop();
        lastStepTime = micros();
        currentStep = 0;
        midiClockCounter = 0;
        counter24ppqn = 0;
        handleStep(currentStep);
      }
    }
  }
}

int mapToTempoExp(int input) {
  int maxTempo = 999;
  int linearMapped = map(input, 0, 127, 40, maxTempo);                // Map the input linearly first
  int percentage = (linearMapped - 10) * 100 / (maxTempo - 10);       // Here, the idea is to create a "percentage" of how far along we are in our linear range
  int squaredPercentage = (percentage * percentage) / 100;            // Then, square this percentage (still maintaining integer operations), Dividing by 100 to bring it back to a "percentage"
  int expoMapped = 10 + (squaredPercentage * (maxTempo - 10) / 100);  // Compute the final exponential mapped value
  int rounded = (expoMapped + 5) / 10 * 10;                           // Round to nearest 10
  return rounded;
}

int mapToTempo(int input) {
  int maxTempo = 999;
  int mapped = map(input, 0, 127, 40, maxTempo);  // Map the input from [0, 127] to [10, maxTempo]
  int rounded = (mapped + 5) / 10 * 10;           // To round to the nearest multiple of 10, we can add half the step (5), then truncate to the lower multiple of 10.
  return rounded;
}

int tempoToMillis(int bpm) {
  unsigned int tempMillis = round(60000.0 / bpm);
  tempMillis = tempMillis >> 2;
  return tempMillis;
}

int tempoToMicros(int bpm) {
  unsigned int tempMicros = round(60000000.0 / bpm);  // Adjusted for microseconds
  tempMicros = tempMicros >> 2;                       // This operation remains the same, as it's independent of the unit.
  return tempMicros;
}

int tempoToMicros24(int bpm) {
  // Calculate the number of microseconds for one beat
  unsigned int tempMicros = round(60000000.0 / bpm);

  // Divide by 24 to get the time for one MIDI Clock pulse
  tempMicros = tempMicros / 24;

  return tempMicros;
}


void handleRotaryPush() {  //rotaryclick
  swState = digitalRead(swPin);
  if (swState != swLastState) {
    if (millis() > lastSwTime + swDebounceTime) {
      rotaryPushState = !swState;
      if (rotaryPushState) {
        currentScale += 1;
        currentScale = currentScale % 5;
        displayText(scaleNames[currentScale], 0, 0, true);
      }
      lastSwTime = millis();
    }
  }
  swLastState = swState;
}

void handleRotary() {
  // Read the current state of clkPin
  int clkState = digitalRead(clkPin);
  int dootState = digitalRead(dtPin);
  //if (clkState != clkLastState) {
  if (clkState != clkLastState) {
    if (millis() > lastClkTime + clkDebounceDelay) {
      if (clkState == 1) {
        if (dootState != clkState) {
          rotaryMasterCounter--;
          rotaryMove(-1);
          //debugln("- ");
        } else {
          rotaryMasterCounter++;
          rotaryMove(1);
          //debugln("+ ");
        }
        if (rotaryMasterCounter < 0) {
          rotaryMasterCounter = rotaryMasterCounter * -1;
        }
        //drawRotaryMasterCounterOverSerial();
      }
      lastClkTime = millis();
    }
  }
  // Update clkLastState for the next loop iteration
  clkLastState = clkState;
}

void drawRotaryMasterCounterOverSerial() {
  //debug("rotary = ");
  //debug(rotaryMasterCounter);
  //debug(" ");
  for (int i = 0; i < rotaryMasterCounter; i++) {
    //debug("#");
  }
  //debugln();
}


// add transpose here!
void rotaryMove(int moveAmount) {

  if (shiftState && mode == overviewMode) {
    if (!rotaryPushState) {
      tempo = tempo + moveAmount;
    } else {
      tempo = tempo + moveAmount * 10;
    }
  } else if (!buttStates2D[10][7]) {  // if slip button is not held in
    viewOffset += moveAmount;
    if (viewOffset > maxViewOffset) viewOffset = maxViewOffset;
    if (viewOffset < 0) viewOffset = 0;
  }
  bool directionBool = false;
  if (moveAmount > 0) {
    directionBool = true;
  }
  // check if slip button is held in
  for (byte i = 0; i < 8; i++) {
    if (buttStates2D[10][i]) {  // slip
      if (mode == overviewMode) {
        slip(currentSeq, i, directionBool);
      } else {
        if (i == 7) {
          slip(currentSeq, currentInst, directionBool);
        }
      }
    }
  }

  //check if a transpose button is held in
  for (byte i = 0; i < 8; i++) {
    if (buttStates2D[11][i]) {  // transpose
      if (mode == overviewMode) {
        transpose(currentSeq, i, directionBool);
      } else {
        if (shiftState) {
          if (i == 7) {
            transpose(currentSeq, currentInst, directionBool);
          }
        }
      }
    }
  }

  if (transposeState) {
    if (moveAmount > 0) {
      transpose(currentSeq, transposeTrack, true);
    } else {
      transpose(currentSeq, transposeTrack, true);
    }
  }

  if (tempo > 999) {
    tempo = 999;
  } else if (tempo < 0) {
    tempo = 0;
  }
}