#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <dht11.h>
// GSM Comm
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
#include "call.h"

Adafruit_BMP085 bmp;
dht11 DHT11;

CallGSM call;
SMSGSM sms;

#define DHT11PIN 2
#define LED 6
/******************** Relays *********************/
#define RELAY1 7
#define RELAY2 8
#define RELAY3 9
#define RELAY4 10

/*********************** Codes ********************/
int                 i, r                 = 0;
int                 photoR               = A6;
float               dhtTemp              = 0;
float               dhtHum               = 0;
int                 val                  = 0;
float               light                = 0;
long                bmpPres              = 0;
float               bmpPresKPa           = 0;

float               dhtFah               = 0;
float               bmpTemp              = 0;
float               bmpAlt               = 0;
float               bmpRealAlt           = 0;

boolean             relay[4]             = {false, false, false, false};
int                 relayPin[]           = {7, 8, 9, 10};
byte                relays               = B00000000; // relek allapota

/************************ GSM Comm ******************************/
char number[20];
char number2[20];
byte person1 = 0;
byte person2 = 0;
int  value   = 0;
char value_str[5];

/************************** Setup *******************************/
void setup()  {
  pinMode(LED, OUTPUT);
  bmpInit();
  relayInit();
  Serial.begin(9600);
  Serial.println("Controller started!");
  if (gsm.begin(9600))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
}

/************************** Loop *******************************/
void loop(){
  // DHT11 program 
  DHT11Check();
  voidDHTValue();
  
  // BMP085 program
  voidBMP085Value();
  
  // Photoresistor program
  voidPhoto();
  
  // Controlling relays
  relays = 0;
  voidRelays();
  
  person1=call.CallStatusWithAuth(number,2,2);
  if(person1==CALL_INCOM_VOICE_AUTH){
    digitalWrite(LED, HIGH);
    Serial.println("Peti calling!!");
  } else
  if(person1==CALL_NONE){
    digitalWrite(LED, LOW);
  }  
  
  if ( light > 30 ){
    call.Call(2);
    digitalWrite(LED, HIGH);
  }
  else {
    digitalWrite(LED, LOW);
    call.HangUp();
  }
}

/*********************** Initalizing relays *******************/
void relayInit(){
  int i;
  for ( i=0; i<8; i++ ){
    pinMode(relayPin[i], OUTPUT);
  } 
  for ( i=0; i<8; i++){
    digitalWrite(relayPin[i], HIGH);
  }
}

/******************* Initalizing BMP085 **********************/
void bmpInit(){
    if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
}

/******************* DHT11 Check *******************/
void DHT11Check(){
  Serial.println("\n");

  int chk = DHT11.read(DHT11PIN);
  Serial.print("Read sensor: ");
  switch (chk)
  {
  case 0: 
    Serial.println("OK"); 
    break;
  case -1: 
    Serial.println("Checksum error"); 
    break;
  case -2: 
    Serial.println("Time out error"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
}

/****************** DHT value ***********************/
void voidDHTValue(){
  dhtTemp = DHT11.temperature;
  dhtHum = DHT11.humidity;
  dhtFah = Fahrenheit(DHT11.temperature);
  
  Serial.print("Humidity (%): ");
  Serial.println(dhtHum, DEC);
 
  Serial.print("Temperature (oC): ");
  Serial.println(dhtTemp, DEC);
 
  Serial.print("Temperature (oF): ");
  Serial.println((float)dhtFah, 2);
  Serial.println("\n");
}

/***************** Fahrenheit ***********************/
float Fahrenheit(int celsius){
  return 1.8 * celsius + 32;
}

/********************** Photoresistor ************************/
void voidPhoto(){
  val = analogRead(photoR);
  Serial.print("A fenyero: ");
  Serial.println(light, DEC);
  light =  (float)val / 10.24; 
}

/********************* BMP085 Value **************************/
void voidBMP085Value(){
  bmpTemp = bmp.readTemperature();
  bmpPres = bmp.readPressure();
  bmpPresKPa = (float)bmpPres / 1000;
  bmpAlt = bmp.readAltitude();
  bmpRealAlt = bmp.readAltitude(101500);
  
  Serial.print("Temperature = ");
  Serial.print((float)bmpTemp, 2);
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(bmpPres, DEC);
  Serial.println(" Pa");
  Serial.print("Altitude = ");
  Serial.print((float)bmpAlt, 2);
  Serial.println(" meters");
  Serial.print("Real altitude = ");
  Serial.print((float)bmpRealAlt, 2);
  Serial.println(" meters");
  Serial.println("\n"); 
}

/********************* Relays ***************************/
void voidRelays(){
  for ( r = 0; r < 5; r++ ){
    relay[r]  = bitRead(relays ,r);
  }
  for ( r = 0; r < 5; r++ ){
    if ( relay[r] == true ){
      digitalWrite(relayPin[r], LOW);
    }
    else digitalWrite(relayPin[r], HIGH);
  }
}

