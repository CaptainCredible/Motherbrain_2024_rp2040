



//USES Francois Best's MIDI library
//USES Earle Philhower's  RP2040 Core
//  TinyUSB stack, FLASH: 2MB (512KB FS)
//USES FastLed library



/*

    IT CRASHES IF IT SENDS AND RECEIVES OVER USB.
    IF IT ONLY SENDS, IT'S FINE.
    IF IT ONLY RECEIVES, IT'S FINE

*/

/*

things to add:

  - chain sequences (add own chain page ?) SONG MODE. Logic for songmode all tracks vs song mode 1 track
  - chain transposes
  - change MIDI clock input subdivision
  - Randomize
  - LASTSTEP
  
functions i need to be able to toggle on overview page:
  - select
  - mute
  - solo
  - transpose
  - shift left/right
  - save
  - clear
  - generate // need scales arrays, currentScale selector , root note selector, drum / tones selector 
  - slot 12345678
  - settings
  - song
  - lastStep
  

functions i need to be able to toggle on setup page:
  - midi out channel per track
  - USB clock out
  - HW MIDI clock out
  - 
*/


int debugNum = 0;  // Variable to store a number that will br displaye on the scrim

#define FASTLED_RP2040_CLOCKLESS_PIO true  //gives me error: #if with no expression 7 | #if FASTLED_RP2040_CLOCKLESS_PIO
#define DEBUG_ENABLED true

#if DEBUG_ENABLED
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)

#else
#define debug(x)
#define debugln(x)
#endif

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <FastLED.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LittleFS.h>

#define overviewMode 0
#define instSeqMode 1
#define lastStepMode 2
#define globalTransposeMode 11
#define ALLTRACKS 8



int sparkleLifespan = 200;
volatile uint16_t tracksBuffer16x8[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };      //tracks 0 - 8 then currentstep then mutes
volatile uint16_t midiTracksBuffer16x8[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  //last one used for currentStep, so receivers need to be able to determine that we are not setting step number!
bool isMuted[8] = { false, false, false, false, false, false, false, false };
bool isPoly[8] = { true, true, true, true, true, true, true, true };

bool rotaryPushState = false;

//trackColours:
byte trackColors[8][3] = {
  { 118, 60, 60 },   // Violet faded
  { 75, 0, 130 },    // Indigo
  { 0, 0, 255 },     // Blue
  { 0, 255, 255 },   // Cyan
  { 0, 128, 0 },     // Green
  { 255, 255, 30 },  // Vivid Yellow
  { 255, 100, 0 },   // Darker Orange
  { 255, 0, 0 }      // Red
};

byte fadedTrackColors[8][3] = {
  { 59, 30, 30 },    // Violet faded faded
  { 37, 0, 65 },     // Indigo
  { 0, 0, 128 },     // Blue
  { 0, 128, 128 },   // Cyan
  { 0, 64, 0 },      // Green
  { 128, 128, 15 },  // Vivid Yellow
  { 128, 50, 0 },    // Darker Orange
  { 128, 0, 0 }      // Red
};


int counter24ppqn = -1;

int tempo = 120;
int oldTempo = 120;

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;


//sequencer related
bool transposeActivated[8] = {true, false, false, false, true, false, false, true};

int globalTransposeSeq[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

byte stepDataSize = 64;
byte solos = 0b00000000;
uint16_t mutes = 0b00000000;
volatile int currentStep = -1;
int currentBar = -1; // where are we in the song
uint8_t songLength = 16;
byte currentSeq = 0;
byte currentSeqs[8] = {0,0,0,0,0,0,0,0};
byte currentInst = 0;
bool playing = false;
unsigned int tempoMillis = 100;
unsigned long tempoMicros = 100000;
unsigned long tempoMicros24 = 4167;
uint16_t numberToDisplay = 0;
int viewOffset = 0;
byte maxViewOffset = stepDataSize - 9;
bool transposeState = false; // flag to see if insta-transpose button is held in
byte transposeTrack = 0; //variable to hold the track number to insta-transpose
byte lastStep = 16;

void increaseBar(){
  currentBar++;
  if(currentBar >= songLength){currentBar = 0;};
}

#define DRUMSEQ 1
#define CHORDSEQ 2
#define MELODYSEQ 3
#define BASSSEQ 4

byte scaleSelect = 0;
#define MAJ 0
#define MIN 1
#define PEN 2
#define BLU 3
#define ORI 4

const char* scaleNames[5] = { "MAJ", "MIN", "PEN", "BLU", "ORI" };

byte scales[5][8] = {
  { 0, 2, 4, 5, 7, 9, 11, 12 },   // Major scale
  { 0, 2, 3, 5, 7, 8, 10, 12 },   // Natural Minor scale
  { 0, 2, 4, 7, 9, 12, 12, 12 },  // Pentatonic Major scale (repeating the octave to fill 8 notes)
  { 0, 3, 5, 6, 7, 10, 11, 12 },  // Blues scale
  { 0, 1, 4, 5, 7, 8, 11, 12 }    // Oriental scale
};

byte lastSteps[8][8]{ //track, page/seq
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 },
                      { 15, 15, 15, 15, 15, 15, 15, 15 }
};

byte songLengths[8]{ 1, 1, 1, 1, 1, 1, 1, 1 };

byte songs[8][16]{ //track, songStep
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

bool triplets = false;


byte rootNote = 0;
byte currentScale = 1;

//led related
byte cycle = 0;
byte testColor = 255;
int textDisplayTimeout = 500;     //500 ms from text is displayed until it dissappears
int textDisplayStartTime = -500;  //to store the time the text started displaying
byte currentInstCol[3] = { 0, 255, 0 };

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, USBMIDI);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, HWMIDI);

