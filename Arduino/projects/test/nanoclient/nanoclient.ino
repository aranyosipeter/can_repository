#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define LED 2
#define PIR 4
#define BUTTON 5
int pir=0;
int led=0;
void setup(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = 32;
  Mirf.config();
  pinMode(PIR, INPUT);
  pinMode(LED, OUTPUT);  
}

void loop(){
   voidPir();
   voidDataSend(pir);
} 

void voidPir(){
  
  if(digitalRead(PIR)==HIGH){
    pir=1;
    digitalWrite(LED, HIGH);
  }
  else{
    pir=0;
    digitalWrite(LED, LOW);
  }

} 

void voidDataSend(int pir){
  unsigned long alarm;
  alarm=pir;
  if(alarm==1){
  Mirf.setTADDR((byte *)"serv1");
  Mirf.send((byte *)&alarm);
  while(Mirf.isSending()){
  } 
  Mirf.getData((byte *) &alarm);
  delay(1000);
  }
}
  
  
