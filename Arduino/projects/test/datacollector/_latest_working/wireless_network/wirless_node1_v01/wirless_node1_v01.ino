#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <dht11.h>

// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>


Adafruit_BMP085 bmp;
dht11 DHT11;

#define DHT11PIN 16
#define IRQ 2
#define ANA 17
/******************** Relays *********************/
#define RELAY1 9
#define RELAY2 6
#define RELAY3 5
#define RELAY4 4
#define RELAY5 3
#define RELAY6 7
#define RELAY7 1
#define RELAY8 0
/************************* Timers ******************/
long               time                  = 0;
long               sendTime              = 0;
long               prevTime              = 0;
boolean            dataRec               = false;
/************************* Flags *******************/

/*********************** Codes ********************/
int                 i, r                 = 0;
short               devID                = 3;
int                 photoR               = 3;
byte                command              = B00000001;
int                 dhtTemp              = 0;
int                 dhtHum               = 0;
int                 val                  = 0;
long                bmpPres              = 0;
long                bmpPresKPa           = 0;

float               dhtFah               = 0;
float               bmpTemp              = 0;
float               bmpAlt               = 0;
float               bmpRealAlt           = 0;

//boolean             switch1              = false;     // 1. szabadon felhasznalhato kapcsolo
//boolean             switch2              = false;     // 2. szabadon felhasznalhato kapcsolo
//boolean             switch3              = false;     // 3. szabadon felhasznalhato kapcsolo
boolean             relay[9]             = {false, false, false, false, false, false, false, false};
int                 relayPin[]           = {9, 6, 5, 4, 3, 7, 1, 0};
byte                relays               = B00000000; // relek allapota

/******************* Buffers for the transreceiver *****************/
int                 packetNum            = 0;
int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup() {
  pinMode(IRQ, INPUT);
  pinMode(ANA, INPUT);
  mirfInit();
  bmpInit();
  relayInit();
  Serial.begin(9600);
  Serial.println("Controller started!");
  attachInterrupt(0, voidResponse, LOW);
}

/************************** Loop *******************************/
void loop() {
  // DHT11 program 
  DHT11Check();
  voidDHTValue();
  
  // BMP085 program
  voidBMP085Value();
  
  // Photoresistor program
  voidPhoto();
  
  // Controlling relays
  relays = rxbuff[0];
  voidRelays();

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

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 81;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"node1");
  Mirf.setTADDR((byte *)"serwl");
  Mirf.config();
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
  Serial.println(val, DEC); 
}

/********************* BMP085 Value **************************/
void voidBMP085Value(){
  bmpTemp = bmp.readTemperature();
  bmpPres = bmp.readPressure();
  bmpPresKPa = bmpPres / 1000;
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

/******************** Generating packet ********************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = devID;
    txbuff[1] = command;
    txbuff[2] = packetNum;
    txbuff[3] = dhtTemp;
    txbuff[4] = dhtHum;
    txbuff[5] = bmpTemp;
    txbuff[6] = bmpPresKPa;
    txbuff[7] = val;
    txbuff[8] = 0;
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    voidPacketGen();
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    //Serial.println("Packet sent...");
}
 
/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
  }
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
}
/********************* Relays ***************************/
void voidRelays(){
  for ( r = 0; r < 9; r++ ){
    relay[r]  = bitRead(relays ,r);
  }
  for ( r = 0; r < 9; r++ ){
    if ( relay[r] == true ){
      digitalWrite(relayPin[r], LOW);
    }
    else digitalWrite(relayPin[r], HIGH);
  }
}

