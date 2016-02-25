#include <Wire.h>


// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define RIGHT_PWM 6
#define LEFT_PWM 5
#define RIGHT_FOR 16
#define RIGHT_BACK 17
#define LEFT_FOR 14
#define LEFT_BACK 15
#define STANDBY 9
/******************** Timer values *****************/

unsigned long      alarmTimer              = 0;
unsigned long      tmpTimer                = 0;
unsigned long      time                    = 0;
unsigned long      timeOut                 = 0;
unsigned long      clearTime               = 0;
unsigned long      clearPrevTime           = 0;

/************************* Flags *******************/

boolean             sendFlag               = false;
boolean             serialDataFlag         = false;
boolean             dataRec                = false;
  
/*********************** Declaration of parameters ********************/

int                 j, k, s                = 0;         // for ciklus valtozoja

boolean             switch1                = false;     // 1. szabadon felhasznalhato kapcsolo
boolean             switch2                = false;     // 2. szabadon felhasznalhato kapcsolo
boolean             switch3                = false;     // 3. szabadon felhasznalhato kapcsolo

unsigned long       mills                  = 0;
unsigned long       subtract               = 0;
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
  Serial.begin(9600);
  mirfInit();
  IOInit();
  attachInterrupt(0,voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop()   {
  voidProcess();
  voidSerialData();
  tankControl();
}
/********************* I/O init ******************************/
void IOInit(){
  pinMode(IRQ, INPUT);
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_FOR, OUTPUT);
  pinMode(RIGHT_BACK, OUTPUT);
  pinMode(LEFT_FOR, OUTPUT);
  pinMode(LEFT_BACK, OUTPUT);
  pinMode(STANDBY, OUTPUT);
}
/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"tank1");
  Mirf.setTADDR((byte *)"cont1");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = 1;
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
  dataRec = true;
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
  dataRec = true;
}

/******************* Processing data *******************/
void voidProcess(){
  command = rxbuff[0];
  directRight = rxbuff[1];
  directLeft = rxbuff[2];
  acceRight = (rxbuff[3] / 1.5);
  acceLeft = (rxbuff[4] / 1.5); 
}

/******************************************- Serial data ************************************/
void voidSerialData(){
  if ( dataRec == true ){
    Serial.print("Jobb oldali irany: ");
    Serial.println(directRight, DEC);
    Serial.print("Bal oldali irany: ");
    Serial.println(directLeft, DEC);
    Serial.print("Jobb oldali seb: ");
    Serial.println(acceRight, DEC);
    Serial.print("Bal oldali seb: ");
    Serial.println(acceLeft, DEC);
    dataRec = false;  
  } 
}

/********************** Tank control ************************/
void tankControl(){ 
  // Standby
  if ((directRight == 0) && (directLeft == 0)){
    digitalWrite(STANDBY, LOW);
    digitalWrite(LEFT_FOR, LOW);
    digitalWrite(LEFT_BACK, LOW);
    digitalWrite(RIGHT_FOR, LOW);
    digitalWrite(RIGHT_BACK, LOW);
    analogWrite(LEFT_PWM,  0);
    analogWrite(RIGHT_PWM, 0);
    Serial.println("nem megyunk");
  }
  // Going forward
  if (directRight > 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(RIGHT_BACK, LOW);
    digitalWrite(RIGHT_FOR, HIGH);
    analogWrite(RIGHT_PWM, acceRight);
    Serial.println("jobbra elre megyunk");
  }
  if (directLeft > 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(LEFT_FOR, HIGH);
    digitalWrite(LEFT_BACK, LOW);
    analogWrite(LEFT_PWM, acceLeft);
    Serial.println("balra elre megyunk");
  }
  
  // Going backward
  if (directRight < 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(RIGHT_FOR, LOW);
    digitalWrite(RIGHT_BACK, HIGH);
    analogWrite(RIGHT_PWM, acceRight);
    Serial.println("jobbra hatra megyunk");
  }
  if (directLeft < 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(LEFT_FOR, LOW);
    digitalWrite(LEFT_BACK, HIGH);
    analogWrite(LEFT_PWM, acceLeft);
    Serial.println("balra hatra megyunk");
  }
}


