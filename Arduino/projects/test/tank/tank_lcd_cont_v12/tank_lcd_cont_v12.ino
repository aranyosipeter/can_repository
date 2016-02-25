
/*********************************Controller pinout***************************
                
          (Button)LEFT_DCC_BUT    |Tx            RAW| NC
          (Button)LEFT_ACC_BUT    |Rx            GND| GND
                            NC    |RST           RST| NC
                           GND    |GND       A5  VCC| VCC
                                  |2         A4   A3| 
                                  |3              A2|
                                  |4              A1|
                                  |5              A0|
                                  |6              13|
                                  |7              12|
                                  |8              11|
                                  |9__ __ __ __ __10|
                      


******************************************************************************/
// These headers for LCD Display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  

// These headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define IRQ 2
#define MENU 20
#define LEFT_CONT 16
#define RIGHT_CONT 17
#define LEFT_ACC_BUT 0
#define LEFT_DCC_BUT 1
#define RIGHT_ACC_BUT 14
#define RIGHT_DCC_BUT 15

// LEDs
#define LEFT_ACC_LED 3 
#define LEFT_DCC_LED 5
#define LEFT_STD_LED 4
#define RIGHT_ACC_LED 6 
#define RIGHT_DCC_LED 10
#define RIGHT_STD_LED 9

//LCD Display
#define I2C_ADDR    0x27  
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define LED 13
#define  LED_OFF  1
#define  LED_ON  1

// Increasements
#define VALUE_1 1
#define VALUE_2 10
#define VALUE_3 100

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

/************************* Flags **************************/

boolean             dataRec                = false;      // nincs hasznalatban

/**************** Declaration of parameters ***************/

int                 j, k, s                = 0;         // for ciklus valtozoja

int                 leftSlide              = 0;
int                 rightSlide             = 0;
int                 prevLeft               = 0;
int                 prevRight              = 0;

/************    Read from physical buttons   *************/
boolean             leftAccBut             = false;
boolean             leftDecBut             = false;
boolean             rightAccBut            = false;
boolean             rightDecBut            = false;

/***********           Buttons, menu       ****************/
boolean             leftButtonAcc          = false;      // Left button for acceleration
boolean             leftButtonDec          = false;      // Left button for decreasing acceleration
boolean             rightButtonAcc         = false;      // Right button for acceleration
boolean             rightButtonDec         = false;      // Right button for decreasing acceleration
unsigned long       leftTimer              = 0;          // Button timer to avoid prell
unsigned long       rightTimer             = 0;          // Button timer to avoid prell

int                 leftCount              = 6;          // Counter for number of left button pushes
int                 rightCount             = 6;          // Counter for number of right button pushes
int                 prevRCount             = 0;          
int                 prevLCount             = 0;

int                 analogButton           = 0;
int                 actualOpt              = 0;
int                 workingOpt             = 0;
boolean             menu                   = false;
boolean             nextOpt                = false;
boolean             prevOpt                = false;
boolean             optOpen                = false;
boolean             optClose               = false;
boolean             selectOpt              = false;

boolean             menuEnable             = false;
boolean             optionEnab             = false;
boolean             menuButton             = false;
boolean             buttonDec              = false;
boolean             buttonInc              = false;
boolean             buttonOpen             = false;
boolean             buttonClose            = false;
unsigned long       menuTimer              = 0;
unsigned long       buttonTimer            = 0;
unsigned long       optionTimer            = 0;

int                 timer                  = 0;
int                 prevTimer              = 0;

byte                command                = B00000000; // egyeb parancsok
int                 directRight            = 0;        // jobboldali lanctalp iranya
int                 directLeft             = 0;        // baloldali lanctalp iranya
int                 acceRight              = 0;        // jobboldali lanctalp PWM jele
int                 acceLeft               = 0;        // baloldai lanctalp PWM jele

int                 prevAcceLeft           = 0;        // sebesseg mutatasa
int                 prevAcceRight          = 0;        // sebesseg mutatasa

int                 leftLEDs               = 0;        // baloldali LED-ek PWM jele
int                 rightLEDs              = 0;        // jobboldali LED-ek PWM jele
byte                lifeSignal             = B00000000;
boolean             LCDBacklight           = 0;
boolean             SetBacklight           = false;
boolean             backlightValue         = 0;

