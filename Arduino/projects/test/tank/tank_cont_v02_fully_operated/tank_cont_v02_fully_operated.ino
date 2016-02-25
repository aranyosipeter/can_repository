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

byte                command                = 00000000;
int                 directRight            = 0;
int                 directLeft             = 0;
int                 acceRight              = 0;
int                 acceLeft               = 0;
int                 rx                     = 0;


/******************* Buffers for the transreceiver *****************/

int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  IOInit();
  Serial.begin(9600);
  mirfInit();
  attachInterrupt(0, voidResponse, LOW);
}

/************************** Loop *******************************/
void loop()   {
  timer = millis() / 500;
  dataCollect();
  /*if ( timer != prevTimer ){
    voidPacketSend();
    prevTimer = timer;
    Serial.print("Jobb: ");
    Serial.println(directRight, DEC);
    Serial.println(acceRight, DEC);
    Serial.print("Bal: ");
    Serial.println(directLeft, DEC);
    Serial.println(acceLeft, DEC);
  }*/
  slideController();
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
  Mirf.payload = 32;
  Mirf.channel = 10;
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
 rx = rxbuff[0];
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}

/******************* Processing data *******************/
void slideController(){ 
  
  // Backward 
  if (( rightSlide <= 384 ) && ( rightSlide >= 0 )){
    directRight = -1;
    acceRight = ( 384 - rightSlide ) / 1,5;
  }
  if (( leftSlide <= 384 ) && ( leftSlide >= 0 )){
    directLeft = -1;
    acceLeft = ( 384 - leftSlide ) / 1,5;
  }
  // Forward
  if (( rightSlide <= 1024 ) && ( rightSlide >= 640 )){
    directRight = 1;
    acceRight = ( rightSlide - 640 ) / 1,5;
  }
  if (( leftSlide <= 1024 ) && ( leftSlide >= 640 )){
    directLeft = 1;
    acceLeft = ( leftSlide - 640 ) / 1,5;
  }
  // Standby
  if (( rightSlide > 384 ) && ( rightSlide < 640 )){
    directRight = 0;
    acceRight = 0;
  }
  if (( leftSlide > 384 ) && ( leftSlide < 640 )){
    directLeft = 0;
    acceLeft = 0;
  }
}
/********************** Data collect ***********************/
void dataCollect(){
  rightSlide = analogRead(RIGHT_CONT);
  leftSlide = analogRead(LEFT_CONT); 
}
