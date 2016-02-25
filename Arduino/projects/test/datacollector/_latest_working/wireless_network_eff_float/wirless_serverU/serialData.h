// Methods of serial communication

/*
2015.08.03. [General] Create header
2015.09.16. [Performance] Use F() function to store strings in progmem
*/

/*********************************************************************************************************/
/*                                    Write out received datas                                           */
/*********************************************************************************************************/
void printDataSerial(){
  Serial.print(F("ID: "));
  Serial.println(receiveID);
  
  Serial.print(F("Command: "));
  Serial.println(receiveCommand);
  
  Serial.print(F("Temp: "));
  Serial.println(dhtTemp);
  
  Serial.print(F("Humidity: "));
  Serial.println(dhtHum);
  
  Serial.print(F("Pressure: "));
  Serial.println(bmpPreshPa);
  
  Serial.print(F("BMPTemp: "));
  Serial.println(bmpTemp);
  
  Serial.print(F("Sea level pressure: "));
  Serial.println(bmpSeaLev);
}
