#include "SIM900.h"
#include <SoftwareSerial.h>
//#include "GSM.h"
//#include "inetGSM.h"
//#include "sms.h"
//#include "call.h"

//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to communicate with SIM900 through AT commands.

//InetGSM inet;
//CallGSM call;
//SMSGSM sms;

int numdata;
char inSerial[150] = "kurvaanyad";
int i=0;
boolean sendCommand = true;

void setup(){
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  if (gsm.begin(9600))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
}

void loop(){
  if (sendCommand){
    gsm.SimpleWriteln("AT+CENG=1");
    delay(200);
    Serial.println(F("Command sent"));
    sendCommand = false;
  }
  //gsm.SimpleWriteln("AT+CENG?");
  //delay(50);
  //gsm.readComm(inSerial, 148);
  //gsm.read(inSerial, 148);
  gsm.SendATCmdWaitResp("AT+CENG?", 10000, 50, "OK", 1);
  Serial.println();
  Serial.print(F("Eredmeny: "));
  Serial.println(inSerial);
  delay(3000);
}
