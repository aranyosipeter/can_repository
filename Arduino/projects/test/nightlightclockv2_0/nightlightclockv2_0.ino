
#include <Wire.h>
#include <LCD.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>  
#include <EEPROM.h>


#define I2C_ADDR    0x27  
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define LED 13
#define  LED_OFF  1
#define  LED_ON  1
#define BUTTON1 2
#define BUTTON2 3
#define BUTTON3 4
#define BUTTON4 5
#define BUTTON5 6
#define BUZZER 9

RTC_DS1307 RTC;
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

int j = 0;
int buzz = 0;
int prevBuzz = 0;

boolean alarmFlag = false;
boolean menuFlag = false;
boolean settingFlag = false;
boolean examsFlag = false;
boolean saveFlag = false;

boolean buttonPush1 = false;
boolean buttonPush2 = false;
boolean buttonPush3 = false;
boolean buttonPush4 = false;
boolean buttonPush5 = false;


int hours = 0;
int minutes = 0;
int seconds = 0;
unsigned long mills = 0;
unsigned long subtract = 0;

int alarmHours = 0;
int alarmMinutes = 0;

int temp = 0;
int backLightOn = 0;
int backLightTimer = 0;

int function = 0;

int butt1 = 0;
int butt2 = 0;
int butt3 = 0;
int butt4 = 0;
int butt5 = 0;

int button1 = 0;
int button2 = 0;
int button3 = 0;
int button4 = 0;
int button5 = 0;
DateTime now;

void setup(){

  //lcd.setBacklight(LED_ON);
  Wire.begin();
  RTC.begin();
  lcd.begin (16,2);  
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  pinMode(BUTTON4, INPUT);
  pinMode(BUTTON5, INPUT);
  digitalWrite(BUZZER, LOW);
  /****************** EEPROM Config *******************/
  if ( EEPROM.read(1) <= 23 ){
    alarmHours = EEPROM.read(1);
  } 
  else {
    alarmHours = 0; 
  }
  if ( EEPROM.read(2) <= 59 ){
    alarmMinutes = EEPROM.read(2);
  } 
  else {
    alarmMinutes = 0; 
  } 

  if ( EEPROM.read(3) > 1 ){
    alarmFlag = false;
  } 
  else if ( EEPROM.read(3) == 1 ){
    alarmFlag = true; 
  }  
  else if ( EEPROM.read(3) == 0 ){
    alarmFlag = false; 
  }  

}

void loop(){ 
  voidEEPROM();
  voidRTC();
  voidBackLight();
  buttonFunction();
  voidAlarmSetting();
  voidAlarmDisplay();
  voidButtonBuzzer();
  voidAlarm();
}

/***************** voidRTC ***************/
void voidRTC(){
  now = RTC.now();

  lcd.setCursor(0,0);
  lcd.print(now.year(), DEC);

  lcd.setCursor(4,0);
  lcd.print("/");

  if ( now.month() < 10 ){
    lcd.setCursor(5,0);
    lcd.print(0, DEC);
    lcd.setCursor(6,0);
    lcd.print(now.month(), DEC);
  }
  else {
    lcd.setCursor(5,0);
    lcd.print(now.month(), DEC);
  }

  lcd.setCursor(7,0);
  lcd.print("/");

  if ( now.day() < 10 ){
    lcd.setCursor(8,0);
    lcd.print(0, DEC);
    lcd.setCursor(9,0);
    lcd.print(now.day(), DEC);
  }
  else {
    lcd.setCursor(8,0);
    lcd.print(now.day(), DEC);
  }

  if ( now.hour() < 10 ){
    lcd.setCursor(11,0);
    lcd.print(0, DEC);
    lcd.setCursor(12,0);
    lcd.print(now.hour(), DEC);
  }
  else {
    lcd.setCursor(11,0);
    lcd.print(now.hour(), DEC);
  }


  if ( seconds % 2 == 0 ){
    lcd.setCursor(13,0);
    lcd.print(":");
  }
  else{
    lcd.setCursor(13,0);
    lcd.print(" ");
  }

  if ( now.minute() < 10 ){
    lcd.setCursor(14,0);
    lcd.print(0, DEC);
    lcd.setCursor(15,0);
    lcd.print(now.minute(), DEC);
  }
  else {
    lcd.setCursor(14,0);
    lcd.print(now.minute(), DEC);
  }

  //lcd.print(":");
  //lcd.print(now.second(), DEC);

  if(now.second()!=seconds){
    lcd.clear();
    seconds=now.second();
  }     
}  

