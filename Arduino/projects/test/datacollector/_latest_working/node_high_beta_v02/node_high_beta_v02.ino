#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define BUTTON 3
#define REDLED 4
#define GREENLED 5

/********************* Timers *********************/
unsigned long       mill                 = 0;
int                 timer                = 0;
int                 prevTimer            = 0;

/*********************** Codes ********************/
byte                identifier           = B00000010;
byte                command              = B00000000;
byte                receive_Command      = B00000000;
boolean             transmit             = false;
short               transmitNum          = 0;
boolean             sendFlag             = false;
unsigned long       flashTime            = 0;
boolean             buttonPush           = false;
boolean             buttonValue          = false;
unsigned long       buttonTimer          = 0;

/******************* Buffers for the transreceiver *****************/
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
  attachInterrupt(0, voidResponse, LOW);
}

/************************** Loop *******************************/
void loop(){
  mill = millis();
  buttonChange();
  transmissionLED();
  responseLED();
  Serial.println(receive_Command);
} 

/************************* Mirf init ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 10;
  Mirf.cePin = 8;
  Mirf.csnPin = 9;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"node1");
  Mirf.setTADDR((byte *)"serv1"); 
  Mirf.config();  
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = identifier;
    txbuff[1] = command;
    txbuff[2] = buttonValue;
    txbuff[3] = 0;
    txbuff[4] = 0;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    txbuff[8] = 0;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    transmit = true;
}
  
/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);
    receive_Command = rxbuff[0];
  }
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
  sendFlag = true;
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
  
