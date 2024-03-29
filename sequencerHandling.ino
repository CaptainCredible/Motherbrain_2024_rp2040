
uint64_t seqMatrix[SEQUENCES][INSTRUMENTS][GRIDSTEPS] = {  // declare 8 16X8 sequences, each row = an instrument, the bits in the number = the notes
  {
    // seq 1 2
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0 },  // inst 1
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },  // inst 2
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },  // inst 3
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },  // inst 4
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },  // inst 5
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },  // inst 6
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },  // inst 7
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 }   // inst 8
  },
  // seq 3 4
  { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 } },
  // seq 5 6
  { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 } },
  // seq 7 8
  { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 } },
  //seq 9 10
  { { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0 } },
  //seq 11 12
  { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 } },
  //seq 13 14
  { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 } },
  //seq 15 16
  { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 } }
};

// for randomization
uint16_t kickPattens[8] = { 0b1000000010000000, 0b1000000000000000, 0b1001001010001000, 0b1001001001001000, 0b1000100110100100, 0b1010000110100000, 0b1000000010001010, 0b1000000010000100 };
uint16_t snarePatterns[8] = { 0b0000100000001000, 0b0000100101001000, 0b0010010000100100, 0b0010001000100010, 0b0000001000001010, 0b0010001000100001, 0b0000000010000000, 0b0000100001001000 };
uint16_t hatPatterns[8] = { 0b1000100010001000, 0b0000100000001000, 0b0010010000100100, 0b1010101010101010, 0b1111111111111111, 0b0100100100100100, 0b0001001001001001, 0b1001001001001010 };
void randomize(byte whotSeq, byte whotInst) {
  if (whotInst == 0) {  // it its a drum track
    // use pattern arrays to populate tracks TODO
  }
}


//TOTAL 4096 bytes of EEPROM.
//32 bits per step per instrument
//32bits * 16steps * 8 instruments = 4096 bits (not bytes)
//that means 4bytes per step per instrument
//4bytes * 16steps * 8 instruments = 512bytes per page (a.k.a. sequence)
//this times 4 pages = 2048
//bump it up to 64bits = 8bytes * 16steps * 8 instruments = 1024bytes per page (a.k.a. sequence) * 4 pages = 4096, this means no room for flags. just read EEPROM anyway?
// WE NEED BIGGER STORAGE FOR 64 notes / bits per step!
// if we go all the way up to 128, we need 16bytes * 16 steps * 8 instruments = 2048 bytes per page, * 4 pages = 8192. i would also like to set some flags and other settings so i guess 16kB
// if i was to rewrite to describe individual 7bit notes i would get (128/7 = 18 note polyphony per step per inst...) hmm. or 9 note polyphony if i use 64bits pet step.

// or i could use 1 byte per step to describe step number (last bit s not needed to store note number up to 127 so last bit can be to signify step) then only store the notes i need, giving me an average of like 6-7 notes per step ? doesnt quite align with bytes so it would get complicated
// i woud need to describe "end of data" with all ones perhaps 

void saveCurrentSequenceToEEPROM(int slot, int trackToSave) {  // 8 saves all tracks
  if (slot >= 0 && slot < 4) {
    EEPROM.begin(EEPROM_SIZE);
    uint32_t address = slot * (SEQUENCE_SIZE + 1);  // +1 for the flag
    //EEPROM.write(address, SEQUENCE_FLAG);           // Write the flag
    address++;
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 16; j++) {
        if (trackToSave == i || trackToSave == ALLTRACKS) {
          uint32_t truncatedStep = seqMatrix[currentSeq][i][j];
          EEPROM.put(address, truncatedStep);
        }
        address += sizeof(uint32_t);
      }
    }
    EEPROM.commit();
    EEPROM.end();
    //debug("saved instrument ");
    if (trackToSave == ALLTRACKS) {
      //debug("ALL TRACKS");
    } else {
      //debug(trackToSave);
    }
    //debug(" to slot ");
    //debugln(slot);
  } else {
    // Handle error: Invalid slot number
    //debugln("invalid slot number");
  }
}

