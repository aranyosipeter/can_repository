#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define BUTTON 3
#define REDLED 4
#define GREENLED 5

/********************* Timers *********************/
short               tries                = 0;
unsigned long       prevTimer            = 0;
unsigned long       mill                 = 0;
unsigned long       taskNum              = 0;
unsigned long       timer                = 0;
unsigned long       timerValue           = 0;

/*********************** Codes ********************/
byte                devID                = B00000010;
byte                command              = B00000000;
byte                receiveCommand       = B00000000;
byte                receiveID            = B00000000;
byte                packetNum            = B00000000;
byte                maxNum               = B00000000;

short               transmitNum          = 0;

boolean             transmit             = false;
boolean             sendFlag             = false;
boolean             buttonPush           = false;
boolean             buttonValue          = false;
boolean             dataRec              = false;            
unsigned long       flashTime            = 0;
unsigned long       buttonTimer          = 0;


/******************* Received datas *********************/
int                 dhtTemp              = 0;
int                 dhtHum               = 0;
int                 val                  = 0;
int                 bmpTemp              = 0;
int                 bmpPresKPa           = 0;

/************ Buffers for the transreceiver ************/
boolean             nodes[8]             = { false, false, false, false, false, false, false, false };
int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup(){
  pinMode(IRQ, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  mirfInit();
  Serial.begin(9600);
  Serial.println("Controller started!"); 
  attachInterrupt(0, voidGetPacket, LOW);
}

/************************** Loop *******************************/
void loop(){
  mill = millis();
  buttonChange();
  transmissionLED();
  responseLED();
  Serial.println(receiveCommand);
  
  if ( taskNum == 0 ){
           Serial.println("node1: Communicate!");
           Mirf.channel = 81;
           Mirf.init();
           Mirf.config();
           Mirf.setTADDR((byte *)"node1"); 
           command = 0;
           while (( tries < 5 ) && ( dataRec == false )){           
             voidPacketSend(&command);
             timerValue = millis();
             while (( timerValue + 200 ) > millis() ){
             }
             tries++;
             Serial.print("Number of tries: ");
             Serial.println(tries);
           }
           if ( dataRec == true ) nodes[0] = true;           
           dataRec = false;
           tries = 0;
           taskNum++;
           Serial.println("node1: Done!"); 

  } else
  if ( taskNum == 1 ){
           Serial.println("node2: Communicate!");
           Mirf.channel = 82;
           Mirf.init();
           Mirf.config();
           Mirf.setTADDR((byte *)"node2");
           command = 1;
           while (( tries < 5 ) && ( dataRec == false )){           
             voidPacketSend(&command);
             timerValue = millis();
             while (( timerValue + 200 ) > millis() ){
             }
             tries++;
             Serial.print("Number of tries: ");
             Serial.println(tries); 
           }
           //if ( dataRec == true ) nodes[1] = true;             
           dataRec = false;
           tries = 0;
           taskNum++;
           Serial.println("node2: Done!");
  } else 
  if ( taskNum == 2 ){
    Serial.print("Receiced command: ");
    Serial.println(receiveCommand);
    Serial.print("DHTTemp: ");
    Serial.println(dhtTemp);
    Serial.print("DHTHum: ");
    Serial.println(dhtHum);
    Serial.print("BMPTemp: ");
    Serial.println(bmpTemp);
    Serial.print("BMPPress: ");
    Serial.println(bmpPresKPa);
    Serial.print("Value: ");
    Serial.println(val);
    taskNum = 0;
  }
} 

/************************* Mirf init ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.cePin = 8;
  Mirf.csnPin = 9;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serwl");
  Mirf.setTADDR((byte *)"serdb"); 
  Mirf.config();  
}

/******************** Function for sending packet ********************/
void voidPacketSend(byte *option){
    txbuff[0] = devID;
    if ( (*option) == 0 ){
      txbuff[1] = command;
      txbuff[2] = 0;
      txbuff[3] = 0;
      txbuff[4] = 0;
      txbuff[5] = 0;
      txbuff[6] = 0;
      txbuff[7] = 0;
      txbuff[8] = 0;
    } else
    if ( (*option) == 1 ){
      txbuff[1] = command;
      txbuff[2] = dhtTemp;
      txbuff[3] = dhtHum;
      txbuff[4] = bmpTemp;
      txbuff[5] = bmpPresKPa;
      txbuff[6] = val;
      txbuff[7] = 0;
      txbuff[8] = 0;
    }else
    if ( (*option) == 2 ){
      txbuff[1] = command;
      txbuff[2] = packetNum;
      txbuff[3] = maxNum;
      txbuff[4] = dhtTemp;
      txbuff[5] = dhtHum;
      txbuff[6] = bmpTemp;
      txbuff[7] = bmpPresKPa;
      txbuff[8] = val;
    }
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    transmit = true;
}
  
/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);
  }
  receiveID = rxbuff[0];
  receiveCommand = rxbuff[1];
  if ( receiveCommand == 1 ){
    packetNum = rxbuff[2];
    dhtTemp = rxbuff[3];
    dhtHum = rxbuff[4];
    bmpTemp = rxbuff[5];
    bmpPresKPa = rxbuff[6];
    val = rxbuff[7];
  }
  sendFlag = true;
  dataRec = true;
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend(&command);
}

/********************** Changing button value *************************/
void buttonChange(){
  if ( digitalRead(BUTTON) == true ) {
    buttonPush = true;
    buttonTimer = millis();
  }
  buttonEnable(&buttonTimer, &buttonPush, &buttonValue);
}

/********************** Button timer section **************************/
void buttonEnable(unsigned long *timer, boolean *button, boolean *buttonVal){

  if ( (*button) == true ) {
    if (( (*timer) + 100 ) < millis() ) {
      (*button) = false;
      (*buttonVal) = !(*buttonVal);
    }
  } 
    Serial.print("button: "); 
    Serial.println((*button)); 
    Serial.print("buttonVal: "); 
    Serial.println((*buttonVal)); 
}

/********************** Transmission LED *******************************/
void transmissionLED(){
  if ( transmit == true ){
    transmitNum++;
    transmit = false;
  }
  Serial.print("num: "); 
  Serial.println(transmitNum);
  timer = ( mill / 5000 );
  if ( timer != prevTimer){
    if ( transmitNum > 5 ) digitalWrite(GREENLED, HIGH);
    if ( transmitNum <= 5 ) digitalWrite(GREENLED, LOW);
    transmitNum = 0;
    prevTimer = timer;
  }
}

/*************************** Response LED ****************************/
void responseLED(){
  if ( sendFlag == true ){
    flashTime = millis();
    digitalWrite(REDLED, HIGH);
    sendFlag = false;
  }
  if ( millis() > ( flashTime + 50 )){
    digitalWrite(REDLED, LOW);
  }
}
  
