// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2


/******************** Timer values *****************/

/************************* Flags *******************/

boolean             dataRec                = false;      // nincs hasznalatban

/*********************** Declaration of parameters ********************/


int                 j, k, s                = 0;         // for ciklus valtozoja
int                 relayComPin            = 0;

int                 val                    = 0;
int                 dhtHum                 = 0;
int                 dhtTemp                = 0;
int                 light                  = 0;
long                bmpPres                = 0;


/******************* Buffers for the transreceiver *****************/

int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  pinMode(IRQ, INPUT);
  Serial.begin(9600);
  mirfInit();
  attachInterrupt(0, voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop()   {
  if (( millis() % 500 ) == 0 ){
    voidPacketSend();
    Serial.print("dhtHum: ");
  Serial.println(dhtHum, DEC); 
  Serial.print("dhtTemp: ");
  Serial.println(dhtTemp, DEC); 
  Serial.print("val: ");
  Serial.println(val, DEC); 
  Serial.print("bmpPres: ");
  Serial.println(bmpPres, DEC); 
  }
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.csnPin = 9;
  Mirf.cePin = 8;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.setTADDR((byte *)"devou");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = 0;
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
  voidProcess();
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
  dataRec = true;
}

/******************* Processing data *******************/
void voidProcess(){
  dhtHum = rxbuff[0];
  dhtTemp = rxbuff[1];
  val = rxbuff[2];
  bmpPres = rxbuff[3];
  light =  val / 10,24 ;
  
}



