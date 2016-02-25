// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define ANA 17
#define LED 8

/************************* Timers ******************/
long               time                  = 0;
long               sendTime              = 0;
long               prevTime              = 0;
boolean            dataRec               = false;
/************************* Flags *******************/

/*********************** Codes ********************/
boolean             recFlag              = false;
int                 i, r                 = 0;
short               devID                = 3;
byte                command              = B00000001;

/******************* Buffers for the transreceiver *****************/
int                 packetNum            = 0;
byte                txbuff[15];                        // kuldesre varo adatok tombje
byte                rxbuff[15];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup() {
  pinMode(IRQ, INPUT);
  pinMode(ANA, INPUT);
  pinMode(LED, OUTPUT);
  mirfInit();
  Serial.begin(9600);
  Serial.println("Controller started!");
  attachInterrupt(0, voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop() {
  digitalWrite(LED, LOW);
  if (millis() > (sendTime + 15000)){
    voidPacketSend();
    digitalWrite(LED, HIGH);
    delay(1500);
    Serial.println("Data sent!");
    sendTime = millis();
  }
  if (recFlag){
    for (int i = 0; i < 15; i++){
      Serial.print("%i adat: ");
      Serial.println(rxbuff[i]);
      recFlag = false;
    }
  }
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"servU");
  Mirf.setTADDR((byte *)"nodeU");
  Mirf.config();
}

/******************** Generating packet ********************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = 0;
    txbuff[1] = 0;
    txbuff[2] = 0;
    txbuff[3] = 0;
    txbuff[4] = 0;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
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
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
    recFlag = true;
  }
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
  digitalWrite(LED, HIGH);
}

