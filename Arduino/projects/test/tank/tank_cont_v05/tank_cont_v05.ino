#include <Wire.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define LEFT_CONT 16
#define RIGHT_CONT 17
#define LEFT_ACC_BUT 1
#define LEFT_DCC_BUT 0
#define RIGHT_ACC_BUT 14
#define RIGHT_DCC_BUT 15

#define LEFT_ACC_LED 3 
#define LEFT_DCC_LED 5
#define LEFT_STD_LED 4
#define RIGHT_ACC_LED 6 
#define RIGHT_DCC_LED 10
#define RIGHT_STD_LED 9

/************************* Flags *******************/

boolean             dataRec                = false;      // nincs hasznalatban
boolean             handShake              = false;      // kommunikacio szinkron flag
boolean             commComp               = false;
boolean             dataFlag               = false;
boolean             relayFlag              = false;
boolean             sendFlag               = false;
  
/*********************** Declaration of parameters ********************/

int                 j, k, s                = 0;         // for ciklus valtozoja

boolean             switch1                = false;     // 1. szabadon felhasznalhato kapcsolo
boolean             switch2                = false;     // 2. szabadon felhasznalhato kapcsolo
boolean             switch3                = false;     // 3. szabadon felhasznalhato kapcsolo

int                 leftSlide              = 0;
int                 rightSlide             = 0;
int                 prevLeft               = 0;
int                 prevRight              = 0;

int                 timer                  = 0;
int                 prevTimer              = 0;

byte                command                = 00000000; // egyeb parancsok
int                 directRight            = 0;        // jobboldali lanctalp iranya
int                 directLeft             = 0;        // baloldali lanctalp iranya
int                 acceRight              = 0;        // jobboldali lanctalp PWM jele
int                 acceLeft               = 0;        // baloldai lanctalp PWM jele

int                 leftLEDs               = 0;        // baloldali LED-ek PWM jele
int                 rightLEDs              = 0;        // jobboldali LED-ek PWM jele


/******************* Buffers for the transreceiver *****************/

int            txbuff[9];                        // kuldesre varo adatok tombje
int            rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  IOInit();
  //Serial.begin(9600);
  mirfInit();
  //attachInterrupt(0,voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop()   {
  timer = millis() / 250;
  dataCollect();
  if ( timer != prevTimer) {
    voidPacketSend();
  }
  slideController();
  LEDControl();
}
/********************** I/O Init *****************************/
void IOInit(){
  pinMode(IRQ, INPUT);
  
  // Defining slide controllers
  pinMode(LEFT_CONT, INPUT);
  pinMode(RIGHT_CONT, INPUT);
  
  // Defining buttons
  pinMode(LEFT_ACC_BUT, INPUT);
  pinMode(LEFT_DCC_BUT, INPUT);
  pinMode(RIGHT_ACC_BUT, INPUT);
  pinMode(RIGHT_DCC_BUT, INPUT);
  
  //Defining LED's  
  pinMode(LEFT_ACC_LED, OUTPUT);
  pinMode(LEFT_DCC_LED, OUTPUT);
  pinMode(LEFT_STD_LED, OUTPUT);
  pinMode(RIGHT_ACC_LED, OUTPUT);
  pinMode(RIGHT_DCC_LED, OUTPUT);
  pinMode(RIGHT_STD_LED, OUTPUT);  
}
/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"cont1");
  Mirf.setTADDR((byte *)"tank1");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = command;
    txbuff[1] = directRight;
    txbuff[2] = directLeft;
    txbuff[3] = acceRight;
    txbuff[4] = acceLeft;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    txbuff[8] = 0;
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

/************************ Response **************************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}

/********************* Processing data **********************/
void slideController(){ 
  
  // Backward 
  if (( rightSlide < 383 ) && ( rightSlide >= 0 )){
    directRight = -1;
    acceRight = (( 383 - rightSlide ) / 1.5 );
  }
  if (( leftSlide < 383 ) && ( leftSlide >= 0 )){
    directLeft = -1;
    acceLeft = (( 383 - leftSlide )  / 1.5 );
  }
  // Forward
  if (( rightSlide < 1023 ) && ( rightSlide >= 639 )){
    directRight = 1;
    acceRight = (( rightSlide - 639 )  / 1.5 );
  }
  if (( leftSlide < 1023 ) && ( leftSlide >= 639 )){
    directLeft = 1;
    acceLeft = (( leftSlide - 639 ) / 1.5 );
  }
  // Standby
  if (( rightSlide > 383 ) && ( rightSlide < 639 )){
    directRight = 0;
    acceRight = 0;
  }
  if (( leftSlide > 383 ) && ( leftSlide < 639 )){
    directLeft = 0;
    acceLeft = 0;
  }
}

/*********************** Data collect *************************/
void dataCollect(){
  rightSlide = analogRead(RIGHT_CONT);
  leftSlide = analogRead(LEFT_CONT); 
}

/********************* LED control ***************************/
void LEDControl(){
  //leftLEDs = acceLeft / 1.5;
  //rightLEDs = acceRight / 1.5;
  // Right side of the controller
  if ( directRight == 0 ){
    analogWrite(RIGHT_ACC_LED, 0);
    analogWrite(RIGHT_DCC_LED, 0);
    digitalWrite(RIGHT_STD_LED, HIGH);
  }
  if ( directRight == 1 ){
    analogWrite(RIGHT_ACC_LED, acceRight);
    analogWrite(RIGHT_DCC_LED, 0);
    digitalWrite(RIGHT_STD_LED, LOW);
  }
  if ( directRight == -1 ){
    analogWrite(RIGHT_ACC_LED , 0);
    analogWrite(RIGHT_DCC_LED , acceRight);
    digitalWrite(RIGHT_STD_LED , LOW);
  }
  // Left side of the controller
  if ( directLeft == 0 ){
    analogWrite(LEFT_ACC_LED, 0);
    analogWrite(LEFT_DCC_LED, 0);
    digitalWrite(LEFT_STD_LED, HIGH);
  }
  if ( directLeft == 1 ){
    analogWrite(LEFT_ACC_LED, acceLeft);
    analogWrite(LEFT_DCC_LED, 0);
    digitalWrite(LEFT_STD_LED, LOW);
  }
  if ( directLeft == -1 ){
    analogWrite(LEFT_ACC_LED, 0);
    analogWrite(LEFT_DCC_LED, acceLeft);
    digitalWrite(LEFT_STD_LED, LOW);
  }
}

