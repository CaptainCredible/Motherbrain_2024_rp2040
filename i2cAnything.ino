

template<typename T> unsigned int I2C_writeAnything(const T& value) {
  Wire1.write((byte*)&value, sizeof(value));
  return sizeof(value);
}  // end of I2C_writeAnything

template<typename T> unsigned int I2C_readAnything(T& value) {
  byte* p = (byte*)&value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++)
    *p++ = Wire1.read();
  return i;
}  // end of I2C_readAnything