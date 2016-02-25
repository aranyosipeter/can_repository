// This headers for the I2C connections to the display and the RTC
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "header.h"
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

//Defining relay pins
#define relay1  5
#define relay2  6
#define relay3  9
#define relay4  10
#define relay5  16
int relayPin[]={
  5, 6, 9, 10, 16, 17};

// LCD display connection
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

// RTC settings
RTC_DS1307 RTC;

/******************** Timer values *****************/

unsigned long alarmTimer = 0;
unsigned long tmpTimer = 0;
unsigned long time = 0;
unsigned long timeOut = 0;
unsigned long clearTime = 0;
unsigned long clearPrevTime = 0;

/************************* Flags *******************/

boolean alarmInit = false;
boolean sirenInit = false;
boolean timerFlag = false;
boolean codeValid;
boolean flagAlarm;
boolean alarmReq=false;
boolean disAlarmReq=false;
boolean flagHome;
boolean receiveP = false;

boolean checkingPass = false;
boolean inputPass = false;
boolean agreePass = false;

/*********************** Codes ********************/

char inputCode[3];
char code[]={
  '1', '2', '3','4'} 
;

int j, k;
int seconds = 0;
int temp = 0;
int backLightOn = 0;
int backLightTimer = 0;
int clientq = 0;
int pir = 0;
int alarm = 0;
int codeNum = 0;
int alarmCount = 0;
int signalCount = 0;
int pins[]={
  6,9,10};

int sendTime = 0;
int prevTime = 0;

const int numRows = 4;       // number of rows in the keypad
const int numCols = 4;       // number of columns
const int debounceTime = 10; // number of milliseconds for switch to be stable

// Declarations for the IIC keyboard //
byte sendByte = B11111111;
byte recByte = B00000000;

// Node strukturak deklaralasa  //
node alarmsys = {
  "serv1", B11111111, B00000010, B00001010, true};

node client0 = {
  "dev01", B10101010, B00000000, B00001010, false};
node client1 = {
  "dev01", B10101010, B00000001, B00001010, false};
node client2 = {
  "dev01", B10101010, B00000001, B00001010, false};

nodepin devPin;
nodepin clientPin[3];

/******************* Buffers for the transreceiver *****************/

byte txbuff[16];
byte rxbuff[16];

/*********************** Services of the nodes ********************/

int servNum=0;
byte services[3] = { 
  B11001100, B00110011, B10011001 };

/***************** Defining default keyboard's keys **************/

const char keymap[numRows][numCols] = {
  { 
    '1', '2', '3', 'A'                     }
  ,
  { 
    '4', '5', '6', 'B'                     }
  ,
  { 
    '7', '8', '9', 'C'                     }
  ,
  { 
    '*', '0', '#', 'D'                     }
};

const int rowPins[numRows] = { 
  2, 3, 5, 6 }; 
const int colPins[numCols] = { 
  9, 10, 16, 17 };    


/***************** The setup of the program  ******************/

void setup()  
{
  Wire.begin();
  Serial.begin(9600);
  lcdInit();
  rtcInit();
  mirfInit();
  //keyInit();
  iicKeyInit();
  servInit();
  relayInit();
  //codeErase();
}

/*************************** The Loop *************************/

void loop()   {  
  time = millis();
  sendTime = time/180000;

  voidRTC();
  voidMenu();
  if ( Mirf.dataReady() == true ){
    Mirf.setRADDR((byte *)"serv1");
    Mirf.setTADDR((byte *)"dev01");
    voidGetPacket();
    Serial.println("Got packet!");
    readAlarmBit(&client0);
    reqLifeSignal();  
    Serial.print("Packet is sent!");
  }

  if (sendTime != prevTime){
    Mirf.setRADDR((byte *)"serv1");
    Mirf.setTADDR((byte *)"dev01");
    reqLifeSignal();
    timeOut = millis();
    while(!Mirf.dataReady() && time - timeOut < 3000){
    }
    voidRecAck();
    reqLifeSignal();
    if ( receiveP == true ){
      signalCount++;
    }
    prevTime=sendTime;
    receiveP = false;
  }
  clearScreen();
  useSiren();
  //voidDataReceive();
  //voidKey();
  //voidConfigPins(&dev, &devPin);
}

