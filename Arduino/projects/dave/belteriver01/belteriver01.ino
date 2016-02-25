#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

// Defining LCD pins
#define keyboard 0x21
#define I2C_ADDR    0x27  
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define  LED_OFF  1
#define  LED_ON  0

// LCD display connection
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

// RTC settings
RTC_DS1307 RTC;


byte val = 0;

/******************** Timer values *****************/

unsigned long alarmTimer = 0;
unsigned long tmpTimer = 0;
unsigned long time = 0;
unsigned long timeOut = 0;
unsigned long clearTime = 0;
unsigned long clearPrevTime = 0;


void setup()  
{
  Wire.begin();
  Serial.begin(9600);
  lcdInit();
  rtcInit();
  mirfInit();
 

}

void loop()   {  
  voidRTC();
  clearScreen();
  mirfRec();
}

/****************** Initalizing LCD screen ********************/

void lcdInit(){
  lcd.begin (16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
}

/******************** Initalizing RTC ************************/

void rtcInit(){
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));
}

/******************* Initalizing MiRF ************************/

void mirfInit(){
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"devin");
  Mirf.payload = 32;
  Mirf.config();
}

void mirfRec(){
  Mirf.setRADDR((byte *)"devin");
  if(!Mirf.isSending() && Mirf.dataReady()){
    Mirf.getData(&val);
    lcd.setCursor(8,1);
    lcd.print("Hom:");
    lcd.print(val);
    Serial.println(val, DEC);
  }
  lcd.setCursor(5,1);
  lcd.print("Hom:");
    lcd.print(val);
  
}

/****************** Clearing the screen ******************/

void clearScreen(){
  DateTime now = RTC.now();
  clearTime = millis() / 750;
   if( clearTime != clearPrevTime){
    lcd.clear();
    clearPrevTime = clearTime;
  }        
}

/********************* Actual time *********************/

void voidRTC(){
    // Serial.println("Real time clock is clocking!!!");
    DateTime now = RTC.now();
    //lcd.clear();
    lcd.backlight();  
    lcd.setCursor(0,0);
    //lcd.print(now.year()-2000, DEC);
    //lcd.print("/");
    lcd.print(now.month(), DEC);
    lcd.print("/");
    lcd.print(now.day(), DEC);
    lcd.print(" ");
    lcd.setCursor(8,0);
    lcd.print(now.hour(), DEC);
    lcd.print(":");
    lcd.print(now.minute(), DEC);
    lcd.print(":");
    lcd.print(now.second(), DEC);
}
