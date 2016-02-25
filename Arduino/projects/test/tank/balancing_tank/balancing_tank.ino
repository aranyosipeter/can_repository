

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
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


int16_t   acce[3];
int16_t   giro[3];
float   axisX      = 0;
float   axisY      = 0;

int     balanceMeas = -38;
float   actualMeas = 0;

float	kp = 0.05;
float	ki = 0.05;
float	kd = 1;

int	control = 0;			// PID kimenet
int     contDirect = 0;
float	err = 0; 			// hiba
float	err1 = 0;			// hiba 
int	t = 5;                       // derivalasi ido
float	outsum = 0;			// PID elemek osszege
float	sum = 0;			// hibak osszege *)
float	p = 0;                          // p elem 
float	i = 0;                          // i elem
float	d = 0;                          // d elem
int	error_code = 0;			// hibakod


void setup() {
    Wire.begin();
    Serial.begin(38400);
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void loop() {
    accelgyro.getAcceleration(&acce[0], &acce[1], &acce[2]);
    accelgyro.getRotation(&giro[0], &giro[1], &giro[2]);
    actualMeas = 0;
    //PID(&balanceMeas, &actualMeas, &control, &contDirect);
    ComplementaryFilter(acce, giro, &axisY, &axisX);
       /* Serial.print("a/g:\t");
        Serial.print(acce[0]); Serial.print("\t");
        Serial.print(acce[1]); Serial.print("\t");
        Serial.print(acce[2]); Serial.print("\t");
        Serial.print(giro[0]); Serial.print("\t");
        Serial.print(giro[1]); Serial.print("\t");
        Serial.println(giro[2]); Serial.print("\t");*/
        Serial.print("x/y:\t");
        Serial.print(axisX); Serial.print("\t");
        Serial.println(axisY);
        //Serial.println(control);
}

void PID(int *balance, float *actual, int *cont, int *direct){
	if (( (*cont) <= 0 ) && ( err < 0 )){
		(*cont) = 0;
		p = 0;
		i = 0;
		d = 0;
		sum = 0;
		outsum = 0;
		error_code = 1;
        }
	else if (( (*cont) >= 254 ) && ( err > 0 )){
		(*cont) = 254;
		p = 0;
		i = 0;
		d = 0;
		sum = 0;
		outsum = 0;
		error_code = 2;
        }
	else if ((( (*cont) >= 254 ) && ( err < 0 )) || 
                (( (*cont) < 254 ) && ( err >= 0 )) || 
                (( (*cont) < 254 ) && ( err <= 0 )) || 
                (( (*cont) > 0 ) && ( err <= 0 ))  || 
                (( (*cont) > 0 ) && ( err >= 0 )) || 
                (( (*cont) < 0 ) && ( err >= 0 ))) {
                
                if ( (*balance) > (*actual) ){
                  /* a tank a hatara akar dolni */
                  (*direct) = -1;
                  int conBalance = (*balance);
                  int conActual = (*actual);
                  err = abs(conActual) - abs(conBalance);
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
		  outsum = p + i + d;
		  (*cont) = outsum;
		  error_code = 0; 
                  Serial.print("direct: ");
                  Serial.println((*direct));
                  Serial.print("conBalance: ");
                  Serial.println(conBalance);
                  Serial.print("conActual: ");
                  Serial.println(conActual);
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
                else if ( (*balance) == (*actual) ){
                  p = 0;
		  i = 0;
		  d = 0;
		  sum = 0;
		  outsum = 0;
                  err = 0;
                  err1 = 0;
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
                } else if ( (*balance) < (*actual) ){
                  (*direct) = 1;
                  int conBalance = (*balance);
                  int conActual = (*actual);
                  err = abs(conBalance) + conActual;
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
	          outsum = p + i + d;
		  (*cont) = outsum;
		  error_code = 0;
                  Serial.print("direct: ");
                  Serial.println((*direct));
                  Serial.print("conBalance: ");
                  Serial.println(conBalance);
                  Serial.print("conActual: ");
                  Serial.println(conActual);
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
	        error_code = 3;
                }
   }
}

 
void ComplementaryFilter(int16_t accData[3], int16_t gyrData[3], float *roll, float *pitch)
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
        pitchAcc = atan2((float)accData[1], (float)accData[2]) * 180 / M_PI;
        *pitch = *pitch * 0.98 + pitchAcc * 0.02;
 
	// Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2((float)accData[0], (float)accData[2]) * 180 / M_PI;
        *roll = *roll * 0.98 + rollAcc * 0.02;
    }
} 