/*********************** Initalizing relays *******************/

void relayInit(){
  int i;
  for ( i=0; i<8; i++ ){
    pinMode(relayPin[i], OUTPUT);
  } 
  for ( i=0; i<8; i++){
    digitalWrite(relayPin[i], HIGH);
  }
}

/************************ Initalizing flags *******************/

void servInit(){
  codeValid=true;
  flagAlarm=false;
  flagHome=false;
  Mirf.setRADDR((byte *)"serv1");
  Mirf.setTADDR((byte *)"dev01");
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
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 32;
  Mirf.config();
}

/****************** Initalizing keyboard *******************/

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

/**************** Initalizing IIC keyboard *****************/

void iicKeyInit(){
  byte initK = B11111111;
  Wire.beginTransmission(keyboard);
  Wire.write(initK);
  Wire.endTransmission();
}

/********** Getting keys from the keyboard ****************/

char getKey(){
  char key = 0;                                  
  for(int column = 0; column < numCols; column++){
    digitalWrite(colPins[column],LOW);         
    for(int row = 0; row < numRows; row++){     
      if(digitalRead(rowPins[row]) == LOW){     
        delay(debounceTime);                   
        while(digitalRead(rowPins[row]) == LOW);                                  
        key = keymap[row][column];             
      }
    }
    digitalWrite(colPins[column],HIGH);     
  }
  return key;  
}

/********** Getting keys from the IIC keyboard ***********/

char getIICKey(){
  char key = 0;                                  
  for(int column = 3; column >= 0; column--){
    bitClear(sendByte, column);
    Wire.beginTransmission(keyboard);
    Wire.write(sendByte);
    Wire.endTransmission();

    for(int row = 7; row >= 4; row--){     
      Wire.requestFrom(keyboard, 1);
      if(Wire.available())
      {
        recByte = Wire.read();
        if(bitRead(recByte, row) == 0){
          key = keymap[3-(row-4)][3-column];
        }
      }

    }
    bitSet(sendByte, column);
    Wire.beginTransmission(keyboard);
    Wire.write(sendByte);
    Wire.endTransmission(); 
  }
  return key;  
}

/****************** Clearing the screen ******************/

void clearScreen(){
  DateTime now = RTC.now();
  clearTime = millis() / 1000;
   if( clearTime != clearPrevTime){
    lcd.clear();
    clearPrevTime = clearTime;
  }   
 
  /*if(now.second()!=seconds){
    lcd.clear();
    seconds=now.second();
    //backLightTimer++;
  } */     
}

/****************** Menu for the program******************/

void voidMenu(){
  char key = getIICKey();
  if ( key!=0 ){
    //Serial.println(key);
  }
  if( key != 0 ){      
    if( key == 'A' ){
      alarmReq = true;
    }
    if( key == 'D' ){
      disAlarmReq = true;
    }   
  }
  voidAlarm();
}

/******************** Alarm the system ******************/

