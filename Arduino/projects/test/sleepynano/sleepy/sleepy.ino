#include <avr/sleep.h>

#define LED 4
#define PIR 2
#define BUTTON 5
#define INTERRUPT 3

int i = 0;                   // counter


void arming(){
}

void setup(){
  pinMode(PIR, INPUT);
  pinMode(INTERRUPT, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(0, arming, HIGH); 
  Serial.println("Waiting for the Pir");
  for (i=0; i<6; i++){
    digitalWrite(LED, HIGH);
    delay(25);
    digitalWrite(LED, LOW);
  }
}

void sleepNow(){
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   
  sleep_enable();         
  attachInterrupt(0,arming, HIGH); 
  sleep_mode();            
  // Here we're coming!!!
  sleep_disable();  
  detachInterrupt(0);    
}

void loop(){
  delay(1000);
  Serial.println("Ohh F*UCK!! Somebody is in your house who's NOT you, IDIOT!!'");
  for (i=0; i<11; i++){
    digitalWrite(LED, HIGH);
    delay(5);
    digitalWrite(LED, LOW);
    delay(5);
    digitalWrite(LED, HIGH);
    delay(20);
    digitalWrite(LED, LOW);
    Serial.println("Niiiinoooo!!!");
  }
  Serial.println("Ohh I'm tired, going to sleep!"); 
  delay(500);
  sleepNow();     
}


