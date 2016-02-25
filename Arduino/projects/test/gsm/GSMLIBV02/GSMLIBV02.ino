#include "SIM900.h"
#include <SoftwareSerial.h>
//We don't need the http functions. So we can disable the next line.
//#include "inetGSM.h"
#include "sms.h"
#include "call.h"
#define RELAY_1 10
#define RELAY_2 9
#define GREENLED 5
#define REDLED 6
//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to check if an incoming call is from an authorized
//number and in this case, send to this number an SMS with the value
//of a digital input.

//We have to create the classes for SMSs and calls.
CallGSM call;
SMSGSM sms;


char number[20];
char number2[20];
byte person1 = 0;
byte person2 = 0;
int  value   = 0;
char value_str[5];

void setup() {
  relayInit();
  pinMode(GREENLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(9600))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
};

void loop() 
{
  //Chekcs status of call
  person1=call.CallStatusWithAuth(number,1,1);
  //If the incoming call is from an authorized number
  //saved on SIM in the positions range from 1 to 3.
  if(person1==CALL_INCOM_VOICE_AUTH){
    //Check the value of the input.
    digitalWrite(RELAY_1, LOW);
    digitalWrite(GREENLED, HIGH);
    Serial.println("Picim calling!!");
  } else
  if(person1==CALL_NONE){
    //Check the value of the input.
    digitalWrite(RELAY_1, HIGH);
    digitalWrite(GREENLED, LOW);
  }  
  delay(1000);
  person2=call.CallStatusWithAuth(number2,2,2);
  if(person2==CALL_INCOM_VOICE_AUTH){
    digitalWrite(RELAY_2, LOW);
    digitalWrite(GREENLED, HIGH);
    Serial.println("Peti calling!!");
  } else
  if(person2==CALL_NONE){
    digitalWrite(RELAY_2, HIGH);
    digitalWrite(GREENLED, LOW);
  }
  delay(1000);
}

/*********************** Initalizing relays *******************/
void relayInit(){
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
}

