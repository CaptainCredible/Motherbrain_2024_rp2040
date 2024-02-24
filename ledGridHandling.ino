byte pixelMapArray[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 15, 14, 13, 12, 11, 10, 9, 8 };
byte pixelMapArrayWIDE[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16 };
bool pianoKeys[12] = { true, false, true, false, true, true, false, true, false, true, false, true };  // true for hite keys, false for black keys

byte pixelCount(int pxl) {  //translate from pixels numbered from left to right, to the snaked number configuration that the strip has
  int multiplier = pxl / 32;
  //int multiplier = pxl >> 5; // faster?
  return pixelMapArrayWIDE[pxl % 32] + (32 * multiplier);
}

byte pixelXY(int Xcoord, int Ycoord) {  //take XY coordinate and return the number of the neopixel
  int trueX = constrain(Xcoord, 0, GRIDSTEPS);
  int trueY = constrain(Ycoord, 0, GRIDROWS);
  trueY = (GRIDROWS - 1) - trueY;
  int countNum = trueX + (trueY * GRIDSTEPS);
  return pixelCount(countNum);
}

void setALED(byte ledNumber, byte red, byte green, byte blue) {
  leds[ledNumber] = CRGB(green, red, blue);
}

void setALEDHSV(byte ledNumber, byte myHue, byte mySat, byte myVal) {
  leds[ledNumber] = CHSV(myHue, mySat, myVal);
}

void setPixelXY(int Xcoord, int Ycoord, byte R, byte G, byte B) {
  if (Xcoord >= 0 && Xcoord < GRIDSTEPS) {
    if (Ycoord >= 0 && Ycoord < GRIDROWS) {  // check that X and Y are within range
      //pixels.setPixelColor(pixelXY(Xcoord, Ycoord), pixels.Color(R,G,B));
      setALED(pixelXY(Xcoord, Ycoord), R, G, B);
    }
  }
}

int gridBrightness = 3;

void drawHelpers() {
  for (byte i = 0; i < GRIDSTEPS >> 2; i++) {
    for (byte j = 0; j < GRIDROWS; j++) {
      softSetPixelXY(i << 2, j, gridBrightness, 0, 0);
    }
  }
}

void drawPianoRoll(){
    for (int row = 0; row < GRIDROWS >> 2; row++) {
      int rowToDraw = ((GRIDROWS-1)-(row<<2) + viewOffset)%GRIDROWS;
      if(rowToDraw < 0){
        rowToDraw+=GRIDROWS; // wrap around to the bottom
      }
    for (byte stp = 0; stp < GRIDSTEPS; stp++) {
      softSetPixelXY(stp, rowToDraw, gridBrightness, 0, 0);
    }
  }
}

void setPixelXYP(int Xcoord, int Ycoord, CHSV colour) {
  if (Xcoord >= 0 && Xcoord < GRIDSTEPS) {
    if (Ycoord >= 0 && Ycoord < GRIDROWS) {  // check that X and Y are within range
      leds[pixelXY(Xcoord, Ycoord)] = colour;
      //setALEDHSV(pixelXY(Xcoord, Ycoord), colour);
    }
  }
}

void dimShiftPixelXY(int Xcoord, int Ycoord, int dimShiftAmt) {  //bitShift brightness
  if (Xcoord >= 0 && Xcoord < GRIDSTEPS) {
    if (Ycoord >= 0 && Ycoord < GRIDROWS) {  // check that X and Y are within range
      dimShiftLed(pixelXY(Xcoord, Ycoord), dimShiftAmt);
    }
  }
}

void dimPixelXY(int Xcoord, int Ycoord, int dimAmt) {  // dim by fixed amount
  if (Xcoord >= 0 && Xcoord < GRIDSTEPS) {
    if (Ycoord >= 0 && Ycoord < GRIDROWS) {  // check that X and Y are within range
      dimLed(pixelXY(Xcoord, Ycoord), dimAmt);
    }
  }
}

void dimShiftLed(uint16_t pixelNumber, uint8_t dimShift) {
  // Ensure dimShift is within a valid range (0-8)
  if (dimShift > 8) {
    dimShift = 8;
  }

  // Read the current color value
  CHSV currentColor = rgb2hsv_approximate(leds[pixelNumber]);

  // Dim the brightness (value) using bit-shifting
  currentColor.v >>= dimShift;

  // Convert back to RGB and update the LED
  leds[pixelNumber] = currentColor;
}

void dimLed(uint16_t pixelNumber, byte dimAmount) {
  // Ensure pixelNumber is within a valid range
  if (pixelNumber >= 0 && pixelNumber < NUM_LEDS) {
    // Read the current color of the LED
    CHSV currentColor = rgb2hsv_approximate(leds[pixelNumber]);

    // Dim the LED by reducing its Value (brightness)
    if (dimAmount <= currentColor.v) {
      currentColor.v -= dimAmount;  // Subtract dimAmount from brightness
    } else {
      currentColor.v = 0;  // Ensure brightness doesn't go below 0
    }

    // Update the LED with the dimmed color
    leds[pixelNumber] = currentColor;
  }
}