//grid
#define DATA_PIN 2    // neopixel pin
#define NUM_LEDS 128  // 16x8 grid
#define GRIDROWS 8
#define GRIDSTEPS 16
#define INSTRUMENTS 8
#define SEQUENCES 8

//rotary encoder
const int clkPin = 6;  // CLK pin of the encoder
const int dtPin = 7;   // DT pin of the encoder
const int swPin = 3;
int rotaryMasterCounter = 0;  // Keeps track of the encoder position
int clkLastState = 0;         // Stores the last state of the CLK pin
int swLastState = 0;
unsigned long lastClkTime = 0;       // the last time the output pin was toggled
unsigned long clkDebounceDelay = 4;  // the debounce time; increase if the output flickers //was 1
unsigned long lastSwTime = 0;
unsigned long swDebounceTime = 5;
int swState = 0;

//i2c
const byte interruptPin = 22;  //pin to interrupt uBit

//buttons
byte playPausePin = 10;
byte utilPin = 9;
byte shiftPin = 8;

//byte potPin = A0;
CRGB leds[NUM_LEDS];
bool playButtonState = false;
bool utilState = false;
bool shiftState = false;
bool oldPlayButtonState = false;
bool oldUtilState = false;
bool oldShiftState = false;
bool rotarySwitchState = false;
uint16_t knobVal = 0;


//byte byteButtStates2D[GRIDSTEPS][GRIDROWS];
bool buttStates2D[GRIDSTEPS][GRIDROWS];
bool oldButtStates2D[GRIDSTEPS][GRIDROWS];
bool ledGridState2D[GRIDSTEPS][GRIDROWS];
CHSV ledColState2D[GRIDSTEPS][GRIDROWS];


//ledGrid
int textPosX = 0;
unsigned long lastShow = 0;
char scrollText[256] = "";  // Assuming max text length of 255 characters + null terminator
unsigned long scrollTimer = 0;
int scrollSpeed = 7;  // The speed of scrolling, higher is slower
bool isScrolling = false;



unsigned long debugTimer;
void debugTimerStart() {
  debugTimer = micros();
}

void debugTimerEnd() {
  debugTimer = micros() - debugTimer;
  debugln(debugTimer);
}

byte SHIFTPIN = 8;
byte UTILPIN = 9;
byte PLAYPAUSEPIN = 10;

volatile int rotaryVal = 0;
byte lastEncoded = 0;

//#define EEPROM_SIZE 4096  //maximum
#define EEPROM_SIZE 8192  //maximum
#define SEQUENCE_SIZE 512
#define SEQUENCE_FLAG 123

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //FAKE EEPROM IN FLASH
  EEPROM.begin(EEPROM_SIZE);

  //wire
  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin(8);  //address = 8
  Wire1.onRequest(requestEvent);

  pinMode(SHIFTPIN, INPUT_PULLUP);
  pinMode(PLAYPAUSEPIN, INPUT_PULLUP);
  pinMode(UTILPIN, INPUT_PULLUP);

  //rotary
  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  pinMode(swPin, INPUT_PULLUP);
  // Read the initial state of clkPin, swPin
  clkLastState = digitalRead(clkPin);
  swLastState = digitalRead(swPin);
  attachInterrupt(digitalPinToInterrupt(clkPin), handleRotary, CHANGE);  // Attach the interrupts

  //interrup pin to microbit
  pinMode(interruptPin, OUTPUT);  // to interrupt microbit
  digitalWrite(interruptPin, LOW);

