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
unsigned long     alarmTimer     = 0;
unsigned long     tmpTimer       = 0;
unsigned long     time           = 0;
unsigned long     timeOut        = 0;
unsigned long     clearTime      = 0;
unsigned long     clearPrevTime  = 0;

/************************* Flags *******************/
boolean     dataRec              = false;      // nincs hasznalatban
boolean     handShake            = false;      // kommunikacio szinkron flag
boolean     commComp             = false;
boolean     dataFlag             = false;
boolean     relayFlag            = false;

/*********************** Declaration of parameters ********************/
char        commandChar;
char        relayComChar;
char        command[]            = "";
char        dataChar[]           = "data";
char        relayChar[]          = "relay";
char        trueChar[]           = "TRUE";
char        falseChar[]          = "FALSE";
char        empty                =' ';

int         j, k, s              = 0;         // for ciklus valtozoja
int         relayComPin          = 0;
boolean     switch1              = false;     // 1. szabadon felhasznalhato kapcsolo
boolean     switch2              = false;     // 2. szabadon felhasznalhato kapcsolo
boolean     switch3              = false;     // 3. szabadon felhasznalhato kapcsolo
byte        handS_sent           = B10101010; // szinkron bajt
byte        handS_recv           = B00000000; // szinkron bajt
byte        handS_buff           = B00000000; // szinkron bajt

int         val                  = 0;
int         dhtHum               = 0;
int         dhtTemp              = 0;
int         light                = 0;
long        bmpPres              = 0;

DateTime    now;
int         hours                = 0;
int         minutes              = 0;
int         seconds              = 0;
unsigned long     mills          = 0;
unsigned long     subtract       = 0;

/******************* Buffers for the transreceiver *****************/
int16_t     txbuff[9];                        // kuldesre varo adatok tombje
int16_t     rxbuff[9];                        // bejovo adatok tombje
byte        relayBuff;                        // kimeno rele vezerlo adatbajt

/************************** Setup *******************************/
void setup()  {
  pinMode(IRQ, INPUT);
  Wire.begin();
  Serial.begin(9600);
  lcdInit();
  rtcInit();
  mirfInit();
  attachInterrupt(0, voidResponse , LOW); /*voidGetPacket*/
}

/************************** Loop *******************************/
void loop()   {
  voidRTC();
  voidLCDData();
  
  if (commComp) {
    for ( k = 0; k < s; k++ ){
    Serial.print(command[k]);  
    }
    commandProc();
    //for ( k = 0; k < s; k++ ){
    //command[s] = empty;
    Serial.println();
    commComp = false;
    s =0;
    //} 
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
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"devin");
  Mirf.setTADDR((byte *)"devou");
  Mirf.config();
}

/********************* Handshake ****************************/
void voidHandShake(){
  Serial.println(" Handshaking method!"); 
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &handS_buff); 
    Serial.println(" Got request!");
    Serial.println(handS_buff, DEC);
    handS_sent = handS_buff;
    Mirf.send((byte *)&handS_sent);
    while(Mirf.isSending()){
    }
    Serial.println(" Respond for handshaking sent!"); 
    if (Mirf.dataReady() == true){
      Mirf.getData((byte *) &handS_recv); 
    }
    if ( handS_sent == handS_recv ) {
      handShake = true;
      Serial.println(" Handshake successful!"); 
    }
    else 
     Serial.println(" No connection between the receiver and the transmitter! "); 
  }
  else Serial.println(" No request!");
  
}

/******************** Generating packet ********************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = 0;
    //Serial.print("0. bajt: ");
    //Serial.println(txbuff[0], BIN);
    txbuff[1] = 0;
    //Serial.print("1. bajt: ");
    //Serial.println(txbuff[1], BIN);
    txbuff[2] = 0;
    //Serial.print("2. bajt: ");
    //Serial.println(txbuff[2], BIN);
    txbuff[3] = 0;
    //Serial.print("3. bajt: ");
    //Serial.println(txbuff[3], BIN);
    txbuff[4] = 0;
    //Serial.print("4. bajt: ");
    //Serial.println(txbuff[4], BIN);
    txbuff[5] = 0;
    //Serial.print("5. bajt: ");
    //Serial.println(txbuff[5], BIN);
    txbuff[6] = 0;
    //Serial.print("6. bajt: ");
    //Serial.println(txbuff[6], BIN);
    txbuff[7] = 0;
    //Serial.print("7. bajt: ");
    //Serial.println(txbuff[7], BIN);
    txbuff[8] = 0;
    //Serial.print("8. bajt: ");
    //Serial.println(txbuff[8], BIN);
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    voidPacketGen();
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    //Serial.println("Packet sent...");
}

/****************** Filling up server data *******************/
void voidGetPacket(){
  //Serial.println("Buli van!");
  //detachInterrupt(0);
  //while( !Mirf.dataReady() ){
  //}
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
    for ( j = 0; j < 9; j++ ){
      //Serial.print(j);
      //Serial.print(". bajt: ");
      //Serial.println(rxbuff[j], DEC);
    }
    dhtHum = rxbuff[0];
    dhtTemp = rxbuff[1];
    val = rxbuff[2];
    light = ( val / 1024 ) * 100; 
    bmpPres = rxbuff[3];
    //Serial.println("Packet receiver: Packet is got! ");
  }
}
/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  Mirf.send((byte *)&relayBuff);
  while(Mirf.isSending()){
  }
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
    lcd.print(light, DEC);
  
    lcd.setCursor(9,1);
    lcd.print("P: ");
    lcd.setCursor(12,1);
    lcd.print(dhtHum, DEC);
    lcd.setCursor(15,1);
    lcd.print("%");
  }
  if ( now.second() > 29 && now.second() < 60 ){
    lcd.setCursor(0,1);
    lcd.print("H: ");
    lcd.setCursor(3,1);
    lcd.print(dhtTemp, DEC);
    lcd.setCursor(5,1);
    lcd.print("C");
  
    lcd.setCursor(7,1);
    lcd.print("Ny: ");
    lcd.setCursor(11,1);
    lcd.print(bmpPres, DEC);
    lcd.setCursor(15,1);
    lcd.print("P");
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
  Serial.println("4. kar a parancsbol");
  Serial.println(command[3]);
  Serial.println("1. kar a parancsbol &");
  Serial.println(&command[0]);
  Serial.println("d betu");  
  Serial.println("d"); 
  Serial.println("0. kar a data");
  Serial.println(dataChar[0]);
  Serial.println("0. kar a data &");
  Serial.println(&dataChar[0]);
  Serial.println("0. kar a relay &");
  Serial.println(&relayChar[0]);
  if ( (&command[0] == "data")) {
    dataFlag = true;
    Serial.println("zsir");  
  }
  else {
    dataFlag = false;
    Serial.println(" nem zsir"); 
  }  
  if ( command[0] == relayChar[0] && command[1] == relayChar[1] && command[2] == relayChar[2] && command[3] == relayChar[3] && command[4] == relayChar[4] ) 
    relayFlag = true;
  else
    relayFlag = false;
    
  if ( relayFlag == true ){
    relayComPin = atoi(&command[6]) + 1;
    if ( command[10] == trueChar[10]  && command[11] == trueChar[11] && command[12] == trueChar[12] && command[13] == trueChar[13] ) 
      bitSet(relayBuff, relayComPin);
    if ( command[10] == falseChar[10]  && command[11] == falseChar[11] && command[12] == falseChar[12] && command[13] == falseChar[13] && command[14] == falseChar[14] ) 
      bitClear(relayBuff, relayComPin);
  }
}
