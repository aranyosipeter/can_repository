// This headers for the I2C connections to the display and the RTC
#include <Wire.h>
#include <LCD.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h> 

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

// Defining LCD pins
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



int seconds=0;
int temp=0;
int backLightOn=0;
int backLightTimer=0;



void setup()  
{
  Wire.begin();
  RTC.begin();
  lcd.begin (16,2);  
  RTC.adjust(DateTime(__DATE__, __TIME__));
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
 
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 32;
  Mirf.config();
}

void loop()   
{
  voidRTC();
  voidDataReceive();
}


void voidRTC(){
DateTime now = RTC.now();
    //lcd.clear();
   lcd.backlight();  
    lcd.setCursor(4,0);
    lcd.print(now.year(), DEC);
    lcd.print("/");
    lcd.print(now.month(), DEC);
    lcd.print("/");
    lcd.print(now.day(), DEC);
    lcd.print(" ");
    lcd.setCursor(4,1);
    lcd.print(now.hour(), DEC);
    lcd.print(":");
    lcd.print(now.minute(), DEC);
    lcd.print(":");
    lcd.print(now.second(), DEC);
    
    if(now.second()!=seconds){
      lcd.clear();
      seconds=now.second();
      backLightTimer++;
    }      
}  

void voidDataReceive(){
 int data[Mirf.payload];
 
 
  if(!Mirf.isSending() && Mirf.dataReady()){
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Got packet");
    
  Mirf.getData((byte *) &data);
  lcd.setCursor(4,1);
    lcd.print(data[0], DEC);
    delay(200);
  Mirf.setTADDR((byte *)"clie1");
  Mirf.send((byte *) &data);
  lcd.clear();
  lcd.setCursor(4,0);
  Serial.println("Reply sent.");
  delay(200);
}
}


