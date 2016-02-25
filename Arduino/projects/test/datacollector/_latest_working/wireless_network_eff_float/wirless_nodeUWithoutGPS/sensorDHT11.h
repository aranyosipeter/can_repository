//DHT11 Sensor methods

/*
2015.08.06 [General]: Created/add header file => copy existed functions
2015.09.16. [Performance] Use F() function to store strings in progmem
*/


/***************** Fahrenheit ***********************/
float convertFahrenheit(int celsius){
  return 1.8 * celsius + 32;
}

/****************** DHT value ***********************/
void readDHTValues(){
  int chk = DHT11.read(DHT11PIN);
  dhtTemp = DHT11.temperature;
  dhtHum = DHT11.humidity;
  dhtFah = convertFahrenheit(dhtTemp);
  
  if (serialDataFlag){
    Serial.print(F("Humidity (%): "));
    Serial.println(dhtHum, DEC);
 
    Serial.print(F("Temperature (oC): "));
    Serial.println(dhtTemp, DEC);
 
    Serial.print(F("Temperature (oF): "));
    Serial.println(dhtFah, 2);
    Serial.println("\n");
  }
}