/********************** Balancing tank menu *************************/

int                 balancingOpt           = 0;
int                 controllerType         = 1;
int                 increasementType       = 0;
short               increasementValue      = VALUE_1;
boolean             balancingMenu          = true;
boolean             parametersSend         = false;
boolean             valuePlusChange        = false;
boolean             valueMinusChange       = false;

/*************** Setting values of PID for balancing ****************/

int                 kp                     = 0;
int                 ki                     = 0;
int                 kd                     = 0;
int                 kpm                    = 0;
int                 kim                    = 0;
int                 kdm                    = 0;                    

/******************* Buffers for the transreceiver *****************/

int                 txbuff[9];                        // kuldesre varo adatok tombje
int                 rxbuff[9];                        // bejovo adatok tombje

/***********************************************************************/
/********************** End of declarations ****************************/

/************************** Setup *******************************/
void setup()  {
  Serial.begin(9600);
  Wire.begin();
  IOInit();
  mirfInit();
  LCDInit();
  attachInterrupt(0,voidResponse, LOW);
}

/************************** Loop *******************************/
void loop()   {
  menuSelect();
  //slideController();
  LEDControl();
  speedOMeter();
  //backLightControl();
  lcd.setBacklight(LCDBacklight);
}

/********************** LCD Init *****************************/
void LCDInit(){
  lcd.begin (16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LCDBacklight);
}

/********************** I/O Init *****************************/
void IOInit(){
  pinMode(IRQ, INPUT);
  pinMode(MENU, INPUT);
  
  // Defining slide controllers
  pinMode(LEFT_CONT, INPUT);
  pinMode(RIGHT_CONT, INPUT);
  
  // Defining buttons
  pinMode(LEFT_ACC_BUT, INPUT);
  pinMode(LEFT_DCC_BUT, INPUT);
  pinMode(RIGHT_ACC_BUT, INPUT);
  pinMode(RIGHT_DCC_BUT, INPUT);
  
  //Defining LED's  
  pinMode(LEFT_ACC_LED, OUTPUT);
  pinMode(LEFT_DCC_LED, OUTPUT);
  pinMode(LEFT_STD_LED, OUTPUT);
  pinMode(RIGHT_ACC_LED, OUTPUT);
  pinMode(RIGHT_DCC_LED, OUTPUT);
  pinMode(RIGHT_STD_LED, OUTPUT);  
}
/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
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
    if ( parametersSend == true ){
      txbuff[5] = kp;
      txbuff[6] = ki;
      txbuff[7] = kd;
      parametersSend = false; 
    }else {
      txbuff[5] = 0;
      txbuff[6] = 0;
      txbuff[7] = 0;
    }
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
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

/********************* Controlling with sliders **********************/
void slideController(){ 
  dataCollect();
  // Backward 
  if (( rightSlide < 384 ) && ( rightSlide >= 0 )){
    directRight = -1;
    acceRight = (( 384 - rightSlide ) / 3 );
  }
  if (( leftSlide < 384 ) && ( leftSlide >= 0 )){
    directLeft = -1;
    acceLeft = (( 384 - leftSlide )  / 3 );
  }
  // Forward
  if (( rightSlide <= 1023 ) && ( rightSlide >= 640 )){
    directRight = 1;
    acceRight = (( rightSlide - 640 )  / 3 );
  }
  if (( leftSlide <= 1023 ) && ( leftSlide >= 640 )){
    directLeft = 1;
    acceLeft = (( leftSlide - 640 ) / 3 );
  }
  // Standby
  if (( rightSlide > 384 ) && ( rightSlide < 640 )){
    directRight = 0;
    acceRight = 0;
  }
  if (( leftSlide > 384 ) && ( leftSlide < 640 )){
    directLeft = 0;
    acceLeft = 0;
  }
}

