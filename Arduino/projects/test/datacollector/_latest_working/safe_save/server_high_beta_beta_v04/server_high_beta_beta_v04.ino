#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <Ethernet.h>
#include <sha1.h>
//#include <avr/pgmspace.h>
#include <stdlib.h>
#include <mysql.h>
#include <EthernetClient.h>
#define IRQ 2

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
const char SELECT_RELAY[] = "SELECT * FROM ynfhocom_s.relays;";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
//const char INSERT_DATA[] = "INSERT INTO arduino.sensor_data VALUES (%d, %s, %d)";
const char UPDATE[] = "UPDATE ynfhocom_s.sensors SET value=%d WHERE id=%d";  
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

/******************* Buffers for the transreceiver *****************/
int16_t             txbuff[9];                        // kuldesre varo adatok tombje
int16_t             rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup(){
  pinMode(IRQ, INPUT);
  mirfInit();
  Serial.begin(9600);
  Ethernet.begin(mac_addr);
  commInit();
  Serial.println("Controller started!"); 
  attachInterrupt(0, voidGetPacket, FALLING);
}

/************************** Loop *******************************/
void loop(){
  // Flag of connection fail is set!
  if ( my_conn.is_connected() == 0 ){
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
  
  if ( timer == 0 ){
           Serial.println("node1: Communicate!");
           Mirf.channel = 10;
           Mirf.init();
           Mirf.config();
           Mirf.setTADDR((byte *)"node1"); 
           while (( tries < 5 ) && ( dataRec == false )){           
             voidPacketSend();
             timerValue = millis();
             while (( timerValue + 200 ) > millis() ){
             }
             tries++;
             Serial.print("Number of tries: ");
             Serial.println(tries);
           }             
           dataRec = false;
           tries = 0;
           timer++;
           Serial.println("node1: Done!"); 

  } else
  if ( timer == 1 ){
           Serial.println("node2: Communicate!");
           Mirf.channel = 80;
           Mirf.init();
           Mirf.config();
           Mirf.setTADDR((byte *)"devou");
           while (( tries < 5 ) && ( dataRec == false )){           
             voidPacketSend();
             timerValue = millis();
             while (( timerValue + 200 ) > millis() ){
             }
             tries++;
             Serial.print("Number of tries: ");
             Serial.println(tries); 
           }           
           dataRec = false;
           tries = 0;
           timer++;
           Serial.println("node2: Done!");
  } else
  if ( timer == 2 && connectionFail == false){
           dataLogSQL();
           Serial.println("datalog: Done!");
           timer++;    
  } else
  if ( timer == 2 && connectionFail == true){
            Serial.println("There is no connection! Skip the queries! ");
            timer++;
  }
  if ( timer == 3 ){
    /*Serial.print("Humidity: ");
    Serial.println(dhtHum, DEC);
    
    Serial.print("Temperature (oC): ");
    Serial.println(dhtTemp, DEC);
    
    Serial.print("Sensor: ");
    Serial.println(val, DEC);
    
    Serial.print("Temperature: ");
    Serial.println(bmpTemp);
    
    Serial.print("Pressure: ");
    Serial.println(bmpPresKPa, DEC);
    
    Serial.print("Button ");
    Serial.println(buttonValue, DEC);*/
    timer = 0; 
  }  
} 

/************************* Mirf init ************************/
void mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 10;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"serv1");
  Mirf.config();  
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    txbuff[0] = identifier;
    txbuff[1] = command;
    txbuff[2] = 0;
    txbuff[3] = 0;
    txbuff[4] = 0;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    txbuff[8] = 0;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
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

/****************** Filling up server data *******************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);
  }
  dataRec = true;
  devID = rxbuff[0];
  
  if ( devID == 3 ){
    dhtHum = rxbuff[2];
    dhtTemp = rxbuff[3];
    val = rxbuff[4];
    bmpPresKPa = rxbuff[5];
    bmpTemp = rxbuff[6];
  } else
  
  if ( devID == 2 ){
    buttonValue = rxbuff[2];
  }
}

/********************** Response ***********************/
void voidResponse(){
  voidGetPacket();
  voidPacketSend();
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
    /*my_conn.cmd_query(query);
    delay(250);
    sprintf(query, SELECT_RELAY);
    my_conn.cmd_query(query);
    my_conn.show_results(results);
    Serial.println(results);*/
    Serial.println();
    Serial.println("UPDATED!!");
}