void voidAlarm(){
  Serial.print("alarmreq: ");
  Serial.println(alarmReq);
  
  Serial.print("disalarmreq: ");
  Serial.println(disAlarmReq);
  
  Serial.print("flagalarm: ");
  Serial.println(flagAlarm);
  
  Serial.print("inputPas: ");
  Serial.println(inputPass);
  
  Serial.print("agreePass: ");
  Serial.println(agreePass);
  
  Serial.print("checkingPass: ");
  Serial.println(checkingPass);
  delay(250);
  
  
  if(alarmReq == true || disAlarmReq == true){
    alarmTimer = millis();

    if (flagAlarm == true && alarmReq == true){
      lcd.setCursor(0,6);
      lcd.print("ARMED!");
      delay(50);
      alarmReq = false;
    }

    if (flagAlarm == false && disAlarmReq == true){
      lcd.setCursor(0,6);
      lcd.print("DISARMED!");
      delay(50);
      disAlarmReq = false;
    }

    if( flagAlarm == false && alarmReq == true ){
      insertPass();
      if (agreePass == true){
        inputPass = false;
        if (timerFlag == false){
          tmpTimer = alarmTimer;
          timerFlag = true;
        }  
        if (alarmTimer - tmpTimer < 10000){
          lcd.setCursor(0,1);
          lcd.print("Arming the system.... "); 
        }
        if (alarmTimer - tmpTimer > 10000){
          lcd.setCursor(0,1);
          lcd.print("System is armed! ");
          flagAlarm = true;
          alarmReq = false;
          timerFlag = false;
          agreePass = false;
        }
      }
      else if (( agreePass == false ) && ( inputPass == true ) && ( checkingPass == true )){
        lcd.setCursor(0,1);
        lcd.print("Code is invalid"); 
        alarmReq = false;
        inputPass = false;
      }
    }

    if( flagAlarm == true && disAlarmReq == true ){
      insertPass();
      if (agreePass == true){
        inputPass = false;
        if (timerFlag == false){
          tmpTimer = alarmTimer;
          timerFlag = true;
        }  
        if (alarmTimer - tmpTimer < 10000){
          lcd.setCursor(0,1);
          lcd.print("Disarming the system.... "); 
        }
        if (alarmTimer - tmpTimer > 10000){
          lcd.setCursor(0,1);
          lcd.print("System is disarmed! ");
          flagAlarm = false;
          disAlarmReq = false;
          timerFlag = false;
          agreePass = false;
          sirenInit = false;
          alarmCount = 0;
        }  
      }
      else if (( agreePass == false ) && ( inputPass == true ) && ( checkingPass == true )){
        lcd.setCursor(0,1);
        lcd.print("Code is invalid"); 
        disAlarmReq = false;
        inputPass = false;
      }
    }

    //lcd.noCursor();
  }
}

/***************** Insert the password *****************/

int insertPass(){
  Serial.println(" ");
  Serial.print("inPUtpass: ");
  Serial.println(inputPass);
  Serial.print("j: ");
  Serial.println(j);
  Serial.print("Jelszo:");
  for (int h = 0; h <= j; h++){
    Serial.print(inputCode[h]);
  }
  Serial.println("");
  
  
  if (inputPass == false){
    lcd.cursor();
    lcd.setCursor(0,1);
    lcd.print("Password: ");
    for (int h = 0; h <= j; h++){
       lcd.setCursor(10+h,1);
      lcd.print(inputCode[h]);
    }
    if ( j < 4 ){
      char codeKey = getIICKey();
      if( codeKey != 0 && codeKey != 'A' && codeKey != 'B' && codeKey != 'C' && codeKey != 'D'){
        
        if( codeKey == '*'){
          j--;
        }
        else{
          if( codeKey == '#'){
            alarmReq = false;
            disAlarmReq = false;
            return 0;
          }
          else{
            inputCode[j]=codeKey;
            j++;
          }
        }
      }
    }
    if ( j == 4 ){
      inputPass = true;
      checkingPass = true;
    } 
  }
  if ( ( inputPass == true ) && ( checkingPass == true )){ 
    for(int j=0; j<3; j++){
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
        inputPass = false;
        checkingPass = false;
        return 1;
      }
    }

  }
  if (( codeValid == true ) && ( inputPass == true ) && ( checkingPass == true )){
    agreePass = true;
    inputPass = false;
    checkingPass = false;
    for (j = 0; j <= 3 ; j++){
      inputCode[j] = ' ';
    }
    j = 0;
  }
  else if (( codeValid == false ) && ( inputPass == true ) && ( checkingPass == true )) {
    agreePass = false;
    inputPass = false;
    checkingPass = false;
    for (j = 0; j <= 3 ; j++){
      inputCode[j] = ' ';
    }
    j = 0;
  }
}