/****************** Controlling with buttons ******************/
void digController(){
  if ( menuEnable == false ) {
    leftAccBut = digitalRead(LEFT_ACC_BUT);
    leftDecBut = digitalRead(LEFT_DCC_BUT);
    rightAccBut = digitalRead(RIGHT_ACC_BUT);
    rightDecBut = digitalRead(RIGHT_DCC_BUT);
    
    if ( leftAccBut == HIGH ){
      leftButtonAcc = true;
      leftTimer = millis();
    }
    
    if ( leftDecBut == HIGH ){
      leftButtonDec = true;
      leftTimer = millis();
    }
    
    if ( rightAccBut == HIGH ){
      rightButtonAcc = true;
      rightTimer = millis();
    }
    
    if ( rightDecBut == HIGH ){
      rightButtonDec = true;
      rightTimer = millis();
    }
    
    buttonTiming(&leftTimer, &leftButtonAcc, &leftButtonDec, &leftCount);
    buttonTiming(&rightTimer, &rightButtonAcc, &rightButtonDec, &rightCount);
    
    if ( leftCount <= 1 ){ 
      leftCount = 1;
    }
    if ( leftCount >= 12 ){
      leftCount = 12;
    }
    if ( rightCount <= 1 ){
      rightCount = 1;
    }
    if ( rightCount >= 12 ){
      rightCount = 12;
    }
    switch (leftCount){
      case 1:
          acceLeft = 150;
          directLeft = -1;
          break;
      case 2:
          acceLeft = 120;
          directLeft = -1;
          break;
      case 3:
          acceLeft = 90;
          directLeft = -1;
          break;
      case 4:
          acceLeft = 60;
          directLeft = -1;
          break;
      case 5:
          acceLeft = 30;
          directLeft = -1;
          break; 
      case 6:  
          acceLeft = 0;
          directLeft = 0;
          break;
      case 7:  
          acceLeft = 30;
          directLeft = 1;
          break;
      case 8:
          acceLeft = 60;
          directLeft = 1;
          break;
      case 9:
          acceLeft = 90;
          directLeft = 1;
          break;
      case 10:
          acceLeft = 120;
          directLeft = 1;
          break;
      case 12:
          acceLeft = 150;
          directLeft = 1;
          break;
    }
    
    switch (rightCount){
       case 1:
          acceRight = 150;
          directRight = -1;
          break;
      case 2:
          acceRight = 120;
          directRight = -1;
          break;
      case 3:
          acceRight = 90;
          directRight = -1;
          break;
      case 4:
          acceRight = 60;
          directRight = -1;
          break;
      case 5:
          acceRight = 30;
          directRight = -1;
          break; 
      case 6:  
          acceRight = 0;
          directRight = 0;
          break;
      case 7:  
          acceRight = 30;
          directRight = 1;
          break;
      case 8:
          acceRight = 60;
          directRight = 1;
          break;
      case 9:
          acceRight = 90;
          directRight = 1;
          break;
      case 10:
          acceRight = 120;
          directRight = 1;
          break;
      case 12:
          acceRight = 150;
          directRight = 1;
          break;
    }
  }
}

/************************ Menu select *************************/
void menuSelect(){
  analogButton = analogRead(MENU);
  Serial.print("AnalogButton: ");
  Serial.println(analogButton, DEC);
  if ( analogButton > 768 ){
    menuTimer = millis();
    menuButton = true;
    Serial.println("timerEnable true"); 
    Serial.print("menuTimer: ");
    Serial.println(menuTimer);  
  }
  menuTiming(&menuTimer, &menuButton , &menuEnable); 
  
  if ( menuEnable == true ){
    acceRight = 0;
    acceLeft = 0;
    directRight = 0;
    directLeft = 0;
    nextOpt = digitalRead(RIGHT_ACC_BUT);
    prevOpt = digitalRead(RIGHT_DCC_BUT);
    //selectOpt = digitalRead(RIGHT_DCC_BUT);
    Serial.print("Actual option: ");
    Serial.println(actualOpt);
    if ( nextOpt == true ){ 
      buttonTimer = millis();
      buttonInc = true;
    }
    if ( prevOpt == true ){
      buttonTimer = millis();
      buttonDec = true;
    }
    buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &actualOpt);
    if ( actualOpt < 0 ) actualOpt = 0;
    if ( actualOpt > 3 ) actualOpt = 3;   
    switch (actualOpt) {
      case 0: 
              lcd.setCursor(1,0);
              lcd.print("Analog mode    ");
              Serial.println("Analog mode    ");
              break;
      case 1: 
              lcd.setCursor(1,0);
              lcd.print("Digital mode   ");
              Serial.println("Digital mode    ");
              break;
      case 2: 
              if ( LCDBacklight == 0 ){
              lcd.setCursor(1,0);
              lcd.print("Backlight: OFF  ");
              Serial.println("Backlight: OFF  ");
              }
              else if ( LCDBacklight == 1 ){
              lcd.setCursor(1,0);
              lcd.print("Backlight: ON   ");
              Serial.println("Backlight: ON   ");
              }   
              break;
      case 3: 
              lcd.setCursor(1,0);
              lcd.print("Balancing mode  ");
              Serial.println("Balancing mode  ");
              break;        
     /*default: 
              lcd.setCursor(1,0);
              lcd.print("No more option!");
              Serial.println("No more    ");
              break;*/
    }
    /*if ( selectOpt == true ){
      workingOpt = actualOpt;
      menuEnable = false;
    }*/

  }
  else workingOpt = actualOpt;
  /*else { 
    //lcd.clear();
  }*/
  
  if (( workingOpt == 0 ) && ( menuEnable == false )) {
    slideController();
  }
  if (( workingOpt == 1 ) && ( menuEnable == false )){
    digController();
  }
  else {
    rightCount = 6;
    leftCount = 6;
  }
  if (( workingOpt == 2 ) && ( menuEnable == false )) {
    backLightControl();
  }
  if (( workingOpt == 3 ) && ( menuEnable == false )) {
    balancingTank();
  }
  else balancingMenu = true; 
}