/*************** voidBackLight **************/
void voidBackLight(){
  hours = now.hour();
  if ((hours > 16) || (hours < 8)){
    lcd.backlight();
  }
  else {
    lcd.noBacklight();
  }
}

/************ voidAlarmSetting *************/
void voidAlarmSetting(){
  if (button1 == true){
    alarmHours++;
    if (alarmHours > 23){
      alarmHours = 0                                                                                                                                                               ;
    }
  }
  if (button2 == true){
    alarmMinutes++;
    if (alarmMinutes > 59){
      alarmMinutes = 0;
    }
  }
  if ((button3 == true) && (alarmFlag == false)){
    alarmFlag = true;
  } 
  else 
    if ((button3 == true) && (alarmFlag == true)){
    alarmFlag = false;
  }
}

/************ voidAlarmDisplay *************/
void voidAlarmDisplay(){
  if ( alarmHours < 10 ){
    lcd.setCursor(5,1);
    lcd.print(0, DEC);
    lcd.setCursor(6,1);
    lcd.print(alarmHours, DEC);
  }
  else {
    lcd.setCursor(5,1);
    lcd.print(alarmHours, DEC);
  }

  lcd.setCursor(7,1);
  lcd.print(":");

  if ( alarmMinutes < 10 ){
    lcd.setCursor(8,1);
    lcd.print(0, DEC);
    lcd.setCursor(9,1);
    lcd.print(alarmMinutes, DEC);
  }
  else {
    lcd.setCursor(8,1);
    lcd.print(alarmMinutes, DEC);
  }

  if ( alarmFlag == true ){
    lcd.setCursor(0,1);
    lcd.print("A");
  }   
}

/************ voidButtonBuzzer *************/
void voidButtonBuzzer(){
  if (butt1 == 1){
    delay(100);
    button1 = true;
  }
  else {
    button1 = false;
  }
  if (butt1 == 2){
    delay(100);
    saveFlag = true;
  }
  else {
    saveFlag = false;
  }

  if (butt2 == 1){
    delay(100);
    button2 = true;
  }
  else {
    button2 = false;
  }
  if (butt3 == 1){
    delay(100);
    button3 = true;
  }
  else {
    button3 = false;
  }
  if (butt4 == 1){
    delay(100);
    button4 = true;
  }
  else {
    button4 = false;
  }
  if (butt5 == 1){
    delay(100);
    button5 = true;
  }
  else {
    button5 = false;
  }
  if (button1 == true){
    analogWrite(BUZZER, 200);
    digitalWrite(LED, HIGH);
    delay(100);
    analogWrite(BUZZER, 0);
    digitalWrite(LED, LOW);
  }
  if (button2 == true){
    analogWrite(BUZZER, 200);
    digitalWrite(LED, HIGH);
    delay(100);
    analogWrite(BUZZER, 0);
    digitalWrite(LED, LOW);
  }
  if (button3 == true){
    analogWrite(BUZZER, 200);
    digitalWrite(LED, HIGH);
    delay(100);
    analogWrite(BUZZER, 0);
    digitalWrite(LED, LOW);
  }
  if (button4 == true){
    analogWrite(BUZZER, 200);
    digitalWrite(LED, HIGH);
    delay(100);
    analogWrite(BUZZER, 0);
    digitalWrite(LED, LOW);
  }
  if (button5 == true){
    analogWrite(BUZZER, 200);
    digitalWrite(LED, HIGH);
    delay(100);
    analogWrite(BUZZER, 0);
    digitalWrite(LED, LOW);
  }
}

/********************** Menu ********************/

void voidMenu(){
  if ( menuFlag == true ){

    switch ( function ){
    case 1: 
      settingFlag = true;
      break;
    case 2:
      examsFlag = true;
    }

  }


}

/*************** Button function ***************/

