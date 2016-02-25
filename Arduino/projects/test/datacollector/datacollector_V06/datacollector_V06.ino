#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include <avr/pgmspace.h>
//#include <avr/dtostrf.h>  // Add this for the Due
#include <stdlib.h>
#include <mysql.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(81,0,104,144);
char server_name[] = "www.ynfho.com";
char user[] = "ynfhocom_s";
char password[] = "watermelon";

Connector my_conn;        // The Connector/Arduino reference

//const char TEST_SELECT_QUERY[] = "SELECT * FROM world.city LIMIT 10";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
const char INSERT_DATA[] = "INSERT INTO arduino.sensor_data VALUES (%d, %s, %d)";
const char UPDATE[] = "UPDATE ynfhocom_s.sensors SET value=%d WHERE id=%d"; 
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";



void setup() {
  Ethernet.begin(mac_addr);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  delay(1000);
  Serial.println("Connecting...");
  if (my_conn.mysql_connect(server_name, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}
 
void loop() {
if (( millis() % 1000 ) == 0 ){
    int temperature = 22;
    int id = 1;
    char name[] = "'DHT11'";
    char query[64];
    sprintf(query, UPDATE, temperature, id);
    my_conn.cmd_query(query);
  }
}