/******************** Menu timer section **********************/
void menuTiming(unsigned long *timer, boolean *button, boolean *menu){
  if ( (*button) == true ) {
    Serial.println("Button is pushed!");
      Serial.print("timer: ");
      Serial.println((*timer));    
    if ( ((*timer) + 100 ) < millis() ) {
      (*menu) = !(*menu) ;
      Serial.print("Menu: ");
      Serial.println((*menu));
      (*button) = false;
      Serial.print("Button: ");
      Serial.println((*button));
      lcd.clear();
    }
  }
}

/*******************Button timer section **********************/
void buttonTiming(unsigned long *timer, boolean *buttonUp, boolean *buttonDown, int *value){
  Serial.print("ButtonUP: ");
  Serial.println(*buttonUp);
  Serial.print("ButtonDown: ");
  Serial.println(*buttonDown);
  Serial.print("timer: ");
  Serial.println(*timer);
  Serial.print("value: ");
  Serial.println(*value);
  if ( (*buttonUp) == true ) {
    if (( (*timer) + 100 ) < millis() ) {
      (*buttonUp) = false;
      (*value)++;
    }
  } 
   if ( (*buttonDown) == true ) {
    if (( (*timer) + 100 ) < millis() ) {
      (*buttonDown) = false;
      (*value)--;
    }
  } 
}

/*******************Button timer section **********************/
void optionEnable(unsigned long *timer, boolean *optionOpen, boolean *optionClose, boolean *enable){

  if ( (*optionOpen) == true ) {
    if (( (*timer) + 100 ) < millis() ) {
      (*optionOpen) = false;
      (*enable) = true;
    }
  } 
   if ( (*optionClose) == true ) {
    if (( (*timer) + 100 ) < millis() ) {
      (*optionClose) = false;
      (*enable) = false;
    }
  } 
      Serial.print("optionOpen: "); 
    Serial.println((*optionOpen)); 
    Serial.print("enable: "); 
    Serial.println((*enable)); 
}

/*********************** Data collect *************************/
void dataCollect(){
  rightSlide = analogRead(RIGHT_CONT);
  leftSlide = analogRead(LEFT_CONT); 
}

