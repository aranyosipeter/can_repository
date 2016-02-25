/*
  TWI/I2Cscanner
  (c) TavIR http://www.tavir.hu
 
 This sketch tests the standard 7-bit addresses
 from 0 to 127. Devices with higher bit address
 might not be seen properly. Work only at hardware I2C/TWI!

 Tested via Arduino 1.0.2
 
  Founded at arduino.cc forum
  Modified 12 March 2013
  by TavIR / Robert Cseh
 
  This example code is in the public domain.
 */

#include <Wire.h>

void setup()
{ Wire.begin();
  Serial.begin(9600);
  Serial.println("\\nI2C Scanner"); }

void loop()
{ byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 0; address <= 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    { Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\\n");
  else
    Serial.println("done\\n");

  delay(8000);
}
