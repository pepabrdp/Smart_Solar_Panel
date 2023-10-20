#include <Wire.h>
void setup() 
{
  Serial.begin(9600);                            // Initialize serial communication at 9600
  Wire.begin();                                  // Initialize Arduino in I2C master.
  Wire.beginTransmission(0x44);                  // I2C address of OPT3001 = 0x44
  Wire.write(0x0A);                              // Config register address 0x01
  Wire.write(0x32);
  Wire.write(0x08);                              
  Wire.endTransmission();
  Serial.println("Data received \t\t Lux");
}
void loop() 
{
  Wire.beginTransmission(0x44);
  Wire.write(0x00);                              // Send result register address
  //Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x44, 2);                     // Request 2 bytes data from OPT3001
  uint16_t iData;
  uint8_t  iBuff[2];
  while (Wire.available()) 
  { 
    Wire.readBytes(iBuff, 2);
    iData = (iBuff[0] << 8) | iBuff[1];
    //iData = iBuff[0];
    Serial.print(iData,BIN);                     // Print the received data
    Serial.print("\t\t\n");
    float fLux = SensorOpt3001_convert(iData);   // Calculate LUX from sensor data
    Serial.println(fLux);                        // Print it on serial terminal
  }
  delay(1000);
}
float SensorOpt3001_convert(uint16_t iRawData)
{
  uint16_t iExponent, iMantissa;
  iMantissa = iRawData & 0x0FFF;                 // Extract Mantissa
  iExponent = (iRawData & 0xF000) >> 12;         // Extract Exponent 
  return iMantissa * (0.01 * pow(2, iExponent)); // Calculate final LUX
}