void recallSequenceFromEEPROM(int slot, int trackToLoad) {  // 8 LOADS ALL TRACKS
  if (slot >= 0 && slot < 4) {                              // make sure we arent trying to load outside of memory
  EEPROM.begin(EEPROM_SIZE);
    uint32_t address = slot * (SEQUENCE_SIZE + 1);          // +1 for the flag
    //seqMatrix[SEQUENCES][INSTRUMENTS][GRIDSTEPS]
    //if (EEPROM.read(address) == SEQUENCE_FLAG) {  // Check the flag
    if (true) {  // ignore the flag
      address++;
      for (int i = 0; i < 8; i++) {     //tracks
        for (int j = 0; j < 16; j++) {  //steps
          uint32_t value;
          if (trackToLoad == i || trackToLoad == ALLTRACKS) {
            EEPROM.get(address, value);
            seqMatrix[currentSeq][i][j] = value;
          }

          address += sizeof(uint32_t);
          //debug("recalled address ");
          //debugln(address);
        }
      }
      EEPROM.end();
      //debug("loaded instrument ");
      if (trackToLoad == 8) {
        //debug("ALL TRACKS");
      } else {
        //debug(trackToLoad);
      }
      //debug(" from slot ");
      //debugln(slot);
    } else {
      //debugln("wrong flag");
      // Handle error: Invalid sequence flag, sequence not storedf
    }
  } else {
    //debugln("invalid slot number");
    // Handle error: Invalid slot number
  }
}

// to begin with, just read seqMatrix[0][0][0-15]

bool getNote(byte sequence, byte instrument, byte step, byte note) {
  uint64_t thisStep = seqMatrix[sequence][instrument][step];
  return (thisStep & (1ULL << note)) != 0;  // Check if the specific bit is set and return the result
}

// inverted drawing Y axis
void drawStepState(uint8_t sequence, uint8_t instrument, uint8_t step) {
  uint64_t thisStep = seqMatrix[sequence][instrument][step];
  for (uint8_t i = 0; i < 8; i++) {                   // iterate over 8 of the 32 potential bits in the uint32_t
    bool thisBit = (thisStep >> i + viewOffset) & 1;  // check if the i-th bit is set
    if (thisBit) {
      setPixelXY(step, (GRIDROWS - 1) - i, currentInstCol[0], currentInstCol[1], currentInstCol[2]);  // assume i is the Y-coordinate here
    } else {
    }
  }
}

unsigned long lastBlink = 0;
int blinkDuration = 300;
bool blinkState = false;
void displayPageNoBlink() {
  if (millis() - lastBlink > blinkDuration) {
    blinkState = !blinkState;
    lastBlink = millis();
  }
  if (blinkState) {
    setPixelXY(currentSeq, 0, 100, 0, 0);  // HERE!!!!
  }
}

byte overviewColor[3] = { 100, 70, 50 };
byte mutedColor[3] = { 40, 40, 40 };

void drawSeqOverview(uint8_t currentSeq) {
  int fadedAmount = 0;  // doesnt work very well
  for (uint8_t selectInst = 0; selectInst < SEQUENCES; selectInst++) {
    if (bitRead(mutes, selectInst)) {  // if this track is muted
      fadedAmount = 4;
    } else {
      fadedAmount = 0;
    }
    int thisTrackRed = trackColors[selectInst][0] >> fadedAmount;
    if (thisTrackRed < 5) thisTrackRed = 5;
    int thisTrackGreen = trackColors[selectInst][1] >> fadedAmount;
    if (thisTrackGreen < 5) thisTrackGreen = 5;
    int thisTrackBlue = trackColors[selectInst][2] >> fadedAmount;
    if (thisTrackBlue < 5) thisTrackBlue = 5;
    for (uint8_t step = 0; step < GRIDSTEPS; step++) {
      if (seqMatrix[currentSeq][selectInst][step] > 0) {

        setPixelXY(step, selectInst, overviewColor[0], overviewColor[1], overviewColor[2]);  // assume i is the Y-coordinate here
      }
      if (step == 15) {
        setPixelXY(step, selectInst, thisTrackRed, thisTrackGreen, thisTrackBlue);  // assume i is the Y-coordinate here
      }
    }
  }
}

void setNote(uint8_t sequence, uint8_t instrument, uint8_t step, uint8_t note) {
  if (note > 63) return;                             // Check if the note value is valid (0 to 31 because uint32_t has 32 bits)
  uint64_t bitmask = 1UL << note;                    // Create a bitmask for the note
  seqMatrix[sequence][instrument][step] |= bitmask;  // Set the bit in the seqMatrix for the specified sequence, instrument, and step
}

void clearNote(uint8_t sequence, uint8_t instrument, uint8_t step, uint8_t note) {
  if (note > 63) return;                             // Check if the note value is valid (0 to 31 because uint32_t has 32 bits)
  uint64_t bitmask = ~(1UL << note);                 // Create a bitmask for the note
  seqMatrix[sequence][instrument][step] &= bitmask;  // Clear the bit in the seqMatrix for the specified sequence, instrument, and step
}

