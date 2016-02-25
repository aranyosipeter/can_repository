#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "MyStructs.h"

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

unsigned long time;
unsigned long sendTime = 0;
unsigned long prevTime = 0;
unsigned long timeOut = 0;
unsigned long buttonTime;
unsigned long startTime = 0;
unsigned long sendStTime = 0;

int i,j,k;
int clientq = 0;
int pir = 0;
int led = 0;
int alarm = 0;
int pins[]={
  6,9,10};

byte txbuff[16];
byte rxbuff[16];

int servNum=0;
byte services[3] = { 
  B11001100, B00110011, B10011001 };

// Node strukturak deklaralasa  //
node server;
node reg = {
  "dev01",B10101010, B00000001, B00001010, false};
node dev = {
  "dev01",B10101010, B00000001, B00001010, false};
node client[3];

// NodePin strukturak deklaralasa //

nodepin devPin;
nodepin clientPin[3];


void setup(){
  Serial.begin(9600);
  voidMirfInit();
  voidInit();
  pinMode(PIR, INPUT);
  pinMode(INTERRUPT, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT); 
}

void loop(){
  time=millis();
  sendTime=time/15000;

  if (regFlag == false){
    voidReg();
  }
  else {
    buttonMode();
    voidPir();
    if (sendTime != prevTime){
      lifeSignalSend();
      timeOut = millis();
      while(!Mirf.dataReady() && time - timeOut < 2000){
      }
      voidGetPacket(2);
      //voidDataSend(pir);
      //voidConfigPins(&dev, &devPin);
      alarmSent=true;
      prevTime=sendTime;
    }
  }
} 

// Mirf inicializalasa //

void voidMirfInit(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = 32;
  Mirf.config();
}

// 

void voidInit(){
  for (i=0;i<3;i++){
    devPin.pins[i]=pins[i];
    Serial.print("Korok szama: ");
    Serial.println(i, DEC);
    Serial.print("Ha megegyezik akkor jau: ");
    Serial.print(devPin.pins[i], DEC);
    Serial.println(pins[i], DEC);
  }
}

void voidPir(){

  if(digitalRead(PIR)==HIGH){
    pir=1;
    digitalWrite(LED, HIGH);
  }
  else{
    pir=0;
    digitalWrite(LED, LOW);
  }

  if (alarmSent==true){
    if(pir==1){
      setAlarmBit();
    }
    else{
      clearAlarmBit();
    }
    alarmSent==false;
  }
} 

void lifeSignalSend(){
  voidPacketGen(3);
  Mirf.setTADDR((byte *) &server.address);
  Mirf.send((byte *)&rxbuff);
  while(Mirf.isSending()){
  }
}

void setAlarmBit(){
  bitSet(dev.state, 0);
}

void clearAlarmBit(){
  bitClear(dev.state, 0); 
}

/******************* Configuring I/O pins *********************/

void voidConfigPins(struct node *device, struct nodepin *devicePin){
  for(i=0;i<3;i++){

    Serial.print("Eredeti pin: ");
    Serial.println((*devicePin).pins[i], DEC);
    int bit=bitRead((*device).state,(i+1));
    if(bit==0){
      Serial.print("Hanyadik bit: ");
      Serial.println(bitRead((*device).state,(i+1)), DEC);

      pinMode((*devicePin).pins[i], INPUT);

      Serial.print("Hanyadik pin: ");
      Serial.println((*devicePin).pins[i], DEC);

      (*devicePin).mode[i]=0;
    }

    if(bit==1){
      Serial.print("Hanyadik bit: ");
      Serial.println(bitRead((*device).state,(i+1)), DEC);
      pinMode((*devicePin).pins[i], OUTPUT);
      Serial.print("Hanyadik pin: ");
      Serial.println((*devicePin).pins[i], DEC);
      (*devicePin).mode[i]=1;
    }
  }
  delay(1000);
} 

/********************* PIR registration ********************/

