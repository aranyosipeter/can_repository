// This headers for the I2C connections to the display and the RTC
#include <Wire.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

/******************** Timer values *****************/

unsigned long tmpTimer = 0;
unsigned long time = 0;
unsigned long timeOut = 0;
unsigned long clearTime = 0;
unsigned long clearPrevTime = 0;

/************************* Flags *******************/

boolean receiveP = false;

/*********************** Codes ********************/

int j, k;
int seconds = 0;
int temp = 0;
int clientq = 0;

int sendTime = 0;
int prevTime = 0;

/******************* Buffers for the transreceiver *****************/

int txbuff[8];
int rxbuff[8];

void setup()  
{
  Wire.begin();
  Serial.begin(9600);
  mirfInit();
}

void loop()   {
    Mirf.setRADDR((byte *)"cont1");
    Mirf.setTADDR((byte *)"dev01");
    voidPacketSend();
    Serial.print("Packet is sent!");

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
    }
    prevTime=sendTime;
    receiveP = false;
  }
  //voidDataReceive();
}

/******************* Initalizing MiRF ************************/

void mirfInit(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.payload = 32;
  Mirf.config();
}

/******************** Generating packet ********************/

void voidPacketGen(){
  // Creating packet
    txbuff[0] = 0;
    Serial.print("0. bájt: ");
    Serial.println(txbuff[0], BIN);
    txbuff[1] = 0;
    Serial.print("1. bájt: ");
    Serial.println(txbuff[1], BIN);
    txbuff[2] = 0;
    Serial.print("2. bájt: ");
    Serial.println(txbuff[2], BIN);
    txbuff[3] = 0;
    Serial.print("3. bájt: ");
    Serial.println(txbuff[3], BIN);
    txbuff[4] = 0;
    Serial.print("4. bájt: ");
    Serial.println(txbuff[4], BIN);
    txbuff[5] = 0;
    Serial.print("5. bájt: ");
    Serial.println(txbuff[5], BIN);
    txbuff[6] = 0;
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = 0;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = 0;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    Serial.println("Packet for registration is created");
}

/******************** Function for sending packet ********************/

void voidPacketSend(){
    voidPacketGen();
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    Serial.println("Packet sent...");
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


