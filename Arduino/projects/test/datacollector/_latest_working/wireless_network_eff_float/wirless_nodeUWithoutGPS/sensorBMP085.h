//BMP085 Sensor functions

/*
2015.07.24 [General]: Created/add header file => copy existed functions
2015.07.27 [Modification]: Delete all code in connection with GPS module
2015.09.16. [Performance] Use F() function to store strings in progmem
*/

/**************************************************************************/
/*                      Reading values from sensor                        */
/**************************************************************************/
void getSensorData(){  
  bmpTemp = bmp.readTemperature();
  bmpPres = bmp.readPressure();
  bmpPreshPa = (float)bmpPres / 100;
  bmpRealAlt = bmp.readAltitude(102425);
  bmpSeaLev = ((float)bmp.readSealevelPressure(124)) / 100;
  
  if (debugFlag) {
    Serial.print(F("Temperature = "));
    Serial.print((float)bmpTemp, 2);
    Serial.println(F(" *C"));
    Serial.print(F("Pressure = "));
    Serial.print(bmpPres, DEC);
    Serial.println(F(" Pa"));
    Serial.print(F("Sea level = "));
    Serial.print((float)bmpSeaLev, 2);
    Serial.println(F(" hPa"));
    Serial.print(F("Real altitude = "));
    Serial.print((float)bmpRealAlt, 2);
    Serial.println(F(" meters"));
    Serial.println("\n");
  } 
}

