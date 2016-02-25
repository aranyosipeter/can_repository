
// This headers for SPI connection to the wireless connction with NRF24L01
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <Ethernet.h>
#include <sha1.h>
//#include <avr/pgmspace.h>
#include <stdlib.h>
#include <mysql.h>

#define IRQ 2

/************************* Timers ******************/
long               time                  = 0;
long               sendTime              = 0;
long               prevTime              = 0;
unsigned long      timer                 = 0;

/*********************** Codes ********************/

int                 i, r                 = 0;         // for ciklus valtozoja

byte                mac_addr[]           = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char                server_name[]        = "www.ynfho.com";
char                user[]               = "ynfhocom_s";
char                password[]           = "watermelon";
char                node1[]              = "node1";
char                devou[]              = "devou";
char                devin[]              = "devin";

byte                identifier           = B00000000;
byte                slaveId              = B00000000;
byte                command              = B00000000;
int                 dhtTemp              = 0;
int                 dhtHum               = 0;
int                 val                  = 0;
long                bmpPresKPa           = 0;
boolean             buttonValue          = false;
boolean             dataRec              = false;
boolean             connectionOK         = false;
float               bmpTemp              = 0;
byte                receive_Command      = B00000000;

Connector my_conn;        // The Connector/Arduino reference

//const char TEST_SELECT_QUERY[] = "SELECT * FROM world.city LIMIT 10";
//const char QUERY_POP[] = "SELECT population FROM world.city WHERE name = 'New York'";
//const char INSERT_TEXT[] = "INSERT INTO test_arduino.hello VALUES ('Hello, MySQL!', NULL)";
const char INSERT_DATA[] = "INSERT INTO arduino.sensor_data VALUES (%d, %s, %d)";
const char UPDATE[] = "UPDATE ynfhocom_s.sensors SET value=%d WHERE id=%d";  
//const char HELLO_SQL[] = "SELECT * from test_arduino.hello";
//const char HELLO_DATA[] = "SELECT * from test_arduino.temps";

/******************* Buffers for the transreceiver *****************/

int16_t txbuff[9];                        // kuldesre varo adatok tombje
int16_t rxbuff[9];                        // bejovo adatok tombje

/************************** Setup *******************************/
void setup() {
  pinMode(IRQ, INPUT);
  Serial.begin(9600);
  mirfInit();
  //commInit();
  Serial.println("Controller started!");
  attachInterrupt(0, voidGetPacket, FALLING);
}

/************************** Loop *******************************/
void loop() {
  if (( millis() % 500 ) == 0 ){
    voidPacketSend();
       
  }
  if (( millis() % 2500 ) == 0 ){  
    serialMonitor();
  }
  if (( millis() % 2500 ) == 0 && connectionOK == true ){  
    // Datalog to SQL database
    dataLogSQL();
  }
}

/******************* Initalizing MiRF ************************/
void mirfInit(){
  Mirf.payload = 32;
  Mirf.channel = 10;
  Mirf.csnPin = 7;
  Mirf.cePin = 8;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"servr");
  Mirf.setTADDR((byte *)"devou");
  Mirf.config();
}

/*********************** Communication Init ************************/
void commInit(){
  Serial.println("Connecting...");
  if (my_conn.mysql_connect(server_name, 3306, user, password)) {
    delay(1000);
    connectionOK = true;
  }
  else {
    Serial.println("Connection failed.");
    connectionOK = false;
  }
}

/******************** Function for sending packet ********************/
void voidPacketSend(){
    // Creating packet
    txbuff[0] = identifier;
    txbuff[1] = command;
    txbuff[2] = 0;
    txbuff[3] = 0;
    txbuff[4] = 0;
    txbuff[5] = 0;
    txbuff[6] = 0;
    txbuff[7] = 0;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    //Serial.println("Packet sent...");
}
 
/********************* Filling up server data *************************/
void voidGetPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);  
  }
  dataFill();
}

/************************* Data filling up ***************************/
void dataFill(){
  slaveId = rxbuff[0];
  for ( i = 0; i < 9; i++ ){
    Serial.println(rxbuff[i]);
  }
  if ( slaveId == 1){
    dhtHum = rxbuff[1];
    dhtTemp = rxbuff[2];
    val = rxbuff[3];    
    bmpPresKPa = rxbuff[4];
  }
  if ( slaveId == B00000010 ){
    buttonValue = rxbuff[1];
  } 
}
/********************** Data log to SQL database *********************/
void dataLogSQL(){
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

/************************ Serial monitor *******************************/
void serialMonitor(){
  Serial.print("Humidity (%): ");
  Serial.println(dhtHum, DEC);
  Serial.print("Temperature (oC): ");
  Serial.println(dhtTemp, DEC);
  Serial.print("Pressure = ");
  Serial.print(bmpPresKPa, DEC);
  Serial.println(" KPa");
}

/************************ Establish connection *************************/
void connEstab(){
  voidPacketSend(); 
  timer = millis();
  while( millis() < ( timer + 100 )){
  } 
}
