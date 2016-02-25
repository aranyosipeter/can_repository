#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "header.h"
#include <avr/sleep.h>

#define LED 4
#define PIR 2
#define BUTTON 5
#define INTERRUPT 3

// Deklaralas //

boolean alarmSent = true;
boolean regPushB = false;
boolean erasePushB = false;
boolean state = false;
boolean regFlag = false;
boolean timeFlag = false;
boolean sendTiming = false;
boolean pir = false;
boolean sign = false;

unsigned long time;
unsigned long sendTime = 0;
unsigned long prevTime = 0;
unsigned long timeOut = 0;
unsigned long buttonTime;
unsigned long startTime = 0;
unsigned long sendStTime = 0;

int i,j,k;
int clientq = 0;
int led = 0;
int alarm = 0;
int interrupt0 = 0;
int interrupt1 = 0;
int pins[]={
  6,9,10};

byte txbuff[16];
byte rxbuff[16];

int servNum=0;
byte services[3] = { 
  B11001100, B00110011, B10011001 };

// Node strukturak deklaralasa  //
node server = {
  "serv1", B11111111, B00000010, B00001010, true};

node dev = {
  "dev01",B10101010, B00000001, B00001010, false};

node client[3];

// NodePin strukturak deklaralasa //

nodepin devPin;
nodepin clientPin[3];


void arming(){
  pir = true;
}

void signal(){
  sign = true;
}

void setup(){
  Serial.begin(9600);
  voidMirfInit();
  voidInit();
  pinMode(PIR, INPUT);
  pinMode(INTERRUPT, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT); 
  attachInterrupt(0, arming, RISING);
  attachInterrupt(1,signal, LOW);
  Mirf.setRADDR((byte *)"dev01");
  Mirf.setTADDR((byte *) "serv1");
}

void sleepNow(){
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   
  sleep_enable();         
  attachInterrupt(0,arming, RISING); 
  attachInterrupt(1,signal, LOW);
  sleep_mode();            
  // Here we're coming!!!
  sleep_disable();  
  detachInterrupt(0);
  detachInterrupt(1);
}

void loop(){
  if ( sign == true ){
    voidGetSignal();
    if ( rxbuff[6] == services[2] ) {
      Mirf.setTADDR((byte *)"serv1");
      Mirf.setRADDR((byte *)"dev01");
      lifeSignalSend();
      timeOut = millis();
      while(!Mirf.dataReady() && time - timeOut < 3000){
      }
      voidGetPacket();
    }
  }
  voidPir();
  if( pir == true ){
    Mirf.setTADDR((byte *)"serv1");
    Mirf.setRADDR((byte *)"dev01");
    lifeSignalSend();
    timeOut = millis();
    while(!Mirf.dataReady() && time - timeOut < 3000){
    }
    voidGetPacket();
  }
  pir == false;
  sign == false;
  sleepNow();
}

/******************** Mirf inicializalasa *****************/

void voidMirfInit(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = 32;
  Mirf.config();
}

/********************* Initalizing ***********************/

void voidInit(){
  for (int i=0;i<3;i++){
    devPin.pins[i]=pins[i];
  }
}

/************************ Using PIR **********************/

void voidPir(){

  if(pir == true){
    setAlarmBit();
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
  }
  else 
    if (pir == false){
    clearAlarmBit();
  }
} 

/******************** LifeSignal sending ********************/

void lifeSignalSend(){
  servNum = 2;
  voidPacketGen(servNum);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Lifesignal is sent...");
}

/******************** Registration sending ********************/

void registrationSend(){
  servNum = 0;
  voidPacketGen(servNum);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Registration is sent...");
}

/******************** DeleteSignal sending ********************/

void deleteSignalSend(){
  servNum = 1;
  voidPacketGen(servNum);
  Mirf.send((byte *)&txbuff);
  while(Mirf.isSending()){
  }
  Serial.println("Deletesignal is sent...");
}

/****************** Manipulating alarm bit *****************/

void setAlarmBit(){
  bitSet(dev.state, 0);
}

void clearAlarmBit(){
  bitClear(dev.state, 0); 
}

/******************** Generating packet ********************/

void voidPacketGen(int serv){
  byte checkSum;
  // Creating package for the registration
  if ( serv == 0 ){
    for (int i=0; i<6; i++){
      txbuff[i] = dev.address[i];
    }
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

    txbuff[6] = services[0];
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = dev.id;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = dev.state;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = dev.role;
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
  }

  // Creating package for deleting the registration info
  if ( serv == 1 ){
    for (int i=0; i<6; i++){
      txbuff[i] = B00000000;
    }
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

    txbuff[6] = services[1];
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = B00000000;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = B00000000;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = B00000000;
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
    Serial.println("Packet for exit is created");
  }

  // Creating package for sending lifesignals
  if ( serv == 2 ){
    for (int i=0; i<6; i++){
      txbuff[i] = dev.address[i];
    }
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

    txbuff[6] = services[2];
    Serial.print("6. bájt: ");
    Serial.println(txbuff[6], BIN);
    txbuff[7] = dev.id;
    Serial.print("7. bájt: ");
    Serial.println(txbuff[7], BIN);
    txbuff[8] = dev.state;
    Serial.print("8. bájt: ");
    Serial.println(txbuff[8], BIN);
    txbuff[9] = dev.role;
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

}
/****************** Filling up server data *******************/

int voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);  
    for (int i=0; i<16; i++){
      Serial.print(i, DEC);
      Serial.print(" .-k bájt: ");
      Serial.println(rxbuff[i], BIN);
    }
  }
}

/****************** Accepting the signals *******************/

int voidGetSignal(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);  
    
  }
}