void softSetPixelXY(int Xcoord, int Ycoord, byte R, byte G, byte B) {               // only set if this button isn't pressed
  if ((Xcoord >= 0 && Xcoord < GRIDSTEPS) && (Ycoord >= 0 && Ycoord < GRIDROWS)) {  // check that X and Y are within range
    if (!ledGridState2D[Xcoord][Ycoord]) {
      setALED(pixelXY(Xcoord, Ycoord), R, G, B);
    }
  }
}


void softSetPixelXYP(int Xcoord, int Ycoord, uint32_t colour) {                     // only set if this button isn't pressed
  if ((Xcoord >= 0 && Xcoord < GRIDSTEPS) && (Ycoord >= 0 && Ycoord < GRIDROWS)) {  // check that X and Y are within range
    if (!buttStates2D[Xcoord][Ycoord]) {
      leds[pixelXY(Xcoord, Ycoord)] = colour;
    }
  }
}

//PATTERNS


void crossHair(byte X, byte Y) {
  int colorHue = knobVal << 6;
  CHSV color = CHSV(colorHue, 255, 255);
  ledColState2D[X][Y] = color;
  /*
      for(int x = 0; x < GRIDSTEPS; x++){
        setPixelXY(x,Y,255,0,0);
      }
      /*
      for(int y = 0; y < GRIDROWS; y++){
        setPixelXY(X,y,255, 0, 0);
      }
      */
  setPixelXY(X, Y, 255, 255, 255);  // <--CENTER
}

void drawCursor(byte x) {
  for (byte i = 0; i < GRIDROWS; i++) {
    softSetPixelXY(x, i, 20, 0, 0);
  }
}


char digits[10][5][4] = {
  { { ' ', '#', '#', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', '#', '#' } },  // 0

  {
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' } },  // 1

  {
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', ' ' },
    { ' ', '#', '#', '#' } },  // 2

  {
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', '#', '#', '#' } },  // 3

  {
    { ' ', '#', ' ', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', ' ', ' ', '#' } },  // 4

  {
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', ' ' },
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', '#', '#', '#' } },  // 5

  {
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', ' ' },
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', '#', '#' } },  // 6

  {
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' } },  // 7

  {
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', '#', '#' } },  // 8

  {
    { ' ', '#', '#', '#' },
    { ' ', '#', ' ', '#' },
    { ' ', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', '#', '#', '#' } }  // 9
};

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void dimFrame(int x, int y, int w, int h, int dimAmt) {
  for (int i = x; i < x + w; i++) {  // Dim the top row
    dimShiftPixelXY(i, y, dimAmt);
  }
  for (int i = x; i < x + w; i++) {  // Dim the bottom row
    dimShiftPixelXY(i, y + h - 1, dimAmt);
  }
  for (int i = y + 1; i < y + h - 1; i++) {  // Dim the left column (excluding top and bottom pixels)
    dimShiftPixelXY(x, i, dimAmt);
  }
  for (int i = y + 1; i < y + h - 1; i++) {  // Dim the right column (excluding top and bottom pixels)
    dimShiftPixelXY(x + w - 1, i, dimAmt);
  }
}


void displayNumber(int number, int xPos, int yPos) {
  if (number < 0) return;  // Negative numbers are not handled
  int x = xPos;
  int y = yPos;
  int textWidth = 12;  // width if 3 digits
  int textHeight = 5;  // height of character
  if (number < 10) {
    x += 8;
    textWidth -= 8;
  } else if (number < 100) {
    x += 4;
    textWidth -= 4;
  }
  dimFrame(x - 1, y - 1, textWidth + 2, textHeight + 2, 4);
  dimFrame(x - 1, y - 1, textWidth + 4, textHeight + 4, 2);
  dimFrame(x - 2, y - 2, textWidth + 6, textHeight + 6, 1);
  String strNumber = String(number);  // Convert number to a string to easily get individual digits
  int digitWidth = 4;                 // As each digit is represented in a 4x5 grid, we'll use 4 as the width.
  for (int k = 0; k < strNumber.length(); k++) {
    int digit = strNumber.charAt(k) - '0';  // Convert character to integer

    //Frame
    for (int i = 0; i < 5; i++) {
      int currentX = x + i;
      int currentY = y - 1;
      for (int height = 0; height < 7; height++) {
        dimShiftPixelXY(currentX, currentY + height, 3);
      }
    }


    //DIGIT display
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {
        int currentX = x + j;
        int currentY = y + i;
        if (digits[digit][i][j] == '#') {
          setPixelXY(currentX, currentY, 0, 30, 50);  // Display the digit at the current position
        } else {
          setPixelXY(currentX, currentY, 0, 0, 0);  // Display the digit at the current position
        }
      }
    }
    x += digitWidth;  // Move the x position for the next digit
  }
}

















/////////
//DEBUG FUNCTIONS
///////

void testSetCount() {  //degug function
  for (int count = 0; count < 128; count++) {
    FastLED.clear();
    leds[count] = CRGB(0, 255, 0);
    FastLED.show();
    delay(20);
  }
}

void testSetXY(byte testColor) {  //debug function
  for (int y = 0; y < 8; y++) {
    FastLED.clear();
    for (int x = 0; x < 16; x++) {
      setPixelXY(x, y, 255 - testColor, testColor, 0);
    }
    FastLED.show();
    delay(10);
  }
}

void testCursor() {
  for (byte i = 0; i < GRIDSTEPS; i++) {
    FastLED.clear();
    drawCursor(i);
    FastLED.show();
    delay(100);
  }
}