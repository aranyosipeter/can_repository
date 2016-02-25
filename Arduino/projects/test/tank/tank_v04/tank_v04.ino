#include <Wire.h>


// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
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
  pinMode(IRQ, INPUT);
  Serial.begin(9600);
  mirfInit();
  attachInterrupt(0,voidResponse, LOW);
}

/************************** Loop *******************************/
void loop()   {
  voidProcess();
  voidSerialData();
  /*timer = millis() / 500;
  if ( timer != prevTimer ){
    voidPacketSend();
    prevTimer = timer;
    Serial.print("Jobb: ");
    Serial.println(directRight, DEC);
    Serial.println(acceRight, DEC);
    Serial.print("Bal: ");
    Serial.println(directLeft, DEC);
    Serial.println(acceLeft, DEC);
  }*/
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
  acceRight = rxbuff[3];
  acceLeft = rxbuff[4] ; 
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