/**************** Processing alarm bit *****************/

void readAlarmBit(struct node *alarmNode){

  alarmInit = bitRead((*alarmNode).state, 0);
  if (flagAlarm == true && alarmInit == true ){
    lcd.setCursor(0,1);
    lcd.print("Alarming! ");
    alarmInit = false;
    sirenInit = true;
    alarmCount++;
  }
}

/********************* Using Siren *********************/

void useSiren(){
  if ( sirenInit == true ){
    digitalWrite(relay1, LOW);
  }
  if ( sirenInit == false ){
    digitalWrite(relay1, HIGH);
  }
}

/********************* Actual time *********************/

void voidRTC(){
  if ( alarmReq == false || disAlarmReq == false ){
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
    if ( alarmCount > 0 ){
      lcd.setCursor(9,1);
      lcd.print("A: ");
      lcd.print(alarmCount, DEC);
    }
    if ( signalCount > 0 ){
      lcd.setCursor(1,1);
      lcd.print("S: ");
      lcd.print(signalCount, DEC);
    }
    /* if(now.second()!=seconds){
     lcd.clear();
     seconds=now.second();
     backLightTimer++;
     }*/
  }
}


/*************** Configuring I/O pins ******************/

void voidConfigPins(struct node *device, struct nodepin *devicePin){
  //Serial.println("Config I/O pins");
  DateTime serial=RTC.now();
  int sec=0;
  int i;

  if(serial.second()!=sec){
    for(i=0;i<3;i++){

      //Serial.print("Eredeti pin: ");
      //Serial.println((*devicePin).pins[i], DEC);
      int bit=bitRead((*device).state,(i+1));
      if(bit==0){
        //Serial.print("Hanyadik bit: ");
        //Serial.println(bitRead((*device).state,(i+1)), DEC);

        pinMode((*devicePin).pins[i], INPUT);

        //Serial.print("Hanyadik pin: ");
        //Serial.println((*devicePin).pins[i], DEC);

        (*devicePin).mode[i]=0;
      }

      if(bit==1){
        //Serial.print("Hanyadik bit: ");
        //Serial.println(bitRead((*device).state,(i+1)), DEC);
        pinMode((*devicePin).pins[i], OUTPUT);
        //Serial.print("Hanyadik pin: ");
        //Serial.println((*devicePin).pins[i], DEC);
        (*devicePin).mode[i]=1;
      }
    }
    sec=serial.second();  
  }
} 

/******************** Generating packet ********************/

