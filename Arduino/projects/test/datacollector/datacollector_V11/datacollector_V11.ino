#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <dht11.h>

#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
//#include <avr/pgmspace.h>
#include <stdlib.h>
#include <mysql.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define DHT11PIN 2
#define ID1 1
#define ID2 2
#define ID3 3
#define ID4 4

Connector my_conn;        // The Connector/Arduino reference
dht11 DHT11;
Adafruit_BMP085 bmp;

/*********************** Codes ********************/

int                 dhtTemp               = 0;
int                 dhtHum                = 0;
long                bmpPres               = 0;
int                 bmpPresKPa            = 0;
float               bmpTemp               = 0;

byte                mac_addr[]            = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress           server_addr(81,0,104,144);
char                server_name[]         = "www.ynfho.com";
char                user[]                = "ynfhocom_s";
char                password[]            = "watermelon";


//const char TEST_SELECT_QUERY[] = "SELECT * FROM world.city LIMIT 10";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
const char INSERT_DATA[] = "INSERT INTO arduino.sensor_data VALUES (%d, %s, %d)";
const char UPDATE[] = "UPDATE ynfhocom_s.sensors SET value=%d WHERE id=%d";  
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

void setup() {
  bmpInit();
  mirfInit();
  Ethernet.begin(mac_addr);
  Serial.begin(115200);
  commInit();
}
 
void loop() {
  if (( millis() % 2500 ) == 0 ){
  // DHT11 program 
  //voidDHTValue();
  // Datalog to SQL database
  //dataLogSQL();
  // BMP085 program
  //voidBMP085Value();
  mainProgram();
  }
}

/******************* Initalizing BMP085 **********************/
void bmpInit(){
    if (!bmp.begin()) {
    Serial.println("Sensor error!");
    while (1) {
    }
  }
}

/*************************** Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"tank1");
  Mirf.setTADDR((byte *)"cont1");
  Mirf.config();
}

/*********************** Communication Init ************************/
void commInit(){
  Serial.println("Connecting...");
  if (my_conn.mysql_connect(server_name, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}

/****************** DHT value ***********************
void voidDHTValue(){
    int chk = DHT11.read(DHT11PIN);
    dhtTemp = DHT11.temperature;
    dhtHum = DHT11.humidity;
    Serial.print("Humidity (%): ");
    Serial.println(dhtHum, DEC);
 
    Serial.print("Temperature (oC): ");
    Serial.println(dhtTemp, DEC);
    Serial.print("Sensor: ");
}

/********************* BMP085 Value **************************
void voidBMP085Value(){
    bmpTemp = bmp.readTemperature();
    bmpPres = bmp.readPressure();
    bmpPresKPa = bmpPres / 1000;
  
    Serial.print("Temperature = ");
    Serial.print((float)bmpTemp, 2);
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(bmpPresKPa, DEC);
    Serial.println(" KPa");
}

/*********************************** Data log to SQL database *********************************
void dataLogSQL(){

    char query[64];
    sprintf(query, UPDATE, dhtTemp, ID1);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, dhtHum, ID2);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, (int)bmpTemp, ID3);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, bmpPresKPa, ID4);
    my_conn.cmd_query_P(query);
    Serial.println();
    Serial.println("UPDATED!!");
}

/**************************************** Main program *************************************/
void mainProgram(){
    int chk = DHT11.read(DHT11PIN);
    dhtTemp = DHT11.temperature;
    dhtHum = DHT11.humidity;
    Serial.print("Humidity (%): ");
    Serial.println(dhtHum, DEC);
 
    Serial.print("Temperature (oC): ");
    Serial.println(dhtTemp, DEC);
    Serial.print("Sensor: ");
    
    bmpTemp = bmp.readTemperature();
    bmpPres = bmp.readPressure();
    bmpPresKPa = ( bmpPres / 1000 );
  
    Serial.print("Temperature = ");
    Serial.print((float)bmpTemp, 2);
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(bmpPresKPa, DEC);
    Serial.println(" KPa");
    short id = 1;
    short id2 = 2;
    short id3 = 3;
    short id4 = 4;
    char query[64];
    sprintf(query, UPDATE, dhtTemp, id);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, dhtHum, id2);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, (int)bmpTemp, id3);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, bmpPresKPa, id4);
    my_conn.cmd_query(query);
    Serial.println();
    Serial.println("UPDATED!!");
}
