
template <typename T> unsigned int I2C_writeAnything (const T& value)
  {
  Wire.write((byte *) &value, sizeof (value));
  return sizeof (value);
  }  // end of I2C_writeAnything

template <typename T> unsigned int I2C_readAnything(T& value)
  {
    byte * p = (byte*) &value;
    unsigned int i;
    for (i = 0; i < sizeof value; i++)
          *p++ = Wire.read();
    return i;
  }  // end of I2C_readAnything

// code that runs on i2c request
void req() { 
  int valueToReturn = abs(currentStep %8);
  Wire1.write(valueToReturn);
  //debugln("gotRequest");
}


// INTERRUPT uBit //

unsigned long i2cTimeout = 0;
byte i2cTimeoutDuration = 20;
bool isTimedOut = false;
void handleI2CTimeout() {
  if (millis() - i2cTimeout > i2cTimeoutDuration && isTimedOut == false) {
    digitalWrite(interruptPin, HIGH);
    i2cTimeout == millis();
    //debugln("i2c timeout");
    isTimedOut = true;
  }
}

void tellUbitToAskForData(){
  digitalWrite(interruptPin, LOW);
    i2cTimeout = millis();
    isTimedOut = false;
    //debugln("interruptUbit");
}

//microbit read code
//let myNum = pins.i2cReadNumber(48, NumberFormat.UInt8LE, false);