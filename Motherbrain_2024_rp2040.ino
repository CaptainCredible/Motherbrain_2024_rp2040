/*TODO:
  DONE make internal clock only work when its been a while since midi clock
  
  DONE mute button
  solo button is maybe dumb...?

  set up load save buttons correctly (shift + slot for load, shift + save + slot for save)
  sequencer to i2c
  midi notes to i2c
  
  randomize
*/

//list of things tha neefd to change for 64bit version:



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

#define overviewMode 0
#define instSeqMode 1
#define ALLTRACKS 8
bool waitingForTimeOut = false; // are we waiting for more midi notes to come in? 
int sparkleLifespan = 200;
unsigned int tracksBuffer16x8[10] = { 0,0,0,0,0,0,0,0,0,0 }; //tracks 0 - 8 then currentstep then mutes
unsigned int midiTracksBuffer16x8[10] = { 0,0,0,0,0,0,0,0,0,0 }; //last one used for currentStep, so receivers need to be able to determine that we are not setting step number!
uint8_t isMutedByte = 0b00000000;
bool isMuted[8] = { false, false,false, false,false, false,false, false };
bool isPoly[8] = {true, true, true, true, true, true, true, true};

//trackColours:
byte trackColors[8][3] = {
  { 255, 0, 0 },     // Red
  { 255, 100, 0 },   // Darker Orange
  { 255, 255, 30 },  // Vivid Yellow
  { 0, 128, 0 },     // Green
  { 0, 255, 255 },   // Cyan
  { 0, 0, 255 },     // Blue
  { 75, 0, 130 },    // Indigo
  //{ 238, 130, 238 }  // Violet
  { 118, 60, 60 }  // Violet faded
};

//averaging for pot
#define NUM_READINGS 30      // Number of readings to average
int readings[NUM_READINGS];  // Array to store the last n readings
int readIndex = 0;           // Index of current reading
int total = 0;               // Running total of readings
int tempo = 120;
int oldTempo = 0;
unsigned long timeOutStamp = 0;

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

//seq related
int currentStep = -1;
byte currentSeq = 0;
byte currentInst = 0;
bool playing = false;
unsigned int tempoMillis = 100;
uint16_t numberToDisplay = 0;

//led related
byte cycle = 0;
byte testColor = 255;
int textDisplayTimeout = 500;  //500 ms from text is displayed until it dissappears
int textDisplayStartTime = 0;
byte currentInstCol[3] = { 0, 255, 0 };

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

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
unsigned long clkDebounceDelay = 1;  // the debounce time; increase if the output flickers
unsigned long lastSwTime = 0;
unsigned long swDebounceTime = 5;
int swState = 0;

//i2c
const byte interruptPin = 22;

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

//SEQ related
byte solos = 0b00000000;
byte mutes = 0b00000000;
int viewOffset = 0;
byte maxViewOffset = 7;

//byte byteButtStates2D[GRIDSTEPS][GRIDROWS];
bool buttStates2D[GRIDSTEPS][GRIDROWS];
bool oldButtStates2D[GRIDSTEPS][GRIDROWS];
bool ledGridState2D[GRIDSTEPS][GRIDROWS];
CHSV ledColState2D[GRIDSTEPS][GRIDROWS];


void initLedGridState() {
  for (byte x = 0; x < GRIDSTEPS; x++) {
    for (byte y = 0; y < GRIDROWS; y++) {
      ledGridState2D[x][y] = false;
      ledColState2D[x][y] = CHSV(0, 0, 0);
    }
  }
}

byte SHIFTPIN = 8;
byte UTILPIN = 9;
byte PLAYPAUSEPIN = 10;
//byte KNOBPIN = A0;

volatile int rotaryVal = 0;
byte lastEncoded = 0;

#define EEPROM_SIZE 4096  //maximum
#define SEQUENCE_SIZE 512
#define SEQUENCE_FLAG 123

void setup() {
  //wire
  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin(48);  // do i need an argument, address??
  Wire1.onRequest(req);
  EEPROM.begin(EEPROM_SIZE);
  //init averaging:
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }

  //pinMode(KNOBPIN, INPUT);
  pinMode(SHIFTPIN, INPUT_PULLUP);
  pinMode(PLAYPAUSEPIN, INPUT_PULLUP);
  pinMode(UTILPIN, INPUT_PULLUP);

  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  pinMode(swPin, INPUT_PULLUP);
  // Read the initial state of clkPin, swPin
  clkLastState = digitalRead(clkPin);
  swLastState = digitalRead(swPin);

  pinMode(interruptPin, OUTPUT);  // to interrupt microbit
  digitalWrite(interruptPin, LOW);

  attachInterrupt(digitalPinToInterrupt(clkPin), handleRotary, CHANGE);  // Attach the interrupts
  //attachInterrupt(digitalPinToInterrupt(swPin), rotaryClick, FALLING);   // Assuming the button pulls to GND when pressed (THIS TRIGGERS ALSO WHEN KNOB IS TURNED ????)

