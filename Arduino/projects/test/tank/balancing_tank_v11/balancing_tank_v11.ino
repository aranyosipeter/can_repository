#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include  <math.h>

MPU6050 accelgyro;
#define OUTPUT_READABLE_ACCELGYRO
#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536 
#define M_PI 3.14159265359	     
#define dt 0.01	

#define RESET 3
#define IRQ 2
#define RIGHT_PWM 9
#define LEFT_PWM 10
#define RIGHT_FOR 16
#define RIGHT_BACK 17
#define LEFT_FOR 14
#define LEFT_BACK 15
#define STANDBY 4


int16_t         acce[3];
int16_t         giro[3];
float           axisX             = 0;
float           axisY             = 0;

float           balanceMeas       = 106;
float           actualMeas        = 0;

boolean         engage            = false;

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



void setup() {
    Wire.begin();
    Serial.begin(38400);
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    IOInit();
}

void loop() {
    accelgyro.getAcceleration(&acce[0], &acce[1], &acce[2]);
    accelgyro.getRotation(&giro[0], &giro[1], &giro[2]);
    ComplementaryFilter(acce, giro, &axisY, &axisX);
    if ( abs(axisY) > 60 || engage == true ){
      engage = true;
      PID(&balanceMeas, &axisY, &control, &contDirect);
      motorControl(&control, &contDirect);
    }
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

/****************************** I/O init ******************************/
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
/************************* Control of motors ****************************/
void motorControl(int *cont, int *direct){
    if ( (*direct) == 1 ){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(LEFT_FOR, HIGH);
    digitalWrite(LEFT_BACK, LOW);
    digitalWrite(RIGHT_FOR, HIGH);
    digitalWrite(RIGHT_BACK, LOW);
    analogWrite(LEFT_PWM, (*cont));
    analogWrite(RIGHT_PWM, (*cont));
    }
    if ( (*direct) == -1 ){
    digitalWrite(STANDBY, HIGH);
    digitalWrite(LEFT_FOR, LOW);
    digitalWrite(LEFT_BACK, HIGH);
    digitalWrite(RIGHT_FOR, LOW);
    digitalWrite(RIGHT_BACK, HIGH);
    analogWrite(LEFT_PWM, (*cont));
    analogWrite(RIGHT_PWM, (*cont));
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
