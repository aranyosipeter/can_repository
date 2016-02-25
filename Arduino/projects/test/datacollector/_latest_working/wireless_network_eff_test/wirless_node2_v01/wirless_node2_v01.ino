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
#define  LED_OFF  0
#define  LED_ON  1

// LCD display connection
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

// RTC settings
RTC_DS1307 RTC;

#define IRQ 2


/******************** Timer values *****************/

unsigned long      alarmTimer              = 0;
unsigned long      tmpTimer                = 0;
unsigned long      time                    = 0;
unsigned long      timeOut                 = 0;
unsigned long      clearTime               = 0;
unsigned long      clearPrevTime           = 0;

/************************* Flags *******************/

boolean             dataRec                = false;      // nincs hasznalatban
boolean             handShake              = false;      // kommunikacio szinkron flag
boolean             commComp               = false;
boolean             dataFlag               = false;
boolean             relayFlag              = false;
boolean             sendFlag               = false;
boolean             serialDataFlag         = false;
  
/*********************** Declaration of parameters ********************/

char                commandChar;
char                relayComChar;
char                command[16]            = "";
char                test[16]               = "data";
char                dataChar[5]            = "data";
char                relayChar[6]           = "relay";
volatile char       trueChar[5]            = "TRUE";
volatile char       falseChar[6]           = "FALSE";
volatile char       empty                  = ' ';

int                 j, k, s                = 0;         // for ciklus valtozoja
int                 relayComPin            = 0;

boolean             switch1                = false;     // 1. szabadon felhasznalhato kapcsolo
boolean             switch2                = false;     // 2. szabadon felhasznalhato kapcsolo
boolean             switch3                = false;     // 3. szabadon felhasznalhato kapcsolo
byte                handS_sent             = B10101010; // szinkron bajt
byte                handS_recv             = B00000000; // szinkron bajt
byte                handS_buff             = B00000000; // szinkron bajt

byte                relayBuff              = B00000000; // kimeno rele vezerlo adatbajt
int                 val                    = 0;
int                 dhtHum                 = 0;
int                 dhtTemp                = 0;
int                 light                  = 0;
int                 bmpTemp                = 0;
int                 bmpPresKPa             = 0;

DateTime            now;
int                 hours                  = 0;
int                 minutes                = 0;
int                 seconds                = 0;
unsigned long       mills                  = 0;
unsigned long       subtract               = 0;
/******************** Communication of nodes ***********************/

byte                devID                  = B00000101;
byte                commandWl              = B00000000;
byte                receiveCommand         = B00000000;
byte                receiveID              = B00000000;

/******************* Buffers for the transreceiver *****************/

byte                txbuff[16];                        // kuldesre varo adatok tombje
byte                rxbuff[16];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  pinMode(IRQ, INPUT);
  Wire.begin();
  Serial.begin(9600);
  lcdInit();
  rtcInit();
  mirfInit();
  attachInterrupt(0, voidResponse, LOW);
}

/************************** Loop *******************************/
void loop()   {
  voidRTC();
  voidLCDData();
  voidProcess();
  voidSerialData();
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

/****************** Initalizing LCD screen ********************/
void lcdInit(){
  lcd.begin (16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);
}

/******************** Initalizing RTC ************************/
void rtcInit(){
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__));
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 82;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"node2");
  Mirf.setTADDR((byte *)"serwl");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = devID;
    txbuff[1] = commandWl;
    txbuff[2] = 0;
    txbuff[3] = 0;
    txbuff[4] = 0;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    txbuff[8] = 0;
    txbuff[9] = 0;
    txbuff[10] = 0;
    txbuff[11] = 0;
    txbuff[12] = 0;
    txbuff[13] = 0;
    txbuff[14] = 0;
    txbuff[15] = 0;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
}

/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
  }
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
  dataRec = true;
}

/******************* Processing data *******************/
void voidProcess(){
  receiveID = rxbuff[0];
  receiveCommand = rxbuff[1];
  dhtTemp = rxbuff[2];
  dhtHum = rxbuff[3];
  bmpTemp = rxbuff[4];
  bmpPresKPa = rxbuff[5];
  val = rxbuff[6];
  //light =  val / 10.24; 
}

/********************* Actual time *********************/
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

/******************************** Datas on LCD ***************************/
void voidLCDData(){
  if ( now.second() >= 0 && now.second() < 30 ){
    lcd.setCursor(0,1);
    lcd.print("F: ");
    lcd.setCursor(3,1);
    lcd.print(val, DEC);
    lcd.setCursor(5,1);
    lcd.print("%");
  
    lcd.setCursor(7,1);
    lcd.print("Ny:");
    lcd.setCursor(10,1);
    lcd.print(bmpPresKPa, DEC);
    lcd.setCursor(13,1);
    lcd.print("kPa");
    
  }
  if ( now.second() > 29 && now.second() < 60 ){
    lcd.setCursor(0,1);
    lcd.print("H: ");
    lcd.setCursor(3,1);
    lcd.print(bmpTemp, DEC);
    lcd.setCursor(6,1);
    lcd.print("C");
  
    lcd.setCursor(9,1);
    lcd.print("P: ");
    lcd.setCursor(12,1);
    lcd.print(dhtHum, DEC);
    lcd.setCursor(15,1);
    lcd.print("%");
  }
 /* if ( now.second() == 44 ){
    lcd.setCursor(0,1);
    lcd.print("F: ");
    lcd.setCursor(3,1);
    lcd.print(val, DEC);
  
    lcd.setCursor(0,);
    lcd.print("P: ");
    lcd.setCursor(3,1);
    lcd.print(val, DEC);
  }
  if ( now.second() == 59 ){
    lcd.setCursor(0,1);
    lcd.print("F: ");
    lcd.setCursor(3,1);
    lcd.print(val, DEC);
  
    lcd.setCursor(0,);
    lcd.print("P: ");
    lcd.setCursor(3,1);
    lcd.print(val, DEC);
  }*/
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
  //Serial.println("teszteles jelleggel/ sajnos csak igy mukodik");
  //Serial.println(&test[0]);
  //Serial.println("dataChar[]");
  //Serial.println(&dataChar[0]);
  //Serial.println("relayChar[]");
  //Serial.println(&relayChar[0]);
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
void voidSerialData(){
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
    Serial.print(bmpPresKPa, DEC);
    Serial.println(" kPa");
    dataRec = false;  
  }
  
}
