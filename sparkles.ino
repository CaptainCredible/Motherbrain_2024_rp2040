#include <Arduino.h> // Ensure Arduino.h is included for millis() and data types

class Sparkle {
private:
    uint16_t x, y;
    byte red, green, blue;
    byte startRed, startGreen, startBlue; // Starting color components
    byte targetRed, targetGreen, targetBlue; // Target color components
    unsigned long timeSet;
    unsigned long lifespan;


public:
    // Default constructor
    Sparkle() : x(0), y(0), red(255), green(255), blue(255), targetRed(0), targetGreen(0), targetBlue(0), lifespan(0), timeSet(0), startRed(255), startGreen(255), startBlue(255) {}

// Constructor with target color - ensure red, green, and blue are initialized to 255
Sparkle(uint16_t x, uint16_t y, byte targetRed, byte targetGreen, byte targetBlue, unsigned long lifespan) 
  : x(x), y(y), red(255), green(255), blue(255), targetRed(targetRed), targetGreen(targetGreen), targetBlue(targetBlue), lifespan(lifespan) {
    timeSet = millis();  // Capture the current time
}

        // Constructor updated with start colors
    Sparkle(uint16_t x, uint16_t y, byte startRed, byte startGreen, byte startBlue, byte targetRed, byte targetGreen, byte targetBlue, unsigned long lifespan) 
    : x(x), y(y), red(startRed), green(startGreen), blue(startBlue), startRed(startRed), startGreen(startGreen), startBlue(startBlue), targetRed(targetRed), targetGreen(targetGreen), targetBlue(targetBlue), lifespan(lifespan) {
        timeSet = millis();  // Capture the current time
    }

    void handle() {
        unsigned long elapsed = millis() - timeSet;

        if (elapsed < lifespan) {
            float factor = (float)elapsed / lifespan;
            red = startRed + factor * (targetRed - startRed);
            green = startGreen + factor * (targetGreen - startGreen);
            blue = startBlue + factor * (targetBlue - startBlue);
        } else {
            red = targetRed;
            green = targetGreen;
            blue = targetBlue;
        }
    }

    uint16_t getX() const {
        return x;
    }

    uint16_t getY() const {
        return y;
    }

    void show() {
        setPixelXY(x, y, red, green, blue); // Implement setPixelXY to actually set the pixel on your LED grid
    }

    bool isDead() const {
        return red == targetRed && green == targetGreen && blue == targetBlue;
    }
};

const int MAX_SPARKLES = 100;
Sparkle sparkles[MAX_SPARKLES];
int startIdx = 0;
int endIdx = 0;

bool isEmpty() {
    return startIdx == endIdx;
}

bool isFull() {
    return (endIdx + 1) % MAX_SPARKLES == startIdx;
}

void addSparkle(uint16_t x, uint16_t y, byte targetRed, byte targetGreen, byte targetBlue, unsigned long lifespan) {
    sparkles[endIdx] = Sparkle(x, y, targetRed, targetGreen, targetBlue, lifespan);
    if (isFull()) {
        startIdx = (startIdx + 1) % MAX_SPARKLES;  // Remove the oldest sparkle
    }
    endIdx = (endIdx + 1) % MAX_SPARKLES;
}

void addSporkle(uint16_t x, uint16_t y, byte startRed, byte startGreen, byte startBlue, byte targetRed, byte targetGreen, byte targetBlue, unsigned long lifespan) {
    sparkles[endIdx] = Sparkle(x, y, startRed, startGreen, startBlue, targetRed, targetGreen, targetBlue, lifespan);
    if (isFull()) {
        startIdx = (startIdx + 1) % MAX_SPARKLES;  // Remove the oldest sparkle
    }
    endIdx = (endIdx + 1) % MAX_SPARKLES;
}

void updateSparkles() {
  if(true){ // Placeholder condition
    int idx = startIdx;
    while (idx != endIdx) {
        sparkles[idx].handle();
        if (!sparkles[idx].isDead()) {
            sparkles[idx].show();
        }
        idx = (idx + 1) % MAX_SPARKLES;
    }
  }
}

void removeSparkle(uint16_t targetX, uint16_t targetY) {
    int idx = startIdx;
    while (idx != endIdx) {
        if (sparkles[idx].getX() == targetX && sparkles[idx].getY() == targetY) {
            // Assuming setPixelXY to turn off the LED
            setPixelXY(targetX, targetY, 0, 0, 0);  // Implement setPixelXY if not already

            // Shift sparkles
            int nextIdx = (idx + 1) % MAX_SPARKLES;
            while (nextIdx != endIdx) {
                sparkles[idx] = sparkles[nextIdx];
                idx = nextIdx;
                nextIdx = (nextIdx + 1) % MAX_SPARKLES;
            }
            endIdx = (endIdx - 1 + MAX_SPARKLES) % MAX_SPARKLES;
            return;
        }
        idx = (idx + 1) % MAX_SPARKLES;
    }
}