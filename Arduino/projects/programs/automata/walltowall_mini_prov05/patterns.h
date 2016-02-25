#include "Arduino.h"

#define CUBESIZE 4
#define PLANESIZE CUBESIZE*CUBESIZE
#define PLANETIME 3333 
#define TIMECONST 5 

int LEDPin[] = {
  14, 10, 7, 3, 15, 11, 6, 2, 16, 12, 5, 0, 17, 13, 4, 1};
int LEDPinCount = 16;
int PlanePin[] = {
  18, 19, 8, 9};
int PlanePinCount  = 4;
int flag=0;
int l=0;
int button = 0;
volatile int options = 0;
volatile int preOptions = 0;

/******************** PlanesOff *********************/
void planesOff(){
  for(int thisPlane = 0; thisPlane < PlanePinCount; thisPlane++){
    digitalWrite(PlanePin[thisPlane],HIGH);
  }
}

/********************* CubeOff **********************/
void cubeOff(){
  int i;
  for(i=0; i<16; i++){
    digitalWrite(LEDPin[i],LOW);
  }
  for(i=0; i<4; i++){ 
    digitalWrite(PlanePin[i],HIGH);
  }
}

/********************* RandomLED **********************/
void randomLED(){
  // r - Rows, c - Colums randomize them | i - index variable | analogVal - value of analog pin reading | digitalVal - transformed value of the reading
  boolean plusMinus;
  byte r, c, i, j = 0;
  byte digitalVal = 0;
  int analogVal = 0; 
  
  for(i = 0; i < 16; i++){
    plusMinus = random(1);
    r = random(3);
    c = random(15);
    
    analogVal = analogRead(7);
    
    if (analogVal <= 255) digitalVal = 1;
    else if (analogVal <= 511) digitalVal = 2;
    else if (analogVal <= 765) digitalVal = 3;
    else if (analogVal <= 1023) digitalVal = 4;
    
    // Let's randomize them again in an unique way
    if (plusMinus) if (!((c + digitalVal) > 15)) c = c + digitalVal;
    else if (!((c - digitalVal) <= 0)) c = c - digitalVal;
    
    digitalWrite(PlanePin[r],LOW);
    for(j = 0; j < 50; j++){ 
      digitalWrite(LEDPin[c],HIGH);
      delay((digitalVal * 2));
      digitalWrite(LEDPin[c],LOW);
      delay((digitalVal * 2));
    }
    digitalWrite(PlanePin[r],HIGH);
    //delay(200);
  }
  
  cubeOff();
}
