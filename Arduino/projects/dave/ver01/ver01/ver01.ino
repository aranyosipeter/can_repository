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

int photoR = 3;
int dhtTemp = 0;
int dhtHum = 0;
int val = 0;
long bmpPres = 0;

float dhtFah = 0;
float bmpTemp = 0;
float bmpAlt = 0;
float bmpRealAlt = 0;


void setup() {
  pinMode(photoR, INPUT);
  mirfInit();
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
}

void loop() {
  /******************** DHT11 program *******************/
  DHT11Check();
  
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
  
  /******************** BMP085 program *******************/
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

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print((float)bmpAlt, 2);
  Serial.println(" meters");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  Serial.print((float)bmpRealAlt, 2);
  Serial.println(" meters");
  Serial.println("\n");
  
  /******************** Photoresistor program *******************/
  val = analogRead(photoR);
  Serial.print("A fenyero: ");
  Serial.println(val, DEC); 
  delay(2000);  
  
   mirfSend();
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

/***************** Fahrenheit ***********************/

float Fahrenheit(int celsius)
{
  return 1.8 * celsius + 32;
}

/******************* Initalizing MiRF ************************/

void mirfInit(){
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"devou");
  Mirf.payload = 32;
  Mirf.config();
}

void mirfSend(){
  Mirf.setTADDR((byte *)"devin");
  Mirf.send((byte *)&val);
  
  
}

