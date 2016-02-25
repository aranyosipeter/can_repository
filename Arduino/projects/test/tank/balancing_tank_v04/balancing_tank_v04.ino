#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <math.h>


#define ACCELEROMETER_SENSITIVITY 8192.0
#define GYROSCOPE_SENSITIVITY 65.536
#define M_PI 3.14159265359	    
#define dt 0.01							// 10 ms sample rate!    
 
MPU6050 accelgyro;
#define OUTPUT_READABLE_ACCELGYRO

#define RESET 3
#define IRQ 2
#define RIGHT_PWM 9
#define LEFT_PWM 10
#define RIGHT_FOR 16
#define RIGHT_BACK 17
#define LEFT_FOR 14
#define LEFT_BACK 15
#define STANDBY 4

int16_t ax, ay, az;
int16_t gx, gy, gz;

int             balanceMeas       = -40;
float           actualMeas        = 0;

boolean         higher            = false;
boolean         lower             = false;

float           kp                = 0.05;        
float	        ki                = 0.05;
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
    accelgyro.getAcceleration(&ax, &ay, &az);
    actualMeas = az / 100;
    borderedPID(&balanceMeas, &actualMeas, &control, &contDirect);
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        //Serial.print(gx); Serial.print("\t");
        //Serial.print(gy); Serial.print("\t");
        //Serial.println(gz);
        Serial.println(control);
    motorControl(&control, &contDirect);
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
void borderedPID(int *balance, float *actual, int *cont, int *direct){
	if ( (*cont) < 0 ){
		(*cont) = 0;
        }
	else
        if ( (*cont) > 254 ){
		(*cont) = 254;
        }
	else
        if ((( (*cont) >= 254 ) && ( err < 0 )) || 
                (( (*cont) < 254 ) && ( err >= 0 )) || 
                (( (*cont) < 254 ) && ( err <= 0 )) || 
                (( (*cont) > 0 ) && ( err <= 0 ))  || 
                (( (*cont) > 0 ) && ( err >= 0 )) || 
                (( (*cont) < 0 ) && ( err >= 0 ))) {
                
                if ( (*balance) > (*actual) ){
                  higher = true;
                  lower = false;
                }
                if ( (*balance) < (*actual) ){
                  lower = true;
                  higher = false;
                }
                if ( higher == true && (*actual) == (* balance ) ){
                  (*direct) = 1;
                }
                if ( lower == true && (*actual) == (* balance ) ){
                  (*direct) = -1;
                }
                if ( (*balance) > (*actual) ){
                  /* a tank a hatara akar dolni */
                  err = abs((*balance)) - abs((*actual));
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
		  outsum = p + i + d;
		  (*cont) = outsum;
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
                if ( (*balance) < (*actual) ){
                  err = (*actual) - (*balance);
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
	          outsum = p + i + d;
		  (*cont) = outsum;
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

/************** Complementary Filter written by Pieter-Jan Van de Maele **********************/
void ComplementaryFilter(short accData[3], short gyrData[3], float *pitch, float *roll)
{
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
        pitchAcc = atan2f((float)accData[1], (float)accData[2]) * 180 / M_PI;
        *pitch = *pitch * 0.98 + pitchAcc * 0.02;
 
	// Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2f((float)accData[0], (float)accData[2]) * 180 / M_PI;
        *roll = *roll * 0.98 + rollAcc * 0.02;
    }
} 
