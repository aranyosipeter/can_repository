#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include  <math.h>

MPU6050 accelgyro;
#define OUTPUT_READABLE_ACCELGYRO
#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536 
#define M_PI 3.14159265359	     
#define dt 0.01	

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define RESET 3
#define IRQ 2
#define RIGHT_PWM 9
#define LEFT_PWM 10
#define RIGHT_FOR 16
#define RIGHT_BACK 17
#define LEFT_FOR 14
#define LEFT_BACK 15
#define STANDBY 4
/*************************** Timer values ************************/
unsigned long      tmpTimer                = 0;
unsigned long      time                    = 0;

/****************************** Flags ****************************/
boolean             dataRec                = false;

/****************************** Gyro *****************************/
int16_t         acce[3];
int16_t         giro[3];
float           axisX             = 0;
float           axisY             = 0;

/****************************** PID *******************************/
float           kp                = 0.1;        
float	        ki                = 0.01;
float	        kd                = 0.5;

int	        control           = 0;			// PID kimenet
int             contDirect        = -1;
float	        err               = 0; 			// hiba
float	        err1              = 0;			// hiba 
int	        t                 = 5;                  // derivalasi ido
float	        outsum            = 0;			// PID elemek osszege
float	        sum               = 0;			// hibak osszege *)
float	        p                 = 0;                  // p elem 
float	        i                 = 0;                  // i elem
float	        d                 = 0;                  // d elem
  
/**************************** Control ******************************/  
float           balanceMeas       = 106;
float           actualMeas        = 0;

boolean         engage            = false;

/*********************** Declaration of parameters ********************/
int                 j, k, s                = 0;         // for ciklus valtozoja

boolean             switch1                = false;     // 1. szabadon felhasznalhato kapcsolo
boolean             switch2                = false;     // 2. szabadon felhasznalhato kapcsolo
boolean             switch3                = false;     // 3. szabadon felhasznalhato kapcsolo

unsigned long       mills                  = 0;
unsigned long       subtract               = 0;
unsigned long       timer                  = 0;
unsigned long       prevTimer              = 0;

byte                command                = 00000000;
int                 directRight            = 0;
int                 directLeft             = 0;
int                 acceRight              = 0;
int                 acceLeft               = 0;

byte                lifeSignal             = B00000000;
int                 resetCount             = 0;
/******************* Buffers for the transreceiver *****************/
int             txbuff[9];                        // kuldesre varo adatok tombje
int             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup()  {
  Serial.begin(9600);
  mirfInit();
  IOInit();
  GyroInit();
  attachInterrupt(0,voidGetPacket, LOW);
  digitalWrite(RESET, HIGH);
}

/************************** Loop *******************************/
void loop()   {
  timer = millis() / 500;
  if ( timer != prevTimer ){
    voidPacketSend();
    prevTimer = timer;
    resetCount++;
    Serial.print("resetCount: ");
    Serial.println(resetCount, DEC);
    Serial.print("acceRight: ");
    Serial.println(acceRight, DEC);
    Serial.print("acceLeft: ");
    Serial.println(acceLeft, DEC);
  }
  if ( resetCount > 2 ){
    mirfInit();
    Serial.println("Mirf started!");
    resetCount = 0;
  }
  voidProcess();
  voidSerialData();
  tankControl();
}

/***************************** Gyro init ******************************/
void GyroInit(){
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

/***************************** I/O init ******************************/
void IOInit(){
  pinMode(IRQ, INPUT);
  pinMode(RESET, OUTPUT);
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_FOR, OUTPUT);
  pinMode(RIGHT_BACK, OUTPUT);
  pinMode(LEFT_FOR, OUTPUT);
  pinMode(LEFT_BACK, OUTPUT);
  pinMode(STANDBY, OUTPUT);
}
/*************************** Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"tank1");
  Mirf.setTADDR((byte *)"cont1");
  Mirf.config();
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = lifeSignal;
    txbuff[1] = 0;
    txbuff[2] = 0;
    txbuff[3] = 0;
    txbuff[4] = 0;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    txbuff[8] = 0;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
}

/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
  }
  resetCount = 0;
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}

/******************* Processing data *******************/
void voidProcess(){
  command     = rxbuff[0];
  directRight = rxbuff[1];
  directLeft  = rxbuff[2];
  acceRight   = rxbuff[3];
  acceLeft    = rxbuff[4]; 
}

/******************************************- Serial data ************************************/
void voidSerialData(){
  if ( dataRec == true ){
    Serial.print("Jobb oldali irany: ");
    Serial.println(directRight, DEC);
    Serial.print("Bal oldali irany: ");
    Serial.println(directLeft, DEC);
    Serial.print("Jobb oldali seb: ");
    Serial.println(acceRight, DEC);
    Serial.print("Bal oldali seb: ");
    Serial.println(acceLeft, DEC);
    dataRec = false;  
  } 
}

