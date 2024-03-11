/* 
FUNCTIONS NEEDED:

clearLeds()
changeColor (byte r, byte g, byte b)
setPixel (byte x, byte y, byte r, byte g, byte b)
setCursorColor(byte r, byte g, byte b)
drawCursor(byte step)

writeText

updateScreen()

*/

byte AY = 11;
byte BY = 12;
byte CY = 13;

byte AX = 20;
byte BX = 19;
byte CX = 18;

byte inGridA = 16;  // left 8x8
byte inGridB = 17;  // right 8x8



void handleButtPress(byte x, byte y) {  //also handles buttrelease
  switch (mode) {
    case instSeqMode:
      instSeqModeButts(x, y);
      break;
    case overviewMode:
      overviewModeButts(x, y);
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
    debug("x = ");
    debug(x);
    debug("  y = ");
    debugln(y);

    switch (x) {
      case 0:
      case 1:
      case 2:
      case 3:
        //SAVE
        if (y == 0) {      //top row sequence (page) select buttons
          currentSeq = x;  // Seq 1 2 3 or 4
          numberToDisplay = x + 1;
          textDisplayStartTime = millis();
          displayNumber(x, 4, 3);
        } else if (y == 7) {  //bottom row save buttons
          byte slotToSaveTo = x;
          //debug("save instrument to slot ");
          //debugln(slotToSaveTo);
          saveCurrentSequenceToEEPROM(slotToSaveTo, currentInst);
        }
        break;

      case 4:
      case 5:
      case 6:
      case 7:
        {
          //LOAD
          byte slotToLoadFrom = x - 4;  //change x numbers 4,5,6,7 to slot numbers 0,1,2,3
          if (!utilState) {
            //debug("load this instrument from slot ");
            recallSequenceFromEEPROM(slotToLoadFrom, currentInst);
          } else {
            //debug("load all instruments from slot ");
            recallSequenceFromEEPROM(slotToLoadFrom, ALLTRACKS);
          }
          //debugln(slotToLoadFrom);
          break;
        }

      case 8:  //clear
        if (y == 7) {
          clearTrack(currentSeq, currentInst);
        }
        break;

      case 9:  //randomize
        randomize(currentSeq, currentInst);
        break;

      case 10:
        break;

      case 11:  // nuke
        break;

      case 12:
        break;

      case 13:  //solo
        break;

      case 14:  //mute
        break;

      case 15:  //sel
        break;
    }
  } else {  // if shift is not held in
    byte invertedY = (GRIDROWS - 1) - y;
    byte invertedOffsetY = invertedY + viewOffset;
    //debugln(invertedOffsetY);
    toggleNote(currentSeq, currentInst, x, invertedOffsetY);      //toggle the note stored in seq 0, intrument 0, step x, note y
    if (!getNote(currentSeq, currentInst, x, invertedOffsetY)) {  //if we just turned this note off
      removeSparkle(x, y);                                        // stop the sparkle
    }
    testColor = 255;
    byte flippedY = 8 - y;
    byte noteNumber = 64 + flippedY;
  }
}


void overviewModeButts(byte x, byte y) {
  byte loadSelect = 0;  // Initialize the variables outside the switch
  byte saveSlotSelect = 0;
  byte trackSelect = 0;
  if (shiftState) {
    switch (x) {
      case 9:
        randomize(currentSeq, y);
        debug("randomized seq ");
        debugln(y);
        break;
      case 8:  // clear
        if (utilState) {
          if (y == 7) clearInst(currentSeq, ALLTRACKS);
        } else {
          clearInst(currentSeq, y);
        }

        break;
      case 7:
      case 6:
      case 5:
      case 4:
        loadSelect = x - 4;  // Assign values within the cases
        debug("load number ");
        debugln(loadSelect);
        if (!utilState) {
          recallSequenceFromEEPROM(loadSelect, y);
        } else {
          recallSequenceFromEEPROM(loadSelect, ALLTRACKS);
        }
        break;
      case 3:
      case 2:
      case 1:
      case 0:
        saveSlotSelect = x;  // Assign values within the cases
        trackSelect = y;
        if (utilState) {
          trackSelect = 8;
        }
        debug("save number ");
        debugln(saveSlotSelect);
        if (utilState) {
          saveCurrentSequenceToEEPROM(saveSlotSelect, ALLTRACKS);  //save instrument "y" to slot "x" (saveselect)
        } else {
          saveCurrentSequenceToEEPROM(saveSlotSelect, trackSelect);  //save instrument "y" to slot "x" (saveselect)
        }

        break;
      default:
        // Handle other cases when shift is true
        break;
    }
  } else {  //if shift isnt held in
    switch (x) {
      case 15:  // select column
        currentInst = y;
        mode = instSeqMode;
        break;
      case 14:  // mute column
        toggleMute(y);
        debug("muted ");
        debugln(y);
        break;
      case 13:  // solo column
        toggleSolo(y);
        debug("soloed ");
        debugln(y);
        break;
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if (y == 0) {      // if top row page select buttons
          currentSeq = x;  // Seq 1 2 3 or 4
          numberToDisplay = x + 1;
          textDisplayStartTime = millis();
        }
        displayNumber(x, 4, 3);


      default:
        debug("x = ");
        debug(x);
        debug(" and y = ");
        debugln(y);
        break;
    }
  }
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
          handleButtPress(offsetX, y);
          buttStates2D[offsetX][y] = true;
        }
      } else {
        if (buttStates2D[offsetX][y]) {  // if it was previously pushed in
          handleButtRelease(offsetX, y);
          buttStates2D[offsetX][y] = false;
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
      debugln("butt pressed");
    } else {
      debugln("butt released");
    }
  }

  playButtonState = !digitalRead(PLAYPAUSEPIN);
  if (playButtonState != oldPlayButtonState) {
    if (playButtonState) {  // when button goes from off to on
      //check if we are under midi clock:
      if(!midiClockRunning){
        playing = !playing;
        if (!playing) {
          currentStep = -1;
        } else if (playing) {
          lastStepTime = millis();
          currentStep = 0;
          handleStep(currentStep);
        }
      } else {
        if(shiftState){
          currentStep = -1;
          midiClockCounter = 5;
        }
      }
    }
    oldPlayButtonState = playButtonState;
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

long tempoToMicros(int bpm) {
  return (unsigned long)round(60000000.0 / bpm);
}

void handleRotaryPush() {  //rotaryclick
  swState = digitalRead(swPin);
  if (swState != swLastState) {
    if (millis() > lastSwTime + swDebounceTime) {
      debugln(swState);
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
  debug("rotary = ");
  debug(rotaryMasterCounter);
  debug(" ");
  for (int i = 0; i < rotaryMasterCounter; i++) {
    debug("#");
  }
  debugln();
}

void rotaryMove(int moveAmount) {
  if (mode == overviewMode && utilState) {
    tempo = tempo + moveAmount * 10;
  }

  if (shiftState) {
    tempo = tempo + moveAmount;
    debug("tempo ");
    debugln(tempo);
  } else {
    viewOffset += moveAmount;
    if (viewOffset > maxViewOffset) viewOffset = maxViewOffset;
    if (viewOffset < 0) viewOffset = 0;
  }

  if (tempo > 999) {
    tempo = 999;
  } else if (tempo < 0) {
    tempo = 0;
  }
}