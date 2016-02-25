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
float               txbuff[8];                        // kuldesre varo adatok tombje
float               rxbuff[8];                        // bejovo adatok tombje

/***********************************************************************/
/********************** End of declarations ****************************/

void setup() {
  Serial.begin(9600);
  mirfInit();
}

// the loop routine runs over and over again forever:
void loop() {
  packetSendTiming();
  controlDataProcess();
  //dataSendSerial();
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.cePin = 9;
  Mirf.csnPin = 10;
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
    rightValue = abs(joyLRValue);
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
    downValue = abs(joyUDValue);
    upValue = 0;
  }
  if ( joyUDValue < 0 ){ 
    upValue = abs(joyUDValue);
    downValue = 0;
  }
  
  // Calculate up and left-right sides
  if ( upValue > 0 ){
    acceRight = ( 2 * upValue );
    acceLeft = ( 2 * upValue );
    directRight = 1;
    directLeft = 1;
    if ( leftValue >= 0 ){
      acceRight = acceRight - ( 1 * leftValue );
      acceLeft = acceLeft + ( 1 * leftValue );
    }
    if (  rightValue >= 0 ){
      acceRight = acceRight + ( 1 * rightValue );
      acceLeft = acceLeft - ( 1 * rightValue );
    }
  } else
  // Calculate down left right sides
  if ( downValue > 0 ){
    acceRight = ( 2 * downValue );
    acceLeft = ( 2 * downValue );
    directRight = -1;
    directLeft = -1;
    if ( leftValue >= 0 ){
      acceRight = acceRight - ( 1 * leftValue );
      acceLeft = acceLeft + ( 1 * leftValue );
    }
    if ( rightValue >= 0 ){
      acceRight = acceRight + ( 1 * rightValue );
      acceLeft = acceLeft - ( 1 * rightValue );
    }
  } else
  // Calculate stand left right sides
  if ( upValue == 0 && downValue == 0 ){
    Serial.println("Here we go!");
    acceRight = 0;
    acceLeft = 0;
    directRight = 1;
    directLeft = 1;
    if ( leftValue >= 0 ){
      acceRight = acceRight - ( 1 * leftValue );
      acceLeft = acceLeft + ( 1 * leftValue );
    }
    if ( rightValue >= 0 ){
      acceRight = acceRight + ( 1 * rightValue );
      acceLeft = acceLeft - ( 1 * rightValue );
    }
  }
  // Limitations
  if ( acceRight > 255 ) acceRight = 255;
  if ( acceLeft > 255 ) acceLeft = 255;
  if ( acceRight < 0 ){
    if ( directRight == 1 ) directRight = -1;
    else
    if ( directRight == -1 ) directRight = 1;
  }
  if ( acceLeft < 0 ){
    if ( directLeft == 1 ) directLeft = -1;
    else
    if ( directLeft == -1 ) directLeft = 1;
  }
}

/*************************** Timing of sending packects ********************/
void packetSendTiming(){
  timer = millis() / 50;
  if ( timer != prevTimer ){
    voidPacketSend();
    prevTimer = timer;
  }
}

/************************** Print datas over Serial ************************/
void dataSendSerial(){
   Serial.print("Left value: ");
   Serial.println(acceLeft);
   Serial.print("Right value: ");
   Serial.println(acceRight);
   Serial.println();
   Serial.print("Up value(raw): ");
   Serial.println(upValue);
   Serial.print("Down value(raw): ");
   Serial.println(downValue);
   Serial.print("Left value(raw): ");
   Serial.println(leftValue);
   Serial.print("Right value(raw): ");
   Serial.println(rightValue);
   Serial.println();
   Serial.print("Left direction: ");
   Serial.println(directLeft);
   Serial.print("Right direction: ");
   Serial.println(directRight);
   Serial.println();
   delay(1000);
}
