// This headers for the I2C connections to the display and the RTC
#include <Wire.h>
#include <LCD.h>
#include "MyStructs.h"
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
int j;
int clientq=0;
int pir=0;
int alarm=0;
int pins[]={
  6,9,10};

const int numRows = 4;       // number of rows in the keypad
const int numCols = 4;       // number of columns
const int debounceTime = 10; // number of milliseconds for switch to be stable
  
// Node strukturak deklaralasa  //
node dev = {
  B11111111, B00000010, B00001010, false};
node client[4];

nodepin devPin;
nodepin clientPin[3];

// keymap defines the character returned when the corresponding key is pressed
const char keymap[numRows][numCols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// this array determines the pins used for rows and columns
const int rowPins[numRows] = { 2, 3, 5, 6 }; // Rows 0 through 3
const int colPins[numCols] = { 9, 10, 16, 17 };    // Columns 0 through 3

boolean codeValid;
boolean flagAlarm;
boolean flagHome;
char inputCode[3];
char code[3];


void setup()  
{
  Wire.begin();
  Serial.begin(9600);
  lcdInit();
  rtcInit();
  mirfInit();
  keyInit();
  servInit();
}

void loop()   
{
  voidRTC();
  //voidDataReceive();
  //voidKey();
  voidMenu();
  voidConfigPins(&dev, &devPin);
}

void servInit(){
  codeValid=true;
  flagAlarm=false;
  flagHome=false;
  // Elore beallitott kod //
  code[0]='1';
  code[1]='2';
  code[2]='3';
  code[3]='4';
}

void lcdInit(){
  lcd.begin (16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
}

void rtcInit(){
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__));
}

void mirfInit(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 32;
  Mirf.config();
}

void keyInit(){
  for (int row = 0; row < numRows; row++){
    pinMode(rowPins[row],INPUT);       
    digitalWrite(rowPins[row],HIGH);   
  }
  for (int column = 0; column < numCols; column++){
    pinMode(colPins[column],OUTPUT);     
    digitalWrite(colPins[column],HIGH); 
  }
}

void voidMenu(){
  voidRTC();
  char key = getKey();
  if ( key!=0 ){
  Serial.println(key);
  }
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
  lcd.setCursor(0,1);
  lcd.print("PASSWORD: ");
  
    while(i<4){
      char codeKey = getKey();
      if( codeKey != 0 && codeKey != 'A' && codeKey != 'B' && codeKey != 'C' && codeKey != 'D'){
        if( codeKey == '*'){
          i--;
           lcd.cursor();
           lcd.setCursor(10+i,1);
        }
        else{
          if( codeKey == '#'){
            break;
          }
          else{
            inputCode[i]=codeKey;
            lcd.cursor();
            lcd.setCursor(10+i,1);
            lcd.print(inputCode[i]);
            i++;
            
          }
        } 
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
  lcd.setCursor(0,0);
  lcd.print(now.year()-2000, DEC);
  lcd.print("/");
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
    for(int row = 0; row < numRows; row++){     // Scan all rows for 
        // a key press.
    
      if(digitalRead(rowPins[row]) == LOW){     // Is a key pressed?
      
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



void voidConfigPins(struct node *device, struct nodepin *devicePin){
  int i;
  for(i=0;i<3;i++){

    Serial.print("Eredeti pin: ");
    Serial.println((*devicePin).pins[i], DEC);
    int bit=bitRead((*device).state,(i+1));
    if(bit==0){
      Serial.print("Hanyadik bit: ");
      Serial.println(bitRead((*device).state,(i+1)), DEC);

      pinMode((*devicePin).pins[i], INPUT);

      Serial.print("Hanyadik pin: ");
      Serial.println((*devicePin).pins[i], DEC);

      (*devicePin).mode[i]=0;
    }

    if(bit==1){
      Serial.print("Hanyadik bit: ");
      Serial.println(bitRead((*device).state,(i+1)), DEC);
      pinMode((*devicePin).pins[i], OUTPUT);
      Serial.print("Hanyadik pin: ");
      Serial.println((*devicePin).pins[i], DEC);
      (*devicePin).mode[i]=1;
    }
  }
  delay(1000);
} 

