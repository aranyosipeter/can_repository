#define joyLRControl 1
#define joyUDControl 2

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

/************************* Flags **************************/
boolean             dataRec                = false;      // nincs hasznalatban
boolean             dataSent               = false;      

/******************* Controlling of tank *******************/
byte                command                = B00000000; // egyeb parancsok
short               directRight            = 0;        // jobboldali lanctalp iranya
short               directLeft             = 0;        // baloldali lanctalp iranya
short               acceRight              = 0;        // jobboldali lanctalp PWM jele
short               acceLeft               = 0;        // baloldai lanctalp PWM jele
short               joyLRValue = 0;
short               joyUDValue = 0;
short               leftValue = 0;
short               rightValue = 0;
short               upValue = 0;
short               downValue = 0;

byte                lifeSignal             = B00000000;

/************************ Timers of control ***********************/
int                 timer                  = 0;
int                 prevTimer              = 0;

/******************* Buffers for the transreceiver *****************/
short               txbuff[8];                        // kuldesre varo adatok tombje
short               rxbuff[8];                        // bejovo adatok tombje

/***********************************************************************/
/********************** End of declarations ****************************/

void setup() {
  Serial.begin(9600);
  mirfInit();
}

// the loop routine runs over and over again forever:
void loop() {
  packetSandTiming();
  controlDataProcess();
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"cont1");
  Mirf.setTADDR((byte *)"tank1");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = command;
    txbuff[1] = directRight;
    txbuff[2] = directLeft;
    txbuff[3] = acceRight;
    txbuff[4] = acceLeft;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    dataSent = true;
}

/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
  }
  lifeSignal = rxbuff[0];
}

/************************ Response **************************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}

/************************* Data process **********************/
void controlDataProcess(){
  // read from the inputs
  joyLRValue = ((analogRead(joyLRControl)) / 10) - 51;
  joyUDValue = ((analogRead(joyUDControl)) / 10) - 52;
  // decide 
  if ( joyLRValue == 0 ){
    rightValue = 0;
    leftValue = 0;
  } else
  if ( joyLRValue > 0 ){
    rightValue = joyLRValue;
    leftValue = 0;
  } else
  if ( joyLRValue < 0 ){ 
    leftValue = abs(joyLRValue);
    rightValue = 0;
  }
  
  if ( joyUDValue == 0 ){
    downValue = 0;
    upValue = 0;
    
  }
  if ( joyUDValue > 0 ){ 
    downValue = joyUDValue;
    upValue = 0;
  }
  if ( joyUDValue < 0 ){ 
    upValue = abs(joyUDValue);
    downValue = 0;
  }
  
  acceRight = ( 5 * upValue ) + ( 5 * rightValue ) - (5 * leftValue);
  acceLeft = ( 5 * upValue ) + ( 5 * leftValue ) - (5 * rightValue);
}
/*************************** Timing of sending packects ********************/
void packetSandTiming(){
  timer = millis() / 500;
  if ( timer != prevTimer ){
    voidPacketSend();
    prevTimer = timer;
  }
}