void buttonFunction(){
  ////////////////////////// Button 1 ///////////////////////
  if (digitalRead(BUTTON1) == false && buttonPush1 == false){
    butt1 = 0;
    //mills = 0;
  }
  if (digitalRead(BUTTON1) == true && buttonPush1 == false){
    mills = millis();
    buttonPush1 = true;
  } 
  if (digitalRead(BUTTON1) == false && buttonPush1 == true){
    subtract = millis() - mills;
    if ( subtract < 300 ){
      butt1 = 1;
    }
    else{
      if ( subtract < 600 ){
        butt1 = 2;
      }
      else{
        if ( subtract < 900 ){
          butt1 = 3;
        } 
        else butt1 = 0;
      } 
    } 
    buttonPush1 = false;
    mills =0 ;
  } 
  ///////////////////// Button 2 ////////////////////////////
  if (digitalRead(BUTTON2) == false && buttonPush2 == false){
    butt2 = 0;
    // mills = 0;
  }
  if (digitalRead(BUTTON2) == true && buttonPush2 == false){
    mills = millis();
    buttonPush2 = true;
  } 
  if (digitalRead(BUTTON2) == false && buttonPush2 == true){
    subtract = millis() - mills;
    if ( subtract < 300 ){
      butt2 = 1;
    }
    else{
      if ( subtract < 600 ){
        butt2 = 2;
      }
      else{
        if ( subtract < 900 ){
          butt2 = 3;
        } 
        else butt2 = 0;
      } 
    } 
    buttonPush2 = false;
    mills = 0;
  } 

  ///////////////////// Button 3 //////////////////////////
  if (digitalRead(BUTTON3) == false && buttonPush3 == false){
    butt3 = 0;
    // mills = 0;
  }
  if (digitalRead(BUTTON3) == true && buttonPush3 == false){
    mills = millis();
    buttonPush3 = true;
  } 
  if (digitalRead(BUTTON3) == false && buttonPush3 == true){
    subtract = millis() - mills;
    if ( subtract < 300 ){
      butt3 = 1;
    }
    else{
      if ( subtract < 600 ){
        butt3 = 2;
      }
      else{
        if ( subtract < 900 ){
          butt3 = 3;
        } 
        else butt3 = 0;
      } 
    } 
    buttonPush3 = false;
    mills = 0;
  } 
  
  ///////////////////// Button 4 //////////////////////////
  if (digitalRead(BUTTON4) == false && buttonPush4 == false){
    butt4 = 0;
    // mills = 0;
  }
  if (digitalRead(BUTTON4) == true && buttonPush4 == false){
    mills = millis();
    buttonPush4 = true;
  } 
  if (digitalRead(BUTTON4) == false && buttonPush4 == true){
    subtract = millis() - mills;
    if ( subtract < 300 ){
      butt4 = 1;
    }
    else{
      if ( subtract < 600 ){
        butt4 = 2;
      }
      else{
        if ( subtract < 900 ){
          butt4 = 3;
        } 
        else butt4 = 0;
      } 
    } 
    buttonPush4 = false;
    mills = 0;
  } 

///////////////////// Button 5 //////////////////////////
  if (digitalRead(BUTTON5) == false && buttonPush5 == false){
    butt5 = 0;
    // mills = 0;
  }
  if (digitalRead(BUTTON5) == true && buttonPush5 == false){
    mills = millis();
    buttonPush5 = true;
  } 
  if (digitalRead(BUTTON5) == false && buttonPush5 == true){
    subtract = millis() - mills;
    if ( subtract < 300 ){
      butt5 = 1;
    }
    else{
      if ( subtract < 600 ){
        butt5 = 2;
      }
      else{
        if ( subtract < 900 ){
          butt5 = 3;
        } 
        else butt5 = 0;
      } 
    } 
    buttonPush5 = false;
    mills = 0;
  } 

}

/***************** Saving to EEPROM ****************/
void voidEEPROM(){
  if ( saveFlag == true ){
    lcd.setCursor(15,1);
    lcd.print("S");
    delay(100);
    EEPROM.write(1, alarmHours);
    EEPROM.write(2, alarmMinutes);
    EEPROM.write(3, alarmFlag);
    saveFlag = false;
  }
}

/*************** Activating alarm ****************/
void voidAlarm(){
  if ( alarmFlag == true ){
    if ( ( alarmHours == now.hour() ) && ( alarmMinutes == now.minute() )) {
      buzz = millis() % 200;
      prevBuzz = millis() % 300;
      if ( buzz != prevBuzz ){
        int volume = (j * 25);
        analogWrite(BUZZER, volume);
        delay(50);
        if ( volume < 255 ){
          j++;
        } 
        else {
          j = 0;
        }
      }
    } 
    else {
      analogWrite(BUZZER, 0);
    }
  }else {
      analogWrite(BUZZER, 0);
    }
}

