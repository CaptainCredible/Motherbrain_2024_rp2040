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

int gridBrightness = 6;  // shift amount

void drawHelpers() {
  for (byte i = 0; i < GRIDSTEPS >> 2; i++) {
    for (byte j = 0; j < GRIDROWS; j++) {
      //trackColors[8][3]
      //softSetPixelXY(i << 2, j, 2, 0, 0);

      softSetPixelXY(i << 2, j, currentInstCol[0] >> gridBrightness, currentInstCol[1] >> gridBrightness, currentInstCol[2] >> gridBrightness);
    }
  }
}

//bool pianoRoll[12] = {false, true, false, true, false, false, true, false, true, false, true, false};
bool pianoRoll[12] = { true, false, false, false, false, false, false, true, false, false, false, false };
void drawPianoRoll() {
  for (int row = 0; row < GRIDROWS; row++) {
    byte invRow = 7 - row;
    int offsetRow = invRow + viewOffset;
    //if(offsetRow<0){offsetRow = 11;}
    if (pianoRoll[offsetRow % 12]) {
      for (byte stp = 0; stp < GRIDSTEPS; stp++) {
        softSetPixelXY(stp, row, currentInstCol[0] >> gridBrightness, currentInstCol[1] >> gridBrightness, currentInstCol[2] >> gridBrightness);
      }
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
    softSetPixelXY(x, i, 8, 8, 8);
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

char alphabet[26][5][4] = {
  // A
  {
    { ' ', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' } },
  // B
  {
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', ' ' } },
  // C
  {
    { ' ', '#', '#', '#' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { ' ', '#', '#', '#' } },
  // D
  {
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', ' ' } },
  // E
  {
    { '#', '#', '#', '#' },
    { '#', ' ', ' ', ' ' },
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', '#', '#', '#' } },
  // F
  {
    { '#', '#', '#', '#' },
    { '#', ' ', ' ', ' ' },
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' } },
  // G
  {
    { ' ', '#', '#', '#' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', '#', '#' },
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' } },
  // H
  {
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' } },
  // I
  {
    { '#', '#', '#', '#' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { '#', '#', '#', '#' } },
  // J
  {
    { '#', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' } },
  // K
  {
    { '#', ' ', ' ', '#' },
    { '#', ' ', '#', ' ' },
    { '#', '#', ' ', ' ' },
    { '#', ' ', '#', ' ' },
    { '#', ' ', ' ', '#' } },
  // L
  {
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', '#', '#', '#' } },
  // M
  {
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' } },
  // N
  {
    { '#', ' ', ' ', '#' },
    { '#', '#', ' ', '#' },
    { '#', ' ', '#', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' } },
  // O
  {
    { ' ', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' } },
  // P
  {
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' } },
  // Q
  {
    { ' ', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', '#', '#' },
    { ' ', '#', '#', ' ' },
    { ' ', ' ', ' ', '#' } },
  // R
  {
    { '#', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', ' ' },
    { '#', '#', ' ', ' ' },
    { '#', ' ', ' ', '#' } },
  // S
  {
    { ' ', '#', '#', '#' },
    { '#', ' ', ' ', ' ' },
    { ' ', '#', '#', ' ' },
    { ' ', ' ', ' ', '#' },
    { '#', '#', '#', ' ' } },
  // T
  {
    { '#', '#', '#', '#' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' } },
  // U
  {
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' } },
  // V
  {
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' },
    { ' ', ' ', '#', ' ' } },
  // W
  {
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', '#', '#', '#' },
    { '#', ' ', ' ', '#' } },
  // X
  {
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', '#', '#', ' ' },
    { '#', ' ', ' ', '#' } },
  // Y
  {
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' },
    { ' ', ' ', '#', ' ' } },
  // Z
  {
    { '#', '#', '#', '#' },
    { ' ', ' ', ' ', '#' },
    { ' ', ' ', '#', ' ' },
    { ' ', '#', ' ', ' ' },
    { '#', '#', '#', '#' } }
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


void displayText(const char* text, int xPos, int yPos, bool ping) {
  int x = xPos;
  int y = yPos;
  int charWidth = 4;                                             // Width of a character in the 5x4 grid
  int charHeight = 5;                                            // Height of a character
  int spacing = 1;                                               // Space between characters
  int textLength = strlen(text);                                 // Get the length of the text
  int textWidth = textLength * (charWidth + spacing) - spacing;  // Calculate total width of the text

  // Optional: dim frame around the text for visibility
  dimFrame(x - 1, y - 1, textWidth + 2, charHeight + 2, 4);
  dimFrame(x - 1, y - 1, textWidth + 4, charHeight + 4, 2);
  dimFrame(x - 2, y - 2, textWidth + 6, charHeight + 6, 1);

  for (int k = 0; k < textLength; k++) {
    char ch = text[k];
    
    if (ch == ' ') { // Check if the character is a space
      // Clear the space for the 'space' character
      for (int i = 0; i < charHeight; i++) {
        for (int j = 0; j < charWidth; j++) {
          setPixelXY(x + j, y + i, 0, 0, 0);
        }
      }
      x += charWidth; // Move x position for the next character, including space width
      continue; // Skip the rest of the loop and continue with the next character
    }

    int ascii = (int)ch;
    // Convert ASCII to index in alphabet array: 'A' = 65, 'Z' = 90, 'a' = 97, 'z' = 122
    int index = (ascii >= 'A' && ascii <= 'Z') ? ascii - 'A' : (ascii >= 'a' && ascii <= 'z') ? ascii - 'a' : -1;
    if (index != -1) {
      // LETTER display
      for (int i = 0; i < charHeight; i++) {
        for (int j = 0; j < charWidth; j++) {
          int currentX = x + j;
          int currentY = y + i;
          if (alphabet[index][i][j] == '#') {
            if (ping) {
              addSparkle(currentX, currentY, 0, 0, 0, 1000);
            } else {
              setPixelXY(currentX, currentY, 100, 100, 100);  // show pixel
            }
          } else {
            setPixelXY(currentX, currentY, 0, 0, 0);  // Clear the pixel
          }
        }
      }
    }
    // Clear the spacing strip between characters, except after the last character or space
    if (k < textLength - 1) {
        x += charWidth; // Adjust x for the width of the character just processed
        for (int i = 0; i < charHeight; i++) {
            setPixelXY(x, y + i, 0, 0, 0);  // Clear the pixels for spacing
        }
        x += spacing;  // Include the spacing for the next character position
    } else {
        // For the last character, just move the position without clearing
        x += charWidth + spacing;
    }
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

void setMessage(const char* message) {
  strncpy(scrollText, message, sizeof(scrollText) - 1); // Copy the incoming message to the global variable
  scrollText[sizeof(scrollText) - 1] = '\0'; // Ensure null-termination
  scrollTimer = millis(); // Reset the timer at the start of a new message
  isScrolling = true; // Enable scrolling
}

void updateScroll() {
  if (!isScrolling) return; // Exit if scrolling is not enabled

  long textPosX = millis() - scrollTimer; // Calculate the elapsed time since the scrolling started
  textPosX = textPosX >> scrollSpeed; // Apply the scrolling speed
  textPosX = textPosX * -1; // Invert the direction for leftward scrolling

  displayText(scrollText, textPosX + 15, 1, false); // Adjust +15 to set starting offset as needed

  // Optionally, stop scrolling after the text has moved off screen
  int textLength = strlen(scrollText)+4; // add 4 pretend characters to make it scroll all the way
  int charWidth = 4; // Width of a character in the 5x4 grid
  int spacing = 1; // Space between characters
  int screenEnd = -(textLength * (charWidth + spacing)); // When to stop scrolling
  if (textPosX < screenEnd) {
    isScrolling = false; // Stop scrolling when the end of the message has passed
  }
}


void initLedGridState() {
  for (byte x = 0; x < GRIDSTEPS; x++) {
    for (byte y = 0; y < GRIDROWS; y++) {
      ledGridState2D[x][y] = false;
      ledColState2D[x][y] = CHSV(0, 0, 0);
    }
  }
}
