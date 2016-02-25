#include <avr/pgmspace.h> // allows use of PROGMEM to store patterns in flash

#define CUBESIZE 4
#define PLANESIZE CUBESIZE*CUBESIZE
#define PLANETIME 3333 // time each plane is displayed in us -> 100 Hz refresh
#define TIMECONST 5 // multiplies DisplayTime to get ms - why not =100?

/*
** Defining pins in array makes it easier to rearrange how cube is wired
** Adjust numbers here until LEDs flash in order - L to R, T to B
** Note that analog inputs 0-5 are also digital outputs 14-19!
** Pin DigitalOut0 (serial RX) and AnalogIn5 are left open for future apps
*/

int LEDPin[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int LEDPinCount = 16;
int PlanePin[] = {16, 17, 18, 19};
int PlanePinCount  = 4;

// initialization
void setup()
{
  int pin; // loop counter
  // set up LED pins as output (active HIGH)
  for (pin=0; pin<PLANESIZE; pin++) {
    pinMode( LEDPin[pin], OUTPUT );
  }
  // set up plane pins as outputs (active LOW)
  for (pin=0; pin<CUBESIZE; pin++) {
    pinMode( PlanePin[pin], OUTPUT );
  }
}

void loop(){
  //loopFor();
  //loopRand();
   loopColum();
}

// the principles of using 4 planes and 16 pins - here we loop over each, turning on and off in turn
void loopFor()
{
    for(int thisPlane = 0; thisPlane < PlanePinCount; thisPlane++){
      for(int thisPin = 0; thisPin < LEDPinCount; thisPin++){
        
        planesOff();
        digitalWrite(LEDPin[thisPin],HIGH);
        digitalWrite(PlanePin[thisPlane],LOW);
       
        
        delay(100);
        
        digitalWrite(LEDPin[thisPin],LOW);
        digitalWrite(PlanePin[thisPlane],HIGH);
        
        
      }
    }
}

void loopRand()
{
    int randPin = random(15);
    int randPlane = random(3);
    
    planesOff();
    
    digitalWrite(LEDPin[randPin],HIGH);
    digitalWrite(PlanePin[randPlane],LOW);
        
    delay(50);
           
      
    digitalWrite(LEDPin[randPin],LOW);
    digitalWrite(PlanePin[randPlane],HIGH);
     
     
}
void loopColum()
{
    int i=0;
    int r=0;
    int k=0;
    int j=0;
    int ledArray[15];
    
    //Tömb feltöltése
    for(i=0;i<16;i++)
    {
      ledArray[i]=i;
    }
    
    // Tömb randomizálása
    for(i=0;i<31;i++)
    {
      r=random(15);
      if(r==0){
        k=ledArray[r];
        ledArray[r]=ledArray[r+1];
        ledArray[r+1]=k;
      }
      if(r==15){
        k=ledArray[r];
        ledArray[r]=ledArray[r-1];
        ledArray[r-1]=k;
      }
      if(r%2==0){
        k=ledArray[r];
        ledArray[r]=ledArray[r-2];
        ledArray[r-2]=k;
      }
      if(r%2==1){
        k=ledArray[r];
        ledArray[r]=ledArray[r+2];
        ledArray[r+2]=k;
      }
     
}
    planesOff();
   for(i=0;i<16;i++){
      for(j=3;j>=0;j--){
       digitalWrite(PlanePin[j],LOW);
       digitalWrite(LEDPin[ledArray[i]],HIGH);
       
       delay(50);
       
       digitalWrite(PlanePin[j],HIGH);
       digitalWrite(LEDPin[ledArray[i]],LOW);
       }
      }
}  

void planesOff(){
    for(int thisPlane = 0; thisPlane < PlanePinCount; thisPlane++){
        digitalWrite(PlanePin[thisPlane],HIGH);
    }
}