/********************* LED control ***************************/
void LEDControl(){
  //leftLEDs = acceLeft / 1.5;
  //rightLEDs = acceRight / 1.5;
  // Right side of the controller
  if ( directRight == 0 ){
    analogWrite(RIGHT_ACC_LED, 0);
    analogWrite(RIGHT_DCC_LED, 0);
    digitalWrite(RIGHT_STD_LED, HIGH);
  }
  if ( directRight == 1 ){
    analogWrite(RIGHT_ACC_LED, acceRight);
    analogWrite(RIGHT_DCC_LED, 0);
    digitalWrite(RIGHT_STD_LED, LOW);
  }
  if ( directRight == -1 ){
    analogWrite(RIGHT_ACC_LED , 0);
    analogWrite(RIGHT_DCC_LED , acceRight);
    digitalWrite(RIGHT_STD_LED , LOW);
  }
  // Left side of the controller
  if ( directLeft == 0 ){
    analogWrite(LEFT_ACC_LED, 0);
    analogWrite(LEFT_DCC_LED, 0);
    digitalWrite(LEFT_STD_LED, HIGH);
  }
  if ( directLeft == 1 ){
    analogWrite(LEFT_ACC_LED, acceLeft);
    analogWrite(LEFT_DCC_LED, 0);
    digitalWrite(LEFT_STD_LED, LOW);
  }
  if ( directLeft == -1 ){
    analogWrite(LEFT_ACC_LED, 0);
    analogWrite(LEFT_DCC_LED, acceLeft);
    digitalWrite(LEFT_STD_LED, LOW);
  }
}

/************************* SpeedOMeter ****************************/
void speedOMeter(){
      if (( acceLeft != prevAcceLeft ) || ( acceRight != prevAcceRight )){
       if ( acceLeft >= 0 && acceLeft < 10 ){
          lcd.setCursor(0,0);
          lcd.print("  ");
          lcd.setCursor(2,0);
          lcd.print(acceLeft, DEC);
          lcd.setCursor(3,0);
          lcd.print("          ");
       }
       else 
       if ( acceLeft >= 10 && acceLeft < 100 ){
         lcd.setCursor(0,0);
         lcd.print(" ");
         lcd.setCursor(1,0);
         lcd.print(acceLeft);
         lcd.setCursor(3,0);         
         lcd.print("          ");
       }
       else
       if ( acceLeft >= 100 ){
         lcd.setCursor(0,0);
         lcd.print(acceLeft);
         lcd.setCursor(3,0);
         lcd.print("          ");
       }
      prevAcceLeft = acceLeft;
      
      ////////////// Right side /////////////////////
      if ( acceRight >= 0 && acceRight < 10 ){
          lcd.setCursor(13,0);
          lcd.print("  ");
          lcd.setCursor(15,0);
          lcd.print(acceRight, DEC);
       }
       else 
       if ( acceRight >= 10 && acceRight < 100 ){
         lcd.setCursor(13,0);
         lcd.print(" ");
         lcd.setCursor(14,0);
         lcd.print(acceRight);
       }
       else
       if ( acceRight >= 100 ){
         lcd.setCursor(13,0);
         lcd.print(acceRight);
       }
      prevAcceRight = acceRight;
      
 }
}

/****************** Controlling LCD Backlight **************************/
void backLightControl(){
  if ( menuEnable == false ){
    LCDBacklight = !LCDBacklight;
    lcd.setBacklight(LCDBacklight);
    actualOpt = 0;
    menuEnable = true;
  }
}

