

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

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

int             lowBorder         = -36;
int             highBorder        = -44;

float           kp                = 0.1;        
float	        ki                = 0.1;
float	        kd                = 1;

int	        control           = 0;			// PID kimenet
int             contDirect        = 0;
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
    borderedPID(&balanceMeas, &lowBorder, &highBorder, &actualMeas, &control, &contDirect);
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
void borderedPID(int *balance, int *low_border, int *high_border, float *actual, int *cont, int *direct){
	/*if (( (*cont) <= 0 ) && ( err < 0 )){
		(*cont) = 0;
		p = 0;
		i = 0;
		d = 0;
		sum = 0;
		outsum = 0;
        }
	else
        if (( (*cont) >= 254 ) && ( err > 0 )){
		(*cont) = 254;
		p = 0;
		i = 0;
		d = 0;
		sum = 0;
		outsum = 0;
        }
	else*/
        if ((( (*cont) >= 254 ) && ( err < 0 )) || 
                (( (*cont) < 254 ) && ( err >= 0 )) || 
                (( (*cont) < 254 ) && ( err <= 0 )) || 
                (( (*cont) > 0 ) && ( err <= 0 ))  || 
                (( (*cont) > 0 ) && ( err >= 0 )) || 
                (( (*cont) < 0 ) && ( err >= 0 ))) {
                
                if ( (*balance) > (*actual) ){
                  /* a tank a hatara akar dolni */
                  (*direct) = -1;
                  int conLowBorder = (*low_border);
                  int conActual = (*actual);
                  err = conLowBorder - conActual;
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
		  outsum = p + i + d;
		  (*cont) = outsum;
                  Serial.print("direct: ");
                  Serial.println((*direct));
                  Serial.print("conLowBorder: ");
                  Serial.println(conLowBorder);
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
                else 
                if ( (*balance) == (*actual) ){
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
                } else 
                if ( (*balance) < (*actual) ){
                  (*direct) = 1;
                  int conHighBorder = (*high_border);
                  int conActual = (*actual);
                  err = abs(conHighBorder) + conActual;
		  p = kp * err;
		  sum = sum + err;
		  i = ki * sum;
		  d = kd * (err - err1) / t;
		  err1 = err;
	          outsum = p + i + d;
		  (*cont) = outsum;
                  Serial.print("direct: ");
                  Serial.println((*direct));
                  Serial.print("conHighBorder: ");
                  Serial.println(conHighBorder);
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
