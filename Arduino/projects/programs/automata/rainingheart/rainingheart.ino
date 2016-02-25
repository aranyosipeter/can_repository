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
int flag=0;
int l=0;

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
        loopColum();
        loopHeart(); 
}
void loopHeart()
{ 
      int i=0;
      int j=0;
      planesOff();
      
      for(i=0;i<4;i++){
      /*----------------Also szint----------------*/
       // 16., 15., 12. LEDs
       digitalWrite(PlanePin[i],LOW);
       digitalWrite(LEDPin[15],HIGH);
       digitalWrite(LEDPin[14],HIGH);
       digitalWrite(LEDPin[11],HIGH);
       
       delay(50);
       /*-------------Masodik szint-------------*/
       // 14-es LED
       digitalWrite(PlanePin[i],LOW);
       digitalWrite(LEDPin[13],HIGH);
       digitalWrite(LEDPin[7],HIGH);  
       delay(50);
       
       /*-------------Harmadik szint-----------*/
       // 3-as LED
       digitalWrite(PlanePin[i],LOW);
       digitalWrite(LEDPin[2],HIGH);
       digitalWrite(LEDPin[3],HIGH);
       digitalWrite(LEDPin[8],HIGH);
       digitalWrite(LEDPin[12],HIGH);
       delay(50);
       
       /*---------------Felso reteg--------------*/
       // 2-es LED
       digitalWrite(PlanePin[i],LOW);
       digitalWrite(LEDPin[1],HIGH);
       digitalWrite(LEDPin[5],HIGH);
       digitalWrite(LEDPin[4],HIGH);
       delay(50);
       
       
       if(i==3){
       delay(500);
       for(j=3;j>=0;j--){
       digitalWrite(PlanePin[i],HIGH);
       digitalWrite(LEDPin[15],LOW);
       digitalWrite(LEDPin[14],LOW);
       digitalWrite(LEDPin[11],LOW);
       //-----------------------------
       delay(100);  
       digitalWrite(PlanePin[i],HIGH);
       digitalWrite(LEDPin[13],LOW);
       digitalWrite(LEDPin[7],LOW);
       //------------------------------
       delay(100);
       digitalWrite(PlanePin[i],HIGH);
       digitalWrite(LEDPin[2],LOW);
       digitalWrite(LEDPin[3],LOW);
       digitalWrite(LEDPin[8],LOW);
       digitalWrite(LEDPin[12],LOW);
       //------------------------------
       delay(100);
       digitalWrite(PlanePin[i],HIGH);
       digitalWrite(LEDPin[1],LOW);
       digitalWrite(LEDPin[5],LOW);
       digitalWrite(LEDPin[4],LOW);
       delay(200);
       }
     }
    }
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
    for(i=0;i<64;i++)
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