/******************* Controlling Balacing Tank ************************/
void balancingTank(){
    nextOpt = digitalRead(RIGHT_ACC_BUT);
    prevOpt = digitalRead(RIGHT_DCC_BUT);
    optOpen = digitalRead(LEFT_ACC_BUT);
    optClose = digitalRead(LEFT_DCC_BUT);
    if ( nextOpt == true ){ 
      buttonTimer = millis();
      buttonInc = true;
    }
    if ( prevOpt == true ){
      buttonTimer = millis();
      buttonDec = true;
    }
    if ( !optionEnab )
    buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &balancingOpt);
    
    if ( optOpen == true ){ 
      optionTimer = millis();
      buttonOpen = true;
    }
    if ( prevOpt == true ){
      optionTimer = millis();
      buttonClose = true;
    }
    optionEnable(&optionTimer, &buttonOpen, &buttonClose, &optionEnab);
    Serial.println("---------------------------------------------------------"); 
    Serial.print("buttonOpen: "); 
    Serial.println(buttonOpen); 
    Serial.print("buttonClose: "); 
    Serial.println(buttonClose); 
    Serial.print("optionEnab: ");
    Serial.println(optionEnab);   
    if ( balancingMenu ){
      float conkP = (float)kp / 100;
      float conkI = (float)ki / 100;
      float conkD = (float)kd / 100;
      if ( balancingOpt > 6 ) balancingOpt = 6;
      if ( balancingOpt < 0 ) balancingOpt = 0;
      switch (balancingOpt){
        case 0: 
                if ( optionEnab ) {
                  buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &controllerType);
                } 
                if ( controllerType == 0 ){
                  lcd.setCursor(1,0);
                  lcd.print("Controller: A   ");
                  Serial.println("Controller: A   "); 
                } else
                if ( controllerType == 1 ){
                  lcd.setCursor(1,0);
                  lcd.print("Controller: D   ");
                  Serial.println("Controller: D   "); 
                }  
                break;
        case 1: 
                if ( optionEnab ) {
                  buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &increasementType);
                } 
                if ( increasementType == 0 ){
                  lcd.setCursor(1,0);
                  lcd.print("Increase: 0.01  ");
                  Serial.println("Increase: 0.01  "); 
                  increasementValue = VALUE_1;
                } else
                if ( increasementType == 1 ){
                  lcd.setCursor(1,0);
                  lcd.print("Increase: 0.10  ");
                  Serial.println("Increase: 0.10  "); 
                  increasementValue = VALUE_2;
                } else
                if ( increasementType == 2 ){
                  lcd.setCursor(1,0);
                  lcd.print("Increase: 1.00  ");
                  Serial.println("Increase: 1.00  "); 
                  increasementValue = VALUE_3;
                }
                break;
        case 2: 
                if ( optionEnab ) {
                  buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &kpm);
                  if ( buttonInc == true ) valuePlusChange = true;
                  else
                  if ( buttonDec == true ) valueMinusChange = true;
                }
                if ( kpm <= 0 ) kpm = 1;
                if ( valuePlusChange == true ) {
                  kp = kp + ( kpm * increasementValue );
                  valuePlusChange = false;
                }
                if ( valueMinusChange == true ) {
                  kp = kp - ( kpm * increasementValue );
                  valueMinusChange = false;
                }
                lcd.setCursor(1,0);
                lcd.print("kP:       ");
                lcd.setCursor(9,0);
                lcd.print(conkP);
                Serial.println("kP:       "); 
                break;
        case 3: 
                if ( optionEnab ) {
                  buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &kim);
                  if ( buttonInc == true ) valuePlusChange = true;
                  else
                  if ( buttonDec == true ) valueMinusChange = true;
                }
                if ( kim <= 0 ) kim = 1;  
                if ( valuePlusChange == true ) {
                  ki = ki + ( kim * increasementValue );
                  valuePlusChange = false;
                }
                if ( valueMinusChange == true ) {
                  ki = ki - ( kim * increasementValue );
                  valueMinusChange = false;
                }        
                lcd.setCursor(1,0);
                lcd.print("kI:       ");
                lcd.setCursor(9,0);
                lcd.print(conkI);
                Serial.println("kI:       "); 
                break;
        case 4: 
                if ( optionEnab ) {
                  buttonTiming(&buttonTimer, &buttonInc, &buttonDec, &kdm);
                  if ( buttonInc == true ) valuePlusChange = true;
                  else
                  if ( buttonDec == true ) valueMinusChange = true;
                }
                if ( kdm < 0 ) kdm = 0;
                if ( valuePlusChange == true ) {
                  kd = kd + ( kdm * increasementValue );
                  valuePlusChange = false;
                }
                if ( valueMinusChange == true ) {
                  kd = kd - ( kdm * increasementValue );
                  valueMinusChange = false;
                }      
                lcd.setCursor(1,0);
                lcd.print("kD:       ");
                lcd.setCursor(9,0);
                lcd.print(conkD);
                Serial.println("kD:       "); 
                break; 
        case 5: 
                if ( optionEnab ) {
                  parametersSend = true;
                  optionEnab = false;
                }
                lcd.setCursor(1,0);
                lcd.print("Save/Send       ");
                Serial.println("Save/Send       "); 
                break;   
        case 6: 
                if ( optionEnab ) {
                  optionEnab = false;
                  balancingMenu = false;
                  menuEnable = true;
                }
                lcd.setCursor(1,0);
                lcd.print("Back            ");
                Serial.println("Back            "); 
                break;                
      }
    
  }
}
