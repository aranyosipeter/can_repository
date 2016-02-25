#include <Ethernet.h>
#include <SPI.h>
#include <sha1.h>
//#include <avr/pgmspace.h>
#include <stdlib.h>
#include <mysql.h>
#include <EthernetClient.h>

Connector my_conn;        // The Connector/Arduino reference

/********************** Timings *************************/
short               tries                = 0;
unsigned long       timerValue           = 0;
unsigned long       timer                = 0;

/*********************** Codes **************************/
boolean             connectionFail       = false;
boolean             dataRec              = false;
byte                identifier           = B00000010;
byte                devID                = B00000000;

byte                command              = B00000000;
boolean             buttonValue          = false;
char                results[20];

/******************* Received datas *********************/
int                 dhtTemp              = 0;
int                 dhtHum               = 0;
int                 val                  = 0;
int                 bmpTemp              = 0;
int                 bmpPresKPa           = 0;

/******************** MySQL server *********************/
byte                mac_addr[]            = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress           server_addr(81,0,104,144);
char                server_name[]         = "www.ynfho.com";
char                user[]                = "ynfhocom_s";
char                password[]            = "watermelon";


//const char TEST_SELECT_QUERY[] = "SELECT * FROM world.city LIMIT 10";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
//const char INSERT_DATA[] = "INSERT INTO arduino.sensor_data VALUES (%d, %s, %d)";
const char UPDATE[] = "UPDATE ynfhocom_s.sensors SET value=%d WHERE id=%d";  
const char SELECT_RELAY[] = "SELECT * FROM ynfhocom_s.relays;";
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

/************************** Setup *******************************/
void setup(){
  Serial.begin(9600);
  Ethernet.begin(mac_addr);
  commInit();
  Serial.println("Controller started!"); 
}

/************************** Loop *******************************/
void loop(){
  // Flag of connection fail is set!
  if (/*( my_conn.is_available() == 0 ) ||*/ ( my_conn.is_connected() == 0 )){
    Serial.println("Problem with connection!");
    connectionFail = true;
  }
  // Method of reconnecting
  if ( connectionFail == true ){
    Serial.println("Try to connect MySQL server!");
    my_conn.disconnect();
    Ethernet.begin(mac_addr);
    if (my_conn.mysql_connect(server_name, 3306, user, password)){
      connectionFail = false;
      Serial.println("Connection established!");
    }
  } 
  
  if ( timer == 0 && connectionFail == false){
           dataLogSQL();
           Serial.println("datalog: Done!");
           timer++;    
  } else
  if ( timer == 0 && connectionFail == true){
            Serial.println("There is no connection! Skip the queries! ");
            timer++;
  }
  if ( timer == 1 ){
    timer = 0; 
  }  
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

/*********************************** Data log to SQL database *********************************/
void dataLogSQL(){
    short id = 1;
    short id2 = 2;
    short id3 = 3;
    short id4 = 4;
    short id5 = 5;
    short id6 = 6;
    char query[64];
    sprintf(query, UPDATE, dhtTemp, id);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, dhtHum, id2);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, bmpTemp, id3);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, bmpPresKPa, id4);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, buttonValue, id5);
    my_conn.cmd_query(query);
    delay(250);
    sprintf(query, UPDATE, val, id6);
    my_conn.cmd_query(query);
    
    sprintf(query, SELECT_RELAY);
    my_conn.cmd_query(query);
    my_conn.show_results(results);
    Serial.println(results);
    Serial.println();
    Serial.println("UPDATED!!");
}