void voidReg(){
  servNum = 0;
  buttonMode();
  if ( regPushB == true && timeFlag == false ){ 
    voidPacketGen(servNum);
    sendigTime();
    if (voidGetPacket(servNum) == 0){
      regFlag = true;
      ledFlash(0);
    }
    else{
      regPushB = false;
      ledFlash(2);
    }

  }
}

/******************** Using LED ***************************/

void ledFlash(int flash){
  if (flash == 0){
    for (int i=0; i<5; i++){
      digitalWrite(LED, HIGH);
      delay(100+(flash*100));
      digitalWrite(LED, LOW);
    }
  }

  if (flash == 1){
    for (int i=0; i<5; i++){
      digitalWrite(LED, HIGH);
      delay(100+(flash*100));
      digitalWrite(LED, LOW);
    }
  }

  if (flash == 2){
    for (int i=0; i<5; i++){
      digitalWrite(LED, HIGH);
      delay(100+(flash*100));
      digitalWrite(LED, LOW);
    }
  }  
}

/******************* Timer for registration ****************/

void sendigTime(){

  if ( sendTiming == false ){
    sendStTime = millis();
    sendTiming = true;
  }
  if ( millis()-sendStTime>60000 ){
    timeFlag = true;
    sendTiming = false;
  }
}

/***************** Button for registration ***************/

void buttonMode(){
  int regButton = 10000;
  int eraseButton = 30000;

  long duration = switchTime();
  Serial.print("Button time: ");
  Serial.println(duration, DEC);
  if( duration >= regButton ){
    regPushB=true;
  }
  else if ( duration >= eraseButton ){
    regPushB=false;
    erasePushB=true;
    timeFlag=false;
  }
  Serial.print("RegButton: ");
  Serial.println(regPushB);
  Serial.print("EraseButton: ");
  Serial.println(erasePushB);

}

/********************* Timer for the button *****************/

long switchTime(){
  boolean button=digitalRead(BUTTON);

  if(button != state) {
    state = HIGH; 
    startTime = millis(); 
  }
  while (digitalRead(BUTTON)==HIGH && state==HIGH){
    buttonTime=millis() - startTime;
  }
  if(digitalRead(BUTTON)==LOW && state==HIGH){
    state=LOW;
    return buttonTime;
  }    
  else
    return 0; 
}

/******************** Generating packet ******************/

void voidPacketGen(int serv){
  byte checkSum;
  for (int i=0; i<6; i++){
    txbuff[i] = dev.address[i];
  }
  for (int j=0; j<4; j++){
    if (j == serv){
      txbuff[6] = services[j];
    }
  }
  txbuff[7] = dev.id;
  txbuff[8] = dev.state;
  txbuff[9] = dev.role;
  txbuff[10] = B00000000;
  txbuff[11] = B00000000;
  txbuff[12] = B00000000;
  txbuff[13] = B00000000;
  txbuff[14] = B00000000;
  for (int k=0; k<15; k++){
    checkSum += txbuff[k];
  }
  txbuff[15] = checkSum; 
  Mirf.send((byte *)&txbuff);
}

/****************** Filling up server data ************/

int voidGetPacket(int ackServ){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);  
    if (rxbuff[6] == services[ackServ]){

      if (ackServ == 0){
        for (int i=0; i<6; i++){
          server.address[i] = rxbuff[i];
        }
        server.serv = rxbuff[6];
        server.id = rxbuff[7];
        server.state = rxbuff[8];
        server.role = rxbuff[9];
        return 0;
      }

      if (ackServ == 1){
        for (int i=0; i<6; i++){
          server.address[i] = '0';
        }
        dev.serv = B00000000;
        dev.id = B00000000;
        dev.state = B00000000;
        dev.role = B00000000;
        return 1;
      }

      if (ackServ == 2){
        /*for (int i=0; i<6; i++){
         server.address[i] = rxbuff[i];
         }*/
        dev.serv = rxbuff[6];
        dev.id = rxbuff[7];
        dev.state = rxbuff[8];
        dev.role = rxbuff[9];
        return 2;
      }

    }
    else return 5;
  }
  else return 6;


}