//MIDI
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  TinyUSB_Device_Init(0);
#endif
  //usb_midi.setStringDescriptor("mthr");
  Serial1.setRX(1);
  Serial1.setTX(0);
  HWMIDI.begin(MIDI_CHANNEL_OMNI);
  HWMIDI.turnThruOff();

  USBMIDI.begin(MIDI_CHANNEL_OMNI);
  USBMIDI.turnThruOff();
  USBMIDI.setHandleClock(handleUsbMidiClockTicks);
  USBMIDI.setHandleNoteOn(HandleUsbNoteOn);
  USBMIDI.setHandleNoteOff(HandleUsbNoteOff);

  //GRID
  setupGridPins();  //setup pins for buttonGrid
  //pixels.begin();   // INITIALIZE NeoPixel strip object (REQUIRED)
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // INITIALIZE NeoPixel strip object (REQUIRED)
  initLedGridState();
  //FastLED.show();
  //delay(1000); // needed?
  testSetXY(100);
  testColor = 255;                     // green
  unsigned long startTime = millis();  // Capture start time
  while (!TinyUSBDevice.mounted()) {
    delay(1);                           // Wait for 1 millisecond
    if (millis() - startTime > 1000) {  // Check if 1000ms have passed
      testColor = 0;
      break;  // Exit the loop if more than 1000ms have passed
    }
  }
  testSetXY(testColor);
  currentSeq = 0;
  recallSequenceFromFile(0, ALLTRACKS);
  currentSeq = 1;
  recallSequenceFromFile(1, ALLTRACKS);
  currentSeq = 2;
  recallSequenceFromFile(2, ALLTRACKS);
  currentSeq = 3;
  recallSequenceFromFile(3, ALLTRACKS);
  currentSeq = 4;
  recallSequenceFromFile(4, ALLTRACKS);
  currentSeq = 5;
  recallSequenceFromFile(5, ALLTRACKS);
  currentSeq = 6;
  recallSequenceFromFile(6, ALLTRACKS);
  currentSeq = 7;
  recallSequenceFromFile(7, ALLTRACKS);
  currentSeq = 0;
  if (testColor == 0) {
    setMessage("STA");
    scrollSpeed = 6;
  } else {
    setMessage("USB");
    scrollSpeed = 6;
  }
  tempoMicros24 = tempoToMicros24(tempo);
}

bool runClock = true;                         //run internal clock
bool midiClockRunning = false;                //the MIDI clock is running, this is unnessecary, tghe runClock variable should mirror this
unsigned long lastMidiClockReceivedTime = 0;  //how long since last time we received a midi clock
int midiClockDiv = 6;
byte midiClockCounter = 5;

void handleUsbMidiClockTicks() {
  playing = false;
  lastMidiClockReceivedTime = millis();
  midiClockRunning = true;
  midiClockCounter++;
  midiClockCounter = midiClockCounter % midiClockDiv;
  if (midiClockCounter == 0) {
    midiClockStep();
  }
}


byte fastLEDUpdateCounter = 0;
byte mode = overviewMode;
void loop() {  // the whole loop uses max 1040us, idles at 400 for cycles without FastLED.show(), and 500 for cycles with FastLED.show()
  FastLED.clear();
  if (millis() - lastMidiClockReceivedTime > 1000) {  // is it more htan 1000ms since midi clock
    midiClockRunning = false;
  }

  handleRotaryPush();
  if (tempo != oldTempo) {
    //tempoMillis = tempoToMillis(tempo);
    tempoMicros24 = tempoToMicros24(tempo);
    numberToDisplay = tempo;
    textDisplayStartTime = millis();
    oldTempo = tempo;
  }

  if (fastLEDUpdateCounter == 8) {
    scanGrid();  //scan the grid TAKES 335 microseconds
  }

  scanButtsNKnobs();  // takes 2 microseconds

  switch (mode) {
    case instSeqMode:
      handleInstSeqMode();  //not responsible for crashes 40 - 150 us
      break;
    case overviewMode:
      handleOverviewMode();  //not responsible for crashes 100 - 150 us
      break;
    case lastStepMode:
      handleLastStepMode();
      break;
    case globalTransposeMode:
      handleGlobalTransposeMode();
      break;
  }

  updateSparkles();  // not responsible, 340us

  if (millis() < textDisplayStartTime + textDisplayTimeout) {
    displayNumber(numberToDisplay, 4, 3);
  }

  checkStepTimer();  // will also draw cursor 2us

  checkAndHandleTimedNotes();  // usually 8us occasionally as high as 117us //check this in detail

  handleI2CTimeout();  //2us

  checkUSBMidiTimout();  // 2us idle, 20us with many notes

  //MIDI.read();

  while (USBMIDI.read()) {  //2us idle, 50 - 150 for a note, 33 - 60 for a midi clock
  }

  updateScroll();

  fastLEDUpdateCounter++;
  fastLEDUpdateCounter = fastLEDUpdateCounter % 16;  // this MASSIVELY inproves performance, %16 gives me approx 140FPS
  if (fastLEDUpdateCounter == 0) {
    if (rotaryPushState) {
      noise();
    }
    FastLED.show();  //175us
  }
}


