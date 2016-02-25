#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <Adafruit_BMP085.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include "declarations.h"
#include "device_init.h"
#include "wireless.h"
#include "gps.h"
#include "sensorBMP085.h"
#include "lcdDisplay.h"

/*
2015.06.19 [General]: modultesztek atalakitasa
2015.06.23 [General]: device_init.h letrehozasa, fuggveny atnevezes/atertelmezes
2015.07.20 [General]: declarations.h letrehozasa
2015.07.22 [Modul]: nRF modul fuggvenyeinek atalakitasa => wireless.h
2015.07.24 [Modul]: create headers for methods of LCDDisplay, BMP085 sensor
*/

/************************** Setup *******************************/
void setup()  {
  pinMode(IRQ, INPUT);
  attachInterrupt(0, sendResponse, LOW);
  Wire.begin();
  Serial.begin(9600);
  
  if (lcdInit())  displayInit = true;
  if (rtcInit())  realTimeClockInit = true;
  if (mirfInit()) wirelessModInit = true; 
  if (bmpInit())  sensorInit = true;
  //if (GPSInit())  gpsInit = true;
  
  if (!sensorInit) systemInit = false;
  
  if (!systemInit){
    if (displayInit){
      lcd.setCursor(0,1);
      lcd.print(F("System failure!"));
    }
    while(1);
  }
}

/************************** Loop *******************************/
void loop(){
  printRealTime();
  //getGPSDateTime();
  printLCDData();
  //processPacket();
  getSensorData();
  printSerialData();
  if (commComp) {
    for ( k = 0; k < s; k++ ){
    Serial.print(command[k]);  
    }
    Serial.println("");
    commandProc();
    commComp = false;
    s = 0;
  }
}

/*************************** SerialEvent *********************/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char commandChar = (char)Serial.read(); 
    // add it to the inputString:
    command[s] = commandChar;
    s++;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (commandChar == '\n') {
      commComp = true;
    } 
  }
}

/************************ Commands *************************/
void commandProc(){
  if ( (command[0] == dataChar[0]) && (command[1] == dataChar[1]) && (command[2] == dataChar[2]) && (command[3] == dataChar[3])) {
    dataFlag = true;
    Serial.println("data parancs fogadva"); 
  }
  else {
    dataFlag = false;
    Serial.println("nem data parancs"); 
  } 
  if (( dataFlag == true ) && ( serialDataFlag == false )) serialDataFlag = true;
  if (( dataFlag == false ) && ( serialDataFlag == true )) serialDataFlag = false; 
  if ( command[0] == relayChar[0] && command[1] == relayChar[1] && command[2] == relayChar[2] && command[3] == relayChar[3] && command[4] == relayChar[4] ) {
    relayFlag = true;
    Serial.println("relay parancs fogadva");
  }
  else {
    relayFlag = false;
    Serial.println("nem relay parancs");
  }
  if ( relayFlag == true ){
    relayComPin = atoi(&command[6]) - 1;
    Serial.print("Vezerelt rele szama: ");
    Serial.println(relayComPin, DEC);
    if ( command[10] == trueChar[0]  && command[11] == trueChar[1] && command[12] == trueChar[2] && command[13] == trueChar[3] ){
      Serial.print("Allapota: ");
      Serial.println("Bekapcsolva"); 
      bitSet(relayBuff, relayComPin);
      Serial.println(relayBuff, BIN);
    }
    if ( command[10] == falseChar[0]  && command[11] == falseChar[1] && command[12] == falseChar[2] && command[13] == falseChar[3] && command[14] == falseChar[4] ){
      Serial.print("Allapota: "); 
      Serial.println("Kikapcsolva"); 
      bitClear(relayBuff, relayComPin);
      Serial.println(relayBuff, BIN);
    }
  }
}

/******************************************- Serial data ************************************/
void printSerialData(){
  if ( serialDataFlag == true && dataRec == true){
    Serial.print("Fenyero: ");
    Serial.print(light, DEC);
    Serial.println(" %");
    Serial.print("Paratartalom: ");
    Serial.print(dhtHum, DEC);
    Serial.println(" %");
    Serial.print("Homerseklet: ");
    Serial.print(dhtTemp, DEC);
    Serial.println(" C");
    Serial.print("Legnyomas: ");
    Serial.print(bmpPreshPa, DEC);
    Serial.println(" kPa");
    dataRec = false;  
  }
  
}

/********************* Adjust time and date ************************/
/*void adjTimeAndDate(){
  if (adjFlag) {
    if (
  }
}*/



