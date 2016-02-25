#include <dht11.h>

#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include <avr/pgmspace.h>
//#include <avr/dtostrf.h>  // Add this for the Due
#include <stdlib.h>
#include <mysql.h>

#define DHT11PIN 3

/*********************** Codes ********************/

int                 i, r                 = 0;         // for ciklus valtozoja
int                 dhtTemp              = 0;
int                 dhtHum               = 0;
long                bmpPres              = 0;
long                bmpPresKPa           = 0;

float               dhtFah               = 0;
float               bmpTemp              = 0;
float               bmpAlt               = 0;
float               bmpRealAlt           = 0;

byte               mac_addr[]            = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress          server_addr(81,0,104,144);
char               server_name[]         = "www.ynfho.com";
char               user[]                = "ynfhocom_s";
char               password[]            = "watermelon";

Connector my_conn;        // The Connector/Arduino reference
dht11 DHT11;

//const char TEST_SELECT_QUERY[] = "SELECT * FROM world.city LIMIT 10";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
const char INSERT_DATA[] = "INSERT INTO arduino.sensor_data VALUES (%d, %s, %d)";
const char UPDATE[] = "UPDATE ynfhocom_s.sensors SET value=%d WHERE id=%d";  
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

void setup() {
  DHT11Check();
  Ethernet.begin(mac_addr);
  Serial.begin(115200);
  commInit();
}
 
void loop() {
  // DHT11 program 
  voidDHTValue();  
  // Datalog to SQL database
  dataLogSQL();
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
  if (( millis() % 13000 ) == 0 ){
  Serial.print("Humidity (%): ");
  Serial.println(dhtHum, DEC);
 
  Serial.print("Temperature (oC): ");
  Serial.println(dhtTemp, DEC);
 
  Serial.print("Temperature (oF): ");
  Serial.println((float)dhtFah, 2);
  Serial.println("\n");
  }
}

/***************** Fahrenheit ***********************/
float Fahrenheit(int celsius){
  return 1.8 * celsius + 32;
}

/*********************************** Data log to SQL database *********************************/
void dataLogSQL(){
  if (( millis() % 5000 ) == 0 ){
    int id = 1;
    int id2 = 2;
    char query[64];
    sprintf(query, UPDATE, dhtTemp, id);
    my_conn.cmd_query(query);
    sprintf(query, UPDATE, dhtHum, id2);
    my_conn.cmd_query(query);
    Serial.println();
    Serial.println("UPDATED!!");
  }
}
