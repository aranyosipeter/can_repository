#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <dht11.h>
#include <Adafruit_BMP085.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include "declarations.h"
#include "device_init.h"
#include "wireless.h"
#include "sensorBMP085.h"
#include "sensorDHT11.h"
#include "lcdDisplay.h"
#include "serialData.h"

/*
2015.06.19 [General]: modultesztek atalakitasa
2015.06.23 [General]: device_init.h letrehozasa, fuggveny atnevezes/atertelmezes
2015.07.20 [General]: declarations.h letrehozasa
2015.07.22 [Modul]: nRF modul fuggvenyeinek atalakitasa => wireless.h
2015.07.24 [Modul]: create headers for methods of LCDDisplay, BMP085 sensor
2015.07.27 [Modification]: Delete all code in connection with GPS module
2015.08.06 [Modification]: Add DHT11 sensor 
2015.09.16. [Performance] Use F() function to store strings in progmem
*/

/************************** Setup *******************************/
void setup()  {
  pinMode(IRQ, INPUT);
  Wire.begin();
  Serial.begin(9600);
  attachInterrupt(0, getPacket, LOW);
  
  if (lcdInit())  displayInit = true;
  if (rtcInit())  realTimeClockInit = true;
  if (mirfInit()) wirelessModInit = true; 
  if (bmpInit() || dhtInit())  sensorInit = true;
 
  if (!sensorInit) systemInit = false;
  
  if (!systemInit){
    if (displayInit){
      lcd.setCursor(0,1);
      lcd.print(F("System failure!"));
    }
    while(1);
  }
   Serial.println(F("Controller started!"));
}

/************************** Loop *******************************/
void loop(){
  getSensorData();
  readDHTValues();
  printRealTime();
  printLCDData();
  if (commComp) {
    for ( k = 0; k < s; k++ ){
    Serial.print(command[k]);  
    }
    Serial.println("");
    commandProc();
    commComp = false;
    s = 0;
  }
  if (millis() >= (updateTimer + updateTime)){
    updateTimer = millis();
    if ((sendPacket()) && (debugFlag)){
      Serial.println(F("Data sent!"));
    }
  }
}