void voidPacketGen(int serv){
  byte checkSum;
  // Creating package for the registration
  if ( serv == 0 ){
    for (int i=0; i<6; i++){
      txbuff[i] = client0.address[i];
    }
    Serial.print("0. bájt: ");
    Serial.println(txbuff[0], BIN);
    Serial.print("1. bájt: ");
    Serial.println(txbuff[1], BIN);
    Serial.print("2. bájt: ");
    Serial.println(txbuff[2], BIN);
    Serial.print("3. bájt: ");
    Serial.println(txbuff[3], BIN);
    Serial.print("4. bájt: ");
    Serial.println(txbuff[4], BIN);
    Serial.print("5. bájt: ");
    Serial.println(txbuff[5], BIN);

    txbuff[6] = services[0];
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = client0.id;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = client0.state;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = client0.role;
    Serial.print("9. bájt: ");
    Serial.println(txbuff[9], BIN);
    txbuff[10] = B00000000;
    txbuff[11] = B00000000;
    txbuff[12] = B00000000;
    txbuff[13] = B00000000;
    txbuff[14] = B00000000;
    for (int k=0; k<15; k++){
      checkSum += txbuff[k];
    }
    txbuff[15] = checkSum; 
    Serial.println("Packet for registration is created");
  }

  // Creating package for deleting the registration info
  if ( serv == 1 ){
    for (int i=0; i<6; i++){
      txbuff[i] = B00000000;
    }
    Serial.print("0. bájt: ");
    Serial.println(txbuff[0], BIN);
    Serial.print("1. bájt: ");
    Serial.println(txbuff[1], BIN);
    Serial.print("2. bájt: ");
    Serial.println(txbuff[2], BIN);
    Serial.print("3. bájt: ");
    Serial.println(txbuff[3], BIN);
    Serial.print("4. bájt: ");
    Serial.println(txbuff[4], BIN);
    Serial.print("5. bájt: ");
    Serial.println(txbuff[5], BIN);

    txbuff[6] = services[1];
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = B00000000;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = B00000000;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = B00000000;
    Serial.print("9. bájt: ");
    Serial.println(txbuff[9], BIN);
    txbuff[10] = B00000000;
    txbuff[11] = B00000000;
    txbuff[12] = B00000000;
    txbuff[13] = B00000000;
    txbuff[14] = B00000000;
    for (int k=0; k<15; k++){
      checkSum += txbuff[k];
    }
    txbuff[15] = checkSum; 
    Serial.println("Packet for exit is created");
  }

  // Creating package for sending lifesignals
  if ( serv == 2 ){
    for (int i=0; i<6; i++){
      txbuff[i] = client0.address[i];
    }
    Serial.print("0. bájt: ");
    Serial.println(txbuff[0], BIN);
    Serial.print("1. bájt: ");
    Serial.println(txbuff[1], BIN);
    Serial.print("2. bájt: ");
    Serial.println(txbuff[2], BIN);
    Serial.print("3. bájt: ");
    Serial.println(txbuff[3], BIN);
    Serial.print("4. bájt: ");
    Serial.println(txbuff[4], BIN);
    Serial.print("5. bájt: ");
    Serial.println(txbuff[5], BIN);

    txbuff[6] = services[2];
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = client0.id;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = client0.state;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = client0.role;
    Serial.print("9. bájt: ");
    Serial.println(txbuff[9], BIN);
    txbuff[10] = B00000000;
    txbuff[11] = B00000000;
    txbuff[12] = B00000000;
    txbuff[13] = B00000000;
    txbuff[14] = B00000000;
    for (int k=0; k<15; k++){
      checkSum += txbuff[k];
    }
    txbuff[15] = checkSum; 
    Serial.println("Packet for lifesignal is created");
  }

}

/******************** Request for fifeSignal ********************/

void reqLifeSignal(){
  servNum = 2;
  voidPacketGen(servNum);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Lifesignal request sent...");
}

/******************** Request for registration ********************/

void reqRegistration(){
  servNum = 0;
  voidPacketGen(servNum);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Registration request sent...");
}

/******************** Request for deleteSignal ********************/

void reqDeleteSignal(){
  servNum = 1;
  voidPacketGen(servNum);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Deletesignal request sent...");
}


/****************** Filling up server data *******************/

void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);  
    receiveP = true;
    client0.serv = rxbuff[6];
    client0.id = rxbuff[7];
    client0.state = rxbuff[8];
    client0.role = rxbuff[9];
    Serial.println("Packet receiver: Packet is got! ");
    Serial.print("6. bájt: ");
    Serial.println(rxbuff[6]);
    Serial.print("7. bájt: ");
    Serial.println(rxbuff[7]);
    Serial.print("8. bájt: ");
    Serial.println(rxbuff[8]);
    Serial.print("9. bájt: ");
    Serial.println(rxbuff[9]);
  }
}

void voidRecAck(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);  
    receiveP = true;
    client0.serv = rxbuff[6];
    client0.id = rxbuff[7];
    client0.state = rxbuff[8];
    client0.role = rxbuff[9];
    Serial.println("Acknowledgement receive! ");
    Serial.print("6. bájt: ");
    Serial.println(rxbuff[6]);
    Serial.print("7. bájt: ");
    Serial.println(rxbuff[7]);
    Serial.print("8. bájt: ");
    Serial.println(rxbuff[8]);
    Serial.print("9. bájt: ");
    Serial.println(rxbuff[9]);
  }
}









