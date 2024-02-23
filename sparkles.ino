class Sparkle {
private:
    uint16_t x, y;
    byte red, green, blue;
    unsigned long timeSet;
    unsigned long lifespan;

public:
    Sparkle() : x(0), y(0), red(255), green(255), blue(255), lifespan(0), timeSet(0) {}

    Sparkle(uint16_t x, uint16_t y, unsigned long lifespan) 
      : x(x), y(y), red(255), green(255), blue(255), lifespan(lifespan) {
        timeSet = millis();  // Capture the current time
    }

    // Update the sparkle's brightness based on elapsed time
    void handle() {
        unsigned long elapsed = millis() - timeSet;

        if (elapsed < lifespan) {
            float factor = (float)elapsed / lifespan;
            red = 255 + factor * (currentInstCol[0] - 255);
            green = 255 + factor * (currentInstCol[1] - 255);
            blue = 255 + factor * (currentInstCol[2] - 255);
        } else {
            red = currentInstCol[0];
            green = currentInstCol[1];
            blue = currentInstCol[2];
        }
    }

    uint16_t getX() const {
        return x;
    }

    uint16_t getY() const {
        return y;
    }

    // Display the sparkle on the grid
    void show() {
        setPixelXY(x, y, red, green, blue);
    }

    // Check if the sparkle is "dead"
    bool isDead() const {
        // Considering a sparkle dead if its color matches the global variable
        return red == currentInstCol[0] && green == currentInstCol[1] && blue == currentInstCol[2];
    }
};

const int MAX_SPARKLES = 50;
Sparkle sparkles[MAX_SPARKLES];
int startIdx = 0;  // Points to the oldest sparkle
int endIdx = 0;   // Points to the next empty slot

// Check if buffer is empty
bool isEmpty() {
    return startIdx == endIdx;
}

// Check if buffer is full
bool isFull() {
    return (endIdx + 1) % MAX_SPARKLES == startIdx;
}

void addSparkle(uint16_t x, uint16_t y, unsigned long lifespan) {
    sparkles[endIdx] = Sparkle(x, y,lifespan);
    if (isFull()) {
        startIdx = (startIdx + 1) % MAX_SPARKLES;  // Remove the oldest sparkle
    }
    endIdx = (endIdx + 1) % MAX_SPARKLES;
}

void updateSparkles() {
  if(mode == instSeqMode){ // only do this shit if we are in seq mode bruv
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
            // Clear the pixel corresponding to this sparkle
            setPixelXY(targetX, targetY, 0, 0, 0);  // Assuming 0,0,0 is off/black

            // Overwrite the sparkle to be deleted by shifting all subsequent sparkles one position backwards
            int nextIdx = (idx + 1) % MAX_SPARKLES;
            while (nextIdx != endIdx) {
                sparkles[idx] = sparkles[nextIdx];
                idx = nextIdx;
                nextIdx = (nextIdx + 1) % MAX_SPARKLES;
            }
            // Adjust the endIdx to the new end of the buffer
            endIdx = (endIdx - 1 + MAX_SPARKLES) % MAX_SPARKLES; // Decrease endIdx, but ensure it remains non-negative
            return; // Exit after removing the sparkle
        }
        idx = (idx + 1) % MAX_SPARKLES;
    }
}

