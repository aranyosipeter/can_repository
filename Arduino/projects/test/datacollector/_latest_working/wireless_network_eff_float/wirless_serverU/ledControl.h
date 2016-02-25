// LED control methods

/*
2015.08.04 [General]: Created/add header file 
2015.08.13 [Mod]: Create method to blink LEDs
*/

void blinkLED(byte color, byte times, boolean onOrOff){
 for (int i = 0; i < times; i++){
   if (onOrOff) digitalWrite(color, LOW); else digitalWrite(color, HIGH);
   delay(2000);
   if (onOrOff) digitalWrite(color, HIGH); else digitalWrite(color, LOW);
   delay(1000);
 }
}
