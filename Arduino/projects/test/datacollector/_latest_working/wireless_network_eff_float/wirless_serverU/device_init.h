// Initialize device connections/settings

/*
2015.09.16. [Performance] Use F() function to store strings in progmem
*/

#include "declarations.h"

Connector my_conn;        // The Connector/Arduino reference

/************************************* Communication Init *************************************/
void commInit(){
  Serial.println(F("Connecting..."));
  if (my_conn.mysql_connect(server_name, 3306, user, password)) {
    delay(1000);
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(GREENLED, HIGH);
  }
  else
    Serial.println(F("Connection failed."));
}

/*************************************** Mirf init *********************************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"servU");
  Mirf.setTADDR((byte *)"nodeU"); 
  Mirf.config();  
}




