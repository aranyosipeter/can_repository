#include <avr/pgmspace.h>

int relayPin[]={0,1,2,3,4,5,6,7};

void setup()
{
  int i;
  for (i=0;i<8;i++){
   pinMode(relayPin[i], OUTPUT);
  }
  
  for (i=0;i<8;i++){
   digitalWrite(relayPin[i], HIGH);
  }
}

void loop()
{
  knightrider();

}

void knightrider()
{
  int i;
  for(i=0;i<8;i++){
    digitalWrite(relayPin[i], LOW);
    delay(500);
    digitalWrite(relayPin[i], HIGH);
  }
} 
