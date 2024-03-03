
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
  int valueToReturn = 3;
  Wire1.write(valueToReturn);
  debugln("gotRequest");
}

//microbit read code
//let myNum = pins.i2cReadNumber(48, NumberFormat.UInt8LE, false);