void clearTrack(byte sequenceToClear, byte trackToClear) {
  if (trackToClear == ALLTRACKS) {
    for (byte j = 0; j < INSTRUMENTS; j++) {
      for (byte i = 0; i < GRIDSTEPS; i++) {
        seqMatrix[sequenceToClear][j][i] = 0;
      }
    }
  } else {

    for (byte i = 0; i < GRIDSTEPS; i++) {
      seqMatrix[sequenceToClear][trackToClear][i] = 0;
    }
  }
}

void clearInst(byte whotSeq, byte whotInst) {
  if (whotInst == ALLTRACKS) {
    for (byte i = 0; i < 8; i++) {
      for (byte j = 0; j < GRIDSTEPS; j++) {
        seqMatrix[whotSeq][i][j] = 0;
      }
    }
  }
  for (byte i = 0; i < GRIDSTEPS; i++) {
    seqMatrix[whotSeq][whotInst][i] = 0;
  }
}

//pølse



void toggleNote(uint8_t sequence, uint8_t instrument, uint8_t step, uint8_t note) {
  if (note > 63) return;                             // Check if the note value is valid (0 to 63 because uint64_t has 64 bits)
  uint64_t bitmask = 1ULL << note;                    // Create a bitmask for the note
  seqMatrix[sequence][instrument][step] ^= bitmask;  // Toggle the bit in the seqMatrix for the specified sequence, instrument, and step
}


struct TimedNote {
  byte note;
  unsigned long startTime;
  unsigned long duration;
  byte channel;
  byte port;
};


#define HWMIDI_PORT 1
#define USBMIDI_PORT 2
const int maxTimedNotes = 100;
TimedNote timedNotes[maxTimedNotes];
int currentTimedNote = 0;
int numActiveNotes = 0;

void addTimedNote(byte note, unsigned long duration, byte midiChannel) {
  timedNotes[currentTimedNote].note = note;
  timedNotes[currentTimedNote].startTime = millis();
  timedNotes[currentTimedNote].duration = duration;
  timedNotes[currentTimedNote].channel = midiChannel;
  //timedNotes[currentTimedNote].port = outPort;
  currentTimedNote = (currentTimedNote + 1) % maxTimedNotes;  // Circular buffer
  numActiveNotes++;
}

void checkAndHandleTimedNotes() {
  for (int i = 0; i < maxTimedNotes; i++) {
    if (timedNotes[i].note != 0 && (millis() - timedNotes[i].startTime) > timedNotes[i].duration) {
      HWMIDI.sendNoteOff(timedNotes[i].note, 127, timedNotes[i].channel);
      if(!midiClockRunning){
        USBMIDI.sendNoteOff(timedNotes[i].note, 127, timedNotes[i].channel);
      }
      timedNotes[i].note = 0;  // Reset the note to indicate it's handled
      numActiveNotes--;
    }
  }
}






// HANDLE STEP //

void handleStep(byte stepToHandle) {
  bool tracksBufferIsEmpty = true;
  // handle notes THIS ONLY ACTUALLY SCANS THE CURRENTLY VIEWED INSTRUMENT!!!
  byte maxNotes = stepDataSize;  //our datastructure actually allows 64bit steps but microbitOrchestra currently only likes 16bit
  for (byte currentTrack = 0; currentTrack < 8; currentTrack++) {
    if (!bitRead(mutes, currentTrack)) {  // if this track is not muted
      tracksBuffer16x8[currentTrack] = seqMatrix[currentSeq][currentTrack][currentStep];

      if (tracksBuffer16x8[currentTrack != 0]) {
        tracksBufferIsEmpty = false;  // keep track of whether the tracksbuffer has any data that needs to be sent.
      }

      //MIDI OUTPUT AND SPARKLES:
      bool alreadyTriggeredSparkleForThisTrack = false;
      for (byte i = 0; i < maxNotes; i++) {
        byte actualNote = i;
        if (getNote(currentSeq, currentTrack, currentStep, i)) {  //we found a note

          triggerMidiNote(actualNote, currentTrack);  //add one because midichannels start with 1
          //need added logic here to only make sparkles fot the track we are viewing
          if (mode == overviewMode) {
            if (!alreadyTriggeredSparkleForThisTrack) {
              currentInstCol[0] = trackColors[currentTrack][0];
              currentInstCol[1] = trackColors[currentTrack][1];
              currentInstCol[2] = trackColors[currentTrack][2];
              //addSparkle(currentStep, currentTrack, currentInstCol[0], currentInstCol[1], currentInstCol[2], sparkleLifespan*4);  // make that pixel sparkle for 500ms, also invert Y axis
              addSporkle(currentStep, currentTrack, currentInstCol[0], currentInstCol[1], currentInstCol[2], overviewColor[0], overviewColor[1], overviewColor[2], sparkleLifespan * 2);  // make that pixel sparkle for 500ms, also invert Y axis
              addSporkle(15, currentTrack,255,255,255,currentInstCol[0], currentInstCol[1], currentInstCol[2], sparkleLifespan);
              alreadyTriggeredSparkleForThisTrack = true;
            }
          } else {
            if ((i >= viewOffset && i <= viewOffset + 7) && (currentTrack == currentInst)) {                                                                        // if the triggered note is within the view and we are viewing the track that is playing the note
              addSporkle(currentStep, (GRIDROWS - 1) - (i - viewOffset), 200, 200, 200, currentInstCol[0], currentInstCol[1], currentInstCol[2], sparkleLifespan);  // make that pixel sparkle for 500ms, also invert Y axis
            }
          }
        }
      }
    }
  }

  tracksBuffer16x8[8] = currentStep;  //slot number eight is where we send the current step number
  tracksBuffer16x8[9] = mutes;   //slot 9 is where the mutes are stored
  sendTracksBuffer();
}

