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

byte txbuff[16];
byte rxbuff[16];

void setup()  
{
  Wire.begin();
  Serial.begin(9600);
  mirfInit();
}

void loop()   {  
  time = millis();
  sendTime = time/180000;
  if ( Mirf.dataReady() == true ){
    Mirf.setRADDR((byte *)"serv1");
    Mirf.setTADDR((byte *)"dev01");
    voidGetPacket();
    Serial.println("Got packet!");
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
    }
    prevTime=sendTime;
    receiveP = false;
  }
  //voidDataReceive();
  //voidKey();
  //voidConfigPins(&dev, &devPin);
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

void voidPacketGen(int serv){
  byte checkSum;
  // Creating package for the registration
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

    txbuff[6] = 0;
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = 0;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = 0;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = 0;
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

  // Creating package for sending lifesignals
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

    txbuff[6] = 0;
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = 0;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = 0;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = 0;
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

/******************** Request for fifeSignal ********************/

void reqLifeSignal(){
  voidPacketGen(0);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Lifesignal request sent...");
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


