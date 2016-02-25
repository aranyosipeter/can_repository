#include <Wire.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define RIGHT_PWM 5
#define LEFT_PWM 6
#define RIGHT_FOR 14
#define RIGHT_BACK 15
#define LEFT_FOR 16
#define LEFT_BACK 17
#define STANDBY 9

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

int                 timer                  = 0;
int                 prevTimer              = 0;

byte                command                = 00000000;
int                 directRight            = 0;
int                 directLeft             = 0;
int                 acceRight              = 0;
int                 acceLeft               = 0;


/******************* Buffers for the transreceiver *****************/

int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  pinMode(IRQ, INPUT);
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_FOR, OUTPUT);
  pinMode(RIGHT_BACK, OUTPUT);
  pinMode(LEFT_FOR, OUTPUT);
  pinMode(LEFT_BACK, OUTPUT);
  pinMode(STANDBY, OUTPUT);
  Wire.begin();
  Serial.begin(9600);
  mirfInit();
  attachInterrupt(0,voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop()   {
  timer = millis() / 2000;
  voidProcess();
  if ( timer != prevTimer ){
    voidPacketSend();
    Serial.println("Sending");
    prevTimer = timer;
  }
  for (j = 0; j < 10; j++){
      Serial.println(rxbuff[j], DEC);
  }
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.csnPin = 7;
  Mirf.cePin = 8;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"tank1");
  Mirf.setTADDR((byte *)"cont1");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = 255;
    txbuff[1] = 0;
    txbuff[2] = 0;
    txbuff[3] = 0;
    txbuff[4] = 0;
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
  rxbuff[0] = command;
  rxbuff[1] = directRight;
  rxbuff[2] = directLeft;
  rxbuff[3] = acceRight;
  rxbuff[4] = acceLeft;
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}

/******************* Processing data *******************/
void voidProcess(){ 
  if ((directRight == 0) && (directLeft == 0)){
    digitalWrite(STANDBY, LOW);
    digitalWrite(LEFT_FOR, LOW);
    digitalWrite(LEFT_BACK, LOW);
    digitalWrite(RIGHT_FOR, LOW);
    digitalWrite(RIGHT_BACK, LOW);
    analogWrite(LEFT_PWM,  0);
    analogWrite(RIGHT_PWM, 0);
  }
  if (directRight > 0){
    digitalWrite(RIGHT_FOR, HIGH);
    analogWrite(RIGHT_PWM, acceRight);
  }
  if (directRight < 0){
    digitalWrite(RIGHT_BACK, HIGH);
    analogWrite(RIGHT_PWM, acceRight);
  }
  
  if (directLeft > 0){
    digitalWrite(LEFT_FOR, HIGH);
    analogWrite(LEFT_PWM, acceLeft);
  }
  if (directLeft < 0){
    digitalWrite(LEFT_BACK, HIGH);
    analogWrite(LEFT_PWM, acceLeft);
  }
}


