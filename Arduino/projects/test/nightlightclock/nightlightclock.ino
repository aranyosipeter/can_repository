
#include <Wire.h>
#include <LCD.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>  


#define I2C_ADDR    0x27  
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define PIR 4
#define LED 13
#define  LED_OFF  1
#define  LED_ON  0

RTC_DS1307 RTC;
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

int pirCount=0;
int seconds=0;
int temp=0;
int backLightOn=0;
int backLightTimer=0;


void setup()  
{
   lcd.setBacklight(LED_ON);
  Wire.begin();
  RTC.begin();
  lcd.begin (16,2);  
  RTC.adjust(DateTime(__DATE__, __TIME__));
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  pinMode(PIR, INPUT);
  pinMode(LED, OUTPUT);
}

void loop()   
{
  
 // voidDisplay();
  voidRTC();
  voidPIR();
}

void voidDisplay(){
  lcd.clear();
  if (digitalRead(PIR)==HIGH){ 
    pirCount++;
    if (pirCount==3){
      lcd.setCursor(0,0); 
      lcd.print("Behatolas");
      lcd.backlight();
      pirCount=0;
      delay(1000);
     
    }    
  } 
}

void voidRTC(){
DateTime now = RTC.now();
    //lcd.clear();
   
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

void voidPIR(){
  if(backLightOn==1 && backLightTimer<=5){
    lcd.backlight();   
   }   
  if(backLightTimer>5){
    lcd.noBacklight();
    backLightOn=0;
  }
  //digitalWrite(LED, LOW);
  if(digitalRead(PIR)==HIGH){ 
    pirCount++;
    digitalWrite(LED, HIGH);
    if (pirCount==3){
      pirCount=0;
      backLightTimer=0;
      backLightOn=1;
      delay(1000);   
        }
     digitalWrite(LED, LOW);
  }
}
