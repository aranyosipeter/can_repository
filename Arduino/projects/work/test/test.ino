#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;

int temp=0;
int hours=0;
int inputs[]={2, 3, 4, 5, 6, 7, 8, 9};
int outputs[]={10, 11};


void setup () {
    Serial.begin(57600);
    Wire.begin();
    RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  int pin;
  for (pin=0; pin<7; pin++){
    pinMode( inputs[pin], INPUT );
        }
  for (pin=0; pin<2; pin++) {
    pinMode( outputs[pin], OUTPUT );   
        }
}

void loop () {
    
  realTimeClock();
  
}   
void realTimeClock() {
   DateTime now = RTC.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    
  
    Serial.println();
    delay(3000);
  }  
    
void () {
  
  
  }
  
  
  
}