void triggerMidiNote(byte noteToSend, byte channelToSend) {
  if (numActiveNotes < maxTimedNotes) {
    byte midiNote = noteToSend + 60;
    HWMIDI.sendNoteOn(midiNote, 127, channelToSend+1);
    if(!midiClockRunning){   // this keeps us from sending midi if we are busy receiving midi. 
      USBMIDI.sendNoteOn(midiNote, 126, channelToSend+1); // TRY COMMENTING OUT THIS!!!  
    }
    addTimedNote(midiNote, 50, channelToSend);  // Assuming 50ms is the duration for each note ALSO TRY TO REMOVE THIS AS TEST???
  }
}



void radioSendClockTick() {
  //sendWire2microBitTrackAndNote(101, 0);			//send that note to microbit (ask microbit to request it.
}


void saveCurrentSequenceToFile(int slot, int trackToSave) {
  if (slot >= 0 && slot < 8) {
    LittleFS.begin();
    String filename = "/seq" + String(slot) + ".dat";
    // Open the file in read+write mode. Create it if it doesn't exist.
    File file = LittleFS.open(filename, "r+");
    if (!file) {
      // If file does not exist, create it by opening in write mode.
      file = LittleFS.open(filename, "w");
    }
    if (file) {
      for (int i = 0; i < 8; i++) {
        if (trackToSave == i || trackToSave == ALLTRACKS) {
          // Calculate the position in the file where this track's data starts
          file.seek(i * 16 * sizeof(uint64_t), SeekSet);
          for (int j = 0; j < 16; j++) {
            uint64_t truncatedStep = seqMatrix[currentSeq][i][j];
            file.write((byte*)&truncatedStep, sizeof(truncatedStep));
          }
        }
      }
      file.close();
    } else {
      debugln("File open failed");
    }
    LittleFS.end();
  } else {
    debugln("Invalid slot number");
  }
}


void recallSequenceFromFile(int slot, int trackToLoad) {
  if (slot >= 0 && slot < 8) { // Ensure valid slot number
    LittleFS.begin();
    String filename = "/seq" + String(slot) + ".dat"; // Construct filename for slot
    File file = LittleFS.open(filename, "r"); // Open file for reading
    
    if (file) {
      // Calculate the start address for the trackToLoad in the file
      // This skips directly to the data for the specified track
      for (int i = 0; i < 8; i++) { // Iterate over tracks
        if (trackToLoad == i || trackToLoad == ALLTRACKS) {
          file.seek(i * 16 * sizeof(uint64_t), SeekSet); // Seek to the start of the track in the file
          for (int j = 0; j < 16; j++) { // Iterate over steps
            uint64_t value;
            if(file.read((byte*)&value, sizeof(value)) == sizeof(value)) { // Read each step
              seqMatrix[currentSeq][i][j] = value; // Assign value to matrix
            }
          }
        }
      }
      file.close(); // Close the file
      debug("loaded instrument ");
      if (trackToLoad == ALLTRACKS) {
        debug("ALL TRACKS");
      } else {
        debug(trackToLoad);
      }
      debug(" from slot ");
      debugln(slot);
    } else {
      debugln("File open failed");
    }
    LittleFS.end();
  } else {
    debugln("Invalid slot number");
  }
}
