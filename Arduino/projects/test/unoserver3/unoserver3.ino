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

const int numRows = 4;       // number of rows in the keypad
const int numCols = 4;       // number of columns
const int debounceTime = 10; // number of milliseconds for switch to be stable

// keymap defines the character returned when the corresponding key is pressed
const char keymap[numRows][numCols] = {
  { '1', '2', '3', 'A'  } ,
  { '4', '5', '6', 'B'  } ,
  { '7', '8', '9', 'C'  } ,
  { '*', '0', '#', 'D'  }
};

// this array determines the pins used for rows and columns
const int rowPins[numRows] = { 2, 3, 15, 5 }; // Rows 0 through 3
const int colPins[numCols] = { 6, 9, 10, 14};    // Columns 0 through 3

boolean codeValid;
boolean flagAlarm;
boolean flagHome;
char inputCode[3];
char code[3];


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
  
  codeValid=true;
  flagAlarm=false;
  flagHome=false;
  
  code[0]='1';
  code[1]='2';
  code[2]='3';
  code[3]='4';
  
  for (int row = 0; row < numRows; row++){
    
    pinMode(rowPins[row],INPUT);       // Set row pins as input
    digitalWrite(rowPins[row],HIGH);   // turn on Pull-ups
  }
  for (int column = 0; column < numCols; column++)
  {
    pinMode(colPins[column],OUTPUT);     // Set column pins as outputs 
                                         // for writing
    digitalWrite(colPins[column],HIGH);  // Make all columns inactive
  }
  
}

void loop()   
{
  voidRTC();
  voidDataReceive();
  voidKey();
  voidMenu();
}

void voidMenu(){
    char key = getKey();
    if( key != 0) {       // if the character is not 0 then 
                        // it's a valid key press
    if( key=='A' ){
      voidAlarm();
    }
    if( key=='D' ){
      voidAlarm();

    }   
}
}
void voidAlarm(){
  int i=0;
  for (int j=0; j<4; j++){
    inputCode[j]=0;  
  }
   if( flagAlarm==false ){
       while(i<4){
       char codeKey = getKey();
       if( codeKey != 0 && codeKey != 'A' && codeKey != 'B' && codeKey != 'C' && codeKey != 'D'){
         inputCode[i]=codeKey;
         i++;
       }
       if( codeKey == '*'){
         i--;
       }
       if( codeKey == '#'){
         break;
       }
     }
     
     
    for(int j=0; j<4; j++){
      if (code[j]==inputCode[j]){
        if(codeValid==false){
          codeValid=false;
        }
        else{
          codeValid=true;
        }
      }
    if (code[j]!=inputCode[j]){
       codeValid=false; 
    }
  }
  
  if( flagAlarm==false && codeValid==true){
      voidCountdown();
      flagAlarm=true;
    }
  
  
  
    
  }
}

void voidCountdown(){
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

void voidKey(){
  char key = getKey();
  if( key != 0) {       // if the character is not 0 then 
                        // it's a valid key press
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Got key ");
    lcd.setCursor(4,1);
    lcd.print(key);
    delay(200);
  }
}

// returns with the key pressed, or 0 if no key is pressed

char getKey(){
  char key = 0;                                  // 0 indicates no key pressed

  for(int column = 0; column < numCols; column++){
    
    digitalWrite(colPins[column],LOW);         // Activate the current column.
    for(int row = 0; row < numRows; row++)     // Scan all rows for 
                                               // a key press.
    {
      if(digitalRead(rowPins[row]) == LOW)     // Is a key pressed?
      {
        delay(debounceTime);                   // debounce
        while(digitalRead(rowPins[row]) == LOW)
            ;                                  // wait for key to be released
        key = keymap[row][column];             // Remember which key 
                                               // was pressed.
      }
    }
    digitalWrite(colPins[column],HIGH);     // De-activate the current column.
  }
  return key;  // returns the key pressed or 0 if none
}

