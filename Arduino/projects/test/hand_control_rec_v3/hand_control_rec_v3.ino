// This headers for the I2C connections 
#include <Wire.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

/******************** Timer values *****************/

/************************* Flags *******************/

boolean ackRecv              = false;      // visszjelzo flag
boolean handShake            = false;      // kommunikacio szinkron flag

/*********************** Codes ********************/

int i                        = 0;         // for ciklus valtozoja

boolean switch1              = false;     // 1. szabadon felhasznalhato kapcsolo
boolean switch2              = false;     // 2. szabadon felhasznalhato kapcsolo
boolean switch3              = false;     // 3. szabadon felhasznalhato kapcsolo
byte handS_sent              = B00000000; // szinkron bajt
byte handS_recv              = B00000000; // szinkron bajt
byte handS_buff              = B00000000; // szinkron bajt
byte recv_buff               = B00000000; // beerkezo adat
byte sendAck                 = B11111111; // visszajelzes
byte recvAck                 = B00000000; // visszejzes a visszejelzesrol

/******************* Buffers for the transreceiver *****************/

int16_t txbuff[8];
int16_t rxbuff[8];

void setup()  
{
  Wire.begin();
  Serial.begin(9600);
  mirfInit();
  Serial.println(" Controller started!");
}

void loop()   {
  delay(50);
  Mirf.setRADDR((byte *)"dev01");
  Mirf.setTADDR((byte *)"cont1");
  while ( handShake == false ){
      Serial.println(" Handshaking!");
      voidHandShake();
  }
  //while ( ackRecv == false ){
   voidGetPacket();
   voidSendAck();
  //}
  ackRecv = false;
}

/******************* Initalizing MiRF ************************/

void mirfInit(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"dev01");
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.config();
}

/********************* Handshake ****************************/
void voidHandShake(){
  Serial.println(" Handshaking method!"); 
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &handS_buff); 
    Serial.println(" Got request!");
    Serial.println(handS_buff, DEC);
    handS_sent = handS_buff;
    Mirf.send((byte *)&handS_sent);
    while(Mirf.isSending()){
    }
    Serial.println(" Respond for handshaking sent!"); 
    if (Mirf.dataReady() == true){
      Mirf.getData((byte *) &handS_recv); 
    }
    if ( handS_sent == handS_recv ) {
      handShake = true;
      Serial.println(" Handshake successful!"); 
    }
    else 
     Serial.println(" No connection between the receiver and the transmitter! "); 
  }
  else Serial.println(" No request!");
  
}

/******************* Send acknowledgement ******************/
void voidSendAck(){
  Serial.println(" Sending ack!"); 
  Mirf.send((byte *)&sendAck);
  while(Mirf.isSending()){
  }
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &recv_buff);
      recvAck = recv_buff;
      if ( recvAck == sendAck ) {
        ackRecv = true;
        Mirf.send((byte *)&sendAck);
        while(Mirf.isSending()){
        }
        Serial.println(" Ack successful!"); 
      }
      else 
       Serial.println(" Ack not successful! "); 
  }
}

/******************** Generating packet ********************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = 0;
    Serial.print("0. bajt: ");
    Serial.println(txbuff[0], BIN);
    txbuff[1] = 0;
    Serial.print("1. bajt: ");
    Serial.println(txbuff[1], BIN);
    txbuff[2] = 0;
    Serial.print("2. bajt: ");
    Serial.println(txbuff[2], BIN);
    txbuff[3] = 0;
    Serial.print("3. bajt: ");
    Serial.println(txbuff[3], BIN);
    txbuff[4] = 0;
    Serial.print("4. bajt: ");
    Serial.println(txbuff[4], BIN);
    txbuff[5] = 0;
    Serial.print("5. bajt: ");
    Serial.println(txbuff[5], BIN);
    txbuff[6] = 0;
    Serial.print("6. bajt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = 0;
    Serial.print("7. bajt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = 0;
    Serial.print("8. bajt: ");
    Serial.println(txbuff[8], BIN);
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
    for ( i = 0; i < 9; i++ ){
       Serial.print(i);
      Serial.print(". bajt: ");
      Serial.println(rxbuff[i], DEC);
    }
    Serial.println("Packet receiver: Packet is got! ");
  }
}



