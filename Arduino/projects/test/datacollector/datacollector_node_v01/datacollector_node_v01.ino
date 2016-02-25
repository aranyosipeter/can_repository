// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define BUTTON 3
#define REDLED 4
#define GREENLED 5


/************************* Timers ******************/
long               time                  = 0;
long               sendTime              = 0;
long               prevTime              = 0;

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

int16_t txbuff[8];                        // kuldesre varo adatok tombje
int16_t rxbuff[8];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup() {
  pinMode(IRQ, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  mirfInit();
  //Serial.begin(9600);
  Serial.println("Controller started!");
  attachInterrupt(0, voidGetPacket, FALLING);
}

/************************** Loop *******************************/
void loop() {
  
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.csnPin = 8;
  Mirf.cePin = 9;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"node1");
  Mirf.setTADDR((byte *)"servr");
  Mirf.config();
}

/*********************** Generating packet ************************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = command;
    //Serial.print("0. bajt: ");
    //Serial.println(txbuff[0], DEC);
    txbuff[1] = buttonValue;
    //Serial.print("1. bajt: ");
    //Serial.println(txbuff[1], DEC);
    txbuff[2] = 0;
    //Serial.print("2. bajt: ");
    //Serial.println(txbuff[2], DEC);
    txbuff[3] = 0;
    //Serial.print("3. bajt: ");
    //Serial.println(txbuff[3], DEC);
    txbuff[4] = 0;
    //Serial.print("4. bajt: ");
    //Serial.println(txbuff[4], DEC);
    txbuff[5] = 0;
    //Serial.print("5. bajt: ");
    //Serial.println(txbuff[5], DEC);
    txbuff[6] = 0;
    //Serial.print("6. bajt: ");
    //Serial.println(txbuff[6], DEC);
    txbuff[7] = 0;
    //Serial.print("7. bajt: ");
    //Serial.println(txbuff[7], DEC);
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    voidPacketGen();
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    //Serial.println("Packet sent...");
}
 
/********************* Filling up server data *************************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
  }
  receive_Command = rxbuff[0];
}

/************************ Response **************************/
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
  buttonEnable(&buttonTimer, &buttonPush,  &buttonValue);
}

/********************** Button timer section **************************/
void buttonEnable(unsigned long *timer, boolean *button,  boolean *buttonVal){

  if ( (*button) == true ) {
    if (( (*timer) + 100 ) < millis() ) {
      (*button) = false;
      (*buttonVal) = true;
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
    transmit == false;
  }
  if (( millis() % 5000 ) == 0 ){
    if ( transmitNum > 5 ) digitalWrite(GREENLED, HIGH);
    if ( transmitNum <= 5 ) digitalWrite(GREENLED, LOW);
    transmitNum = 0;
  }
}

/*************************** Response LED ****************************/
void responseLED(){
  if ( sendFlag == true ){
    flashTime = millis();
    digitalWrite(REDLED, HIGH);
  }
  if ( millis() > ( flashTime + 50 )){
    sendFlag = false;
    digitalWrite(REDLED, LOW);
  }
}