//MIDI
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleClock(handleUsbMidiClockTicks);
  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
  //GRID
  setupGridPins();  //setup pins for buttonGrid
  //pixels.begin();   // INITIALIZE NeoPixel strip object (REQUIRED)
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // INITIALIZE NeoPixel strip object (REQUIRED)
  initLedGridState();
  FastLED.show();
  //delay(1000); // needed? 
  testSetXY(100);
  
  //wait for mount
  //while( !TinyUSBDevice.mounted()) delay(1);  //this means it wont finish booting if not connected to poooooter
  // rewrote it with timeout
  unsigned long startTime = millis();  // Capture start time
  while (!TinyUSBDevice.mounted()) {
    delay(1);                           // Wait for 1 millisecond
    if (millis() - startTime > 1000) {  // Check if 1000ms have passed
      break;                            // Exit the loop if more than 1000ms have passed
    }
  }
  testSetXY(255);
  recallSequenceFromEEPROM(0, ALLTRACKS);
}
bool runClock = true; //run internal clock
bool midiClockRunning = false;
unsigned long lastMidiClockReceivedTime = 0; //how long since last time we received a midi clock
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

void handleNoteOn(byte channel, byte pitch, byte velocity){
  debugln("note");
}

void midiClockStep(){
 //debugln(midiClockCounter);
 currentStep = (currentStep + 1) % GRIDSTEPS;
handleStep(currentStep);
}

byte mode = overviewMode;
void loop() {
  if (millis() - lastMidiClockReceivedTime > 1000) {  // is it more htan 1000ms since midi clock
      midiClockRunning = false;
  }
  //handleRotary(); //handled by interrupt
  handleRotaryPush();
  if (tempo != oldTempo) {
    tempoMillis = tempoToMillis(tempo);
    numberToDisplay = tempo;
    textDisplayStartTime = millis();
    oldTempo = tempo;
  }
  scanGrid();  //scan the grid
  scanButtsNKnobs();
  FastLED.clear();
  switch (mode) {
    case instSeqMode:
      handleInstSeqMode();
      break;
    case overviewMode:
      handleOverviewMode();
      break;
  }
  updateSparkles();
  if (millis() < textDisplayStartTime + textDisplayTimeout) {
    displayNumber(numberToDisplay, 4, 3);
  }
  checkStepTimer();            // will also draw cursor
  checkAndHandleTimedNotes();  // Continually check and handle timed notes in the background
  handleI2CTimeout();
  FastLED.show();
  MIDI.read();
}

unsigned long lastStepTime = 0;

void handleOverviewMode() {
  drawCursor(currentStep);
  drawSeqOverview(currentSeq);
  for (int row = 0; row < GRIDSTEPS; row++) {
    //drawStepState(0,0,row);
    for (int col = 0; col < GRIDROWS; col++) {
      if (buttStates2D[row][col] == true) {
        crossHair(row, col);
      }
    }
  }
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
}


void checkStepTimer() {
  unsigned long nowTime = millis();
  if (nowTime >= lastStepTime + tempoMillis) {           // if we reached the end of the step
    unsigned int goalTime = lastStepTime + tempoMillis;  // the time we were aiming for
    int diff = nowTime - goalTime;                       //how far did we overshoot?
    lastStepTime = millis() - diff;                      //compensate for overshoot
    if (playing) {
      currentStep = (currentStep + 1) % GRIDSTEPS;
      handleStep(currentStep);
    }
  }
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
    // Bit position is out of range
    return false;
  }
}

void toggleMute(byte trackNumber) {
  mutes = toggleBit(mutes, trackNumber);
}

void toggleSolo(byte trackNumber) {
  debug("SOLOED ");
  debugln(trackNumber);
  for(byte i = 0; i < 8; i++ ){
    if(i == trackNumber){
      mutes = setBit(mutes, i, false);
    } else {
      mutes = setBit(mutes, i, true);
    }
  }
  Serial.println(mutes, BIN);
}