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
/******************** Relays *********************/
#define RELAY1 9
#define RELAY2 6
#define RELAY3 5
#define RELAY4 4
#define RELAY5 3
#define RELAY6 7
#define RELAY7 1
#define RELAY8 0

/************************* Flags *******************/

boolean   receive              = false;      // nincs hasznalatban
boolean   handShake            = false;      // kommunikacio szinkron flag

/*********************** Codes ********************/

int   i                        = 0;         // for ciklus valtozoja
int   photoR                   = 3;
int   dhtTemp                  = 0;
int   dhtHum                   = 0;
int   val                      = 0;
long  bmpPres                  = 0;

float   dhtFah                 = 0;
float   bmpTemp                = 0;
float   bmpAlt                 = 0;
float   bmpRealAlt             = 0;

boolean   switch1              = false;     // 1. szabadon felhasznalhato kapcsolo
boolean   switch2              = false;     // 2. szabadon felhasznalhato kapcsolo
boolean   switch3              = false;     // 3. szabadon felhasznalhato kapcsolo
byte      handS_sent           = B10101010; // szinkron bajt
byte      handS_recv           = B00000000; // szinkron bajt

/******************* Buffers for the transreceiver *****************/

int16_t txbuff[9];                        // kuldesre varo adatok tombje
int16_t rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup() {
  pinMode(IRQ, INPUT);
  pinMode(photoR, INPUT);
  mirfInit();
  bmpInit();
  Serial.begin(9600);
  Serial.println("Controller started!");
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
  
  // Transciever program
  Mirf.setRADDR((byte *)"cont1");
  Mirf.setTADDR((byte *)"dev01");
  while ( handShake == false ){
    Serial.println(" Handshaking!");
    voidHandShake();
   }
  voidPacketSend();
    //delay(100);
  //voidGetPacket();
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"devou");
  Mirf.setTADDR((byte *)"devin");
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

/********************* Handshake ****************************/
void voidHandShake(){
  Mirf.send((byte *)&handS_sent);
  while(Mirf.isSending()){
  }
  Serial.println(" Request for handshaking sent!"); 
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &handS_recv); 
  }
  if ( handS_sent == handS_recv ) {
    handShake = true;
    Mirf.send((byte *)&handS_sent);
    while(Mirf.isSending()){
    }
    Serial.println(" Handshake successful!"); 
  }
  else 
     Serial.println(" No connection between the receiver and the transmitter! "); 
}

/******************** Generating packet ********************/
void voidPacketGen(){
  // Creating packet
    txbuff[0] = dhtHum;
    Serial.print("0. bajt: ");
    Serial.println(txbuff[0], DEC);
    txbuff[1] = dhtTemp;
    Serial.print("1. bajt: ");
    Serial.println(txbuff[1], DEC);
    txbuff[2] = val;
    Serial.print("2. bajt: ");
    Serial.println(txbuff[2], DEC);
    txbuff[3] = bmpPres;
    Serial.print("3. bajt: ");
    Serial.println(txbuff[3], DEC);
    txbuff[4] = 0;
    Serial.print("4. bajt: ");
    Serial.println(txbuff[4], DEC);
    txbuff[5] = 0;
    Serial.print("5. bajt: ");
    Serial.println(txbuff[5], DEC);
    txbuff[6] = switch1;
    Serial.print("6. bajt: ");
    Serial.println(txbuff[6], DEC);
    txbuff[7] = switch2;
    Serial.print("7. bajt: ");
    Serial.println(txbuff[7], DEC);
    txbuff[8] = switch3;
    Serial.print("8. bajt: ");
    Serial.println(txbuff[8], DEC);
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    voidPacketGen();
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    Serial.println("Packet sent...");
}
 
/****************** Filling up server data *******************/
void voidGetPacket(){
  while( !Mirf.dataReady() ){
  }
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
    for ( i = 0; i < 9; i++ ){
       Serial.print(i);
      Serial.print(". bajt: ");
      Serial.println(rxbuff[i]);
    }
    Serial.println("Packet receiver: Packet is got! ");
  }
}
 

