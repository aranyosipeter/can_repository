#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 3

/*********************** Codes ********************/
byte                identifier           = B00000010;
byte                command              = B00000000;
boolean             buttonValue          = false;

/******************* Buffers for the transreceiver *****************/
int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup(){
  pinMode(IRQ, INPUT);
  mirfInit();
  Serial.begin(9600);
  Serial.println("Controller started!"); 
  attachInterrupt(1, voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop(){
  if (( millis() % 500 ) == 0 ){
    voidPacketSend();
  }
  if (( millis() % 1100 ) == 0 ){
    Serial.print(" Button: ");
    Serial.println(buttonValue);
  }
} 

/************************* Mirf init ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 10;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.setTADDR((byte *)"node1"); 
  Mirf.config();  
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = identifier;
    txbuff[1] = command;
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
  buttonValue = rxbuff[2];
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}