void handleLastStepMode(){
  displayText("LS", 1, 1,false);
  drawCursor(currentStep);
  drawRainbowStick();
}

unsigned long lastStepTime = 0;
void handleOverviewMode() {
  drawCursor(currentStep);
  drawSeqOverview(currentSeq);
  drawCurrentBar();
  for (int row = 0; row < GRIDSTEPS; row++) {
    for (int col = 0; col < GRIDROWS; col++) {
      if (buttStates2D[row][col] == true) {
        crossHair(row, col);
      }
    }
  }
  displayPageNoBlink();  // 3us
}

void handleGlobalTransposeMode(){
  currentInstCol[0] = 200;
  currentInstCol[1] = 200;
  currentInstCol[2] = 200;
  drawHelpers();
  drawPianoRoll();
  drawZero();
  drawTransposeSeq();
  drawSongLength();
  drawCurrentBar();
  displayPageNoBlink();  // 3us
  drawCursor(currentStep);
}

void handleInstSeqMode() {
  drawHelpers();
  drawPianoRoll();
  drawCursor(currentStep);

  for (int column = 0; column < GRIDSTEPS; column++) {
    // read the R G B of the colour for this track;
    currentInstCol[0] = trackColors[currentInst][0];
    currentInstCol[1] = trackColors[currentInst][1];
    currentInstCol[2] = trackColors[currentInst][2];

    drawStepState(currentSeq, currentInst, column);
    for (int row = 0; row < GRIDROWS; row++) {
      if (buttStates2D[column][row] == true) {
        crossHair(column, row);
      }
    }
  }
  displayPageNoBlink();  // 3us
}

int lastHandledStep = -2;
void checkStepTimer() {
  unsigned long nowTime = micros();
  if (nowTime >= lastStepTime + tempoMicros24) {            // if we reached the end of the step
    unsigned long goalTime = lastStepTime + tempoMicros24;  // the time we were aiming for
    int diff = nowTime - goalTime;                          //how far did we overshoot?
    lastStepTime = micros() - diff;                         //compensate for overshoot
    if (playing) {
      sendClockTick();
      counter24ppqn++;
      counter24ppqn = counter24ppqn % 6;
      if (counter24ppqn == 0) {
        currentStep = (currentStep + 1) % lastStep;
      }
    }
  }

  if (currentStep != lastHandledStep) {
    if(currentStep == 0){
      increaseBar();
    }
    if (currentStep >= 0) {
      handleStep(currentStep);
    }
    lastHandledStep = currentStep;
    
  }
}


void sendClockTick() {
  HWMIDI.sendClock();
}

template<typename T>
T setBit(T input, byte bit, bool state) {
  // Calculate the number of bits in the input type
  int numBits = sizeof(T) * 8;

  if (bit >= 0 && bit < numBits) {
    if (state) {
      // Set the bit
      input |= (T(1) << bit);
    } else {
      // Clear the bit
      input &= ~(T(1) << bit);
    }
  }

  return input;
}

//template for toggling bit in arbitrary sized int
template<typename T>
T toggleBit(T input, byte bit) {
  // Calculate the number of bits in the input type
  int numBits = sizeof(T) * 8;
  if (bit >= 0 && bit < numBits) {
    // Toggle the bit
    input ^= (T(1) << bit);
  }
  return input;
}

// Template function for reading a bit from an arbitrary-sized integer
template<typename T>
bool readBit(T input, byte bit) {
  // Calculate the number of bits in the input type
  int numBits = sizeof(T) * 8;

  if (bit >= 0 && bit < numBits) {
    return (input & (T(1) << bit)) != 0;
  } else {
    return false;  // Bit position is out of range
  }
}
