// This headers for the I2C connections
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

MPU6050 accelgyro;

/******************** Dec of gyro ******************/

int16_t ax, ay, az;
int16_t gx, gy, gz;

/******************** Timer values *****************/

/************************* Flags *******************/

boolean ackRecv              = false;      // nincs hasznalatban
boolean handShake            = false;      // kommunikacio szinkron flag

/*********************** Codes ********************/

int i                        = 0;         // for ciklus valtozoja

boolean switch1              = false;     // 1. szabadon felhasznalhato kapcsolo
boolean switch2              = false;     // 2. szabadon felhasznalhato kapcsolo
boolean switch3              = false;     // 3. szabadon felhasznalhato kapcsolo
byte handS_sent              = B10101010; // szinkron bajt
byte handS_recv              = B00000000; // szinkron bajt
byte recv_buff               = B00000000; // beerkezo adat
byte sendAck                 = B00000000; // visszajelzes
byte recvAck                 = B00000000; // visszejzes a visszejelzesrol

/******************* Buffers for the transreceiver *****************/

int16_t txbuff[9];                        // kuldesre varo adatok tombje
int16_t rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  Wire.begin();
  Serial.begin(9600);
  mirfInit();
  gyroInit();
  Serial.println(" Controller started!");
}

/************************** Loop *******************************/
void loop()   {
    //delay(50);
    Mirf.setRADDR((byte *)"cont1");
    Mirf.setTADDR((byte *)"dev01");
    while ( handShake == false ){
      Serial.println(" Handshaking!");
      voidHandShake();
    }
    voidGyro();
    //while ( ackRecv == false ){
    voidPacketSend();
    voidRecvAck();
    //}
    ackRecv = false;
    //delay(100);
    //voidGetPacket();
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"cont1");
  Mirf.setTADDR((byte *)"dev01");
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.config();
}

/******************* Initalizing MPU6050 ************************/
void gyroInit(){
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

/********************* Handshake ****************************/
void voidHandShake(){
  Mirf.send((byte *)&handS_sent);
  while(Mirf.isSending()){
  }
  Serial.println(" Request for handshaking sent!"); 
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &handS_recv); 
  }
  if ( handS_sent == handS_recv ) {
    handShake = true;
    Mirf.send((byte *)&handS_sent);
    while(Mirf.isSending()){
    }
    Serial.println(" Handshake successful!"); 
  }
  else 
     Serial.println(" No connection between the receiver and the transmitter! "); 
}
/******************** Receive acknowledgement ***************/
void voidRecvAck(){
 Serial.println(" Receiving ack!"); 
 if (Mirf.dataReady() == true){
   Mirf.getData((byte *) &recv_buff);
   sendAck = recv_buff;
   Mirf.send((byte *)&sendAck);
   while(Mirf.isSending()){
   } 
  }
  if (Mirf.dataReady() == true){
   Mirf.getData((byte *) &recv_buff);
   recvAck = recv_buff;
   if ( recvAck == sendAck ) {
        ackRecv = true;
        Serial.println(" Ack successful!"); 
      }
      else 
       Serial.println(" Ack not successful! "); 
  }
}
/******************** Generating packet ********************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = ax;
    Serial.print("0. bajt: ");
    Serial.println(txbuff[0], BIN);
    txbuff[1] = ay;
    Serial.print("1. bajt: ");
    Serial.println(txbuff[1], BIN);
    txbuff[2] = az;
    Serial.print("2. bajt: ");
    Serial.println(txbuff[2], BIN);
    txbuff[3] = gx;
    Serial.print("3. bajt: ");
    Serial.println(txbuff[3], BIN);
    txbuff[4] = gy;
    Serial.print("4. bajt: ");
    Serial.println(txbuff[4], BIN);
    txbuff[5] = gz;
    Serial.print("5. bajt: ");
    Serial.println(txbuff[5], BIN);
    txbuff[6] = switch1;
    Serial.print("6. bajt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = switch2;
    Serial.print("7. bajt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = switch3;
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
      Serial.println(rxbuff[i]);
    }
    Serial.println("Packet receiver: Packet is got! ");
  }
}

/******************** Gyro **************************/
void voidGyro(){
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); 
  Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
}