/********************** Tank control ************************/
void tankControl(){ 
  // Standby
  if ((directRight == 0) && (directLeft == 0)){
    digitalWrite(STANDBY, LOW);
    digitalWrite(LEFT_FOR, LOW);
    digitalWrite(LEFT_BACK, LOW);
    digitalWrite(RIGHT_FOR, LOW);
    digitalWrite(RIGHT_BACK, LOW);
    analogWrite(LEFT_PWM,  0);
    analogWrite(RIGHT_PWM, 0);
    //Serial.println("nem megyunk");
  }
  // Going forward
  if (directRight > 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(RIGHT_BACK, LOW);
    digitalWrite(RIGHT_FOR, HIGH);
    analogWrite(RIGHT_PWM, acceRight);
    //Serial.println("jobbra elre megyunk");
  }
  if (directLeft > 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(LEFT_FOR, HIGH);
    digitalWrite(LEFT_BACK, LOW);
    analogWrite(LEFT_PWM, acceLeft);
    //Serial.println("balra elre megyunk");
  }
  
  // Going backward
  if (directRight < 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(RIGHT_FOR, LOW);
    digitalWrite(RIGHT_BACK, HIGH);
    analogWrite(RIGHT_PWM, acceRight);
    //Serial.println("jobbra hatra megyunk");
  }
  if (directLeft < 0){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(LEFT_FOR, LOW);
    digitalWrite(LEFT_BACK, HIGH);
    analogWrite(LEFT_PWM, acceLeft);
    //Serial.println("balra hatra megyunk");
  }
}

void GyroRead(){
  if (( millis() % 10 ) == 0 ){
    accelgyro.getAcceleration(&acce[0], &acce[1], &acce[2]);
    accelgyro.getRotation(&giro[0], &giro[1], &giro[2]);
    Serial.print("a/g:\t");
    Serial.print(acce[0]); Serial.print("\t");
    Serial.print(acce[1]); Serial.print("\t");
    Serial.print(acce[2]); Serial.print("\t");
    Serial.print(giro[0]); Serial.print("\t");
    Serial.print(giro[1]); Serial.print("\t");
    Serial.println(giro[2]); Serial.print("\t");
    Serial.print("x/y:\t");
    Serial.print(axisX); Serial.print("\t");
    Serial.println(axisY);
    Serial.print("control: ");
    Serial.println(control);
  }
}


 ComplementaryFilter(acce, giro, &axisY, &axisX);
    if ( abs(axisY) > 60 || engage == true ){
      engage = true;
      PID(&balanceMeas, &axisY, &control, &contDirect);
      motorControl(&control, &contDirect);
    }
    
/**************************** PID Controller ****************************/
void PID(float *balance, float *actual, int *cont, int *direct){
	if (( (*cont) < 0 ) && ( err < 0 )){
		(*cont) = 0;
		/*p = 0;
		i = 0;
		d = 0;
		sum = 0;
		outsum = 0;*/
        }
	else if (( (*cont) > 254 ) && ( err > 0 )){
		(*cont) = 254;
		/*p = 0;
		i = 0;
		d = 0;
		sum = 0;
		outsum = 0;*/
        }
	else if ((( (*cont) >= 254 ) && ( err < 0 )) || 
                (( (*cont) < 254 ) && ( err >= 0 )) || 
                (( (*cont) < 254 ) && ( err <= 0 )) || 
                (( (*cont) > 0 ) && ( err <= 0 ))  || 
                (( (*cont) > 0 ) && ( err >= 0 )) || 
                (( (*cont) < 0 ) && ( err >= 0 ))) {
                
                if ( (*balance) > abs((*actual)) ){
                  /* a tank a hatara akar dolni */
                  (*direct) = 1;
                  err = (*balance) - abs((*actual));
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
		  outsum = p + i + d;
		  (*cont) = abs(outsum);
                  Serial.print("direct: ");
                  Serial.println((*direct));
                  Serial.print("err: ");
                  Serial.println(err);
                  Serial.print("p: ");
                  Serial.println(p);
                  Serial.print("i: ");
                  Serial.println(i);
                  Serial.print("d: ");
                  Serial.println(d);
                  Serial.print("sum: ");
                  Serial.println(sum);
                  Serial.print("outsum: ");
                  Serial.println(outsum);
                }
                else 
                if ( (*balance) < abs((*actual)) ){
                  (*direct) = -1;
                  err = (*balance) - abs((*actual));
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
	          outsum = p + i + d;
		  (*cont) = abs(outsum);
                  Serial.print("direct: ");
                  Serial.println((*direct));
                  Serial.print("err: ");
                  Serial.println(err);
                  Serial.print("p: ");
                  Serial.println(p);
                  Serial.print("i: ");
                  Serial.println(i);
                  Serial.print("d: ");
                  Serial.println(d);
                  Serial.print("sum: ");
                  Serial.println(sum);
                  Serial.print("outsum: ");
                  Serial.println(outsum);
	        }
                else {
	        (*cont) = 0;
                }
   }
}

/******** Complementary Filter written by Pieter-Jan Van de Maele ********/
void ComplementaryFilter(int16_t accData[3], int16_t gyrData[3], float *roll, float *pitch){
    
  float pitchAcc, rollAcc;               
 
    // Integrate the gyroscope data -> int(angularSpeed) = angle
    *pitch += ((float)gyrData[0] / GYROSCOPE_SENSITIVITY) * dt; // Angle around the X-axis
    *roll -= ((float)gyrData[1] / GYROSCOPE_SENSITIVITY) * dt;    // Angle around the Y-axis
 
    // Compensate for drift with accelerometer data if !bullshit
    // Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
    int forceMagnitudeApprox = abs(accData[0]) + abs(accData[1]) + abs(accData[2]);
    if (forceMagnitudeApprox > 8192 && forceMagnitudeApprox < 32768)
    {
	// Turning around the X axis results in a vector on the Y-axis
        pitchAcc = atan2((float)accData[1], (float)accData[2]) * 180 / M_PI;
        *pitch = *pitch * 0.98 + pitchAcc * 0.02;
 
	// Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2((float)accData[0], (float)accData[2]) * 180 / M_PI;
        *roll = *roll * 0.98 + rollAcc * 0.02;
    }
}     
    
    
