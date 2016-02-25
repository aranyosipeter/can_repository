// Declare all constans/variables

/*
2015.07.27 [Modification]: Delete all variables/constans in connection with GPS module
2015.08.13 [Mod]: Timers for getting packets
*/

// Defining LCD pins
#define keyboard 0x21
#define I2C_ADDR    0x27  
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define  LED_OFF  0
#define  LED_ON  1

#define DataScreenChange 8000
#define DataSendingTimer 10000
#define IRQ 2
#define DHT11PIN 3

/***********************************************************************/
/*                            Device flags                             */
/***********************************************************************/
boolean            systemInit        = true;
boolean            sensorInit        = false;
boolean            displayInit       = false;
boolean            realTimeClockInit = false;
boolean            wirelessModInit   = false;

/***********************************************************************/
/*                          Device settings                            */
/***********************************************************************/
boolean            debugFlag         = true;

/******************** Timer values *****************/

unsigned long      alarmTimer              = 0;
unsigned long      tmpTimer                = 0;
unsigned long      time                    = 0;
unsigned long      timeOut                 = 0;
unsigned long      clearTime               = 0;
unsigned long      clearPrevTime           = 0;
unsigned long      updateTimer             = 0;
long               updateTime              = 15000;

/************************* Flags *******************/

boolean             dataRec                = false;      // nincs hasznalatban
boolean             handShake              = false;      // kommunikacio szinkron flag
boolean             commComp               = false;
boolean             dataFlag               = false;
boolean             relayFlag              = false;
boolean             sendFlag               = false;
boolean             serialDataFlag         = false;
boolean             NotInASec              = true;
  
/*********************** Declaration of parameters ********************/

char                commandChar;
char                relayComChar;
char                command[16]          = "";
char                test[16]             = "data";
char                dataChar[5]          = "data";
char                relayChar[6]         = "relay";
volatile char       trueChar[5]          = "TRUE";
volatile char       falseChar[6]         = "FALSE";
volatile char       empty                = ' ';

int                 j, k, s              = 0;         // for ciklus valtozoja
int                 relayComPin          = 0;
byte                cycleVar             = 0;

// DHT11 variables
int                 dhtTemp              = 0;
int                 dhtHum               = 0;
float               dhtFah               = 0;

//BMP085 variables
long                bmpPres              = 0;
float               bmpPreshPa           = 0;
float               bmpTemp              = 0;
float               bmpSeaLev            = 0;
float               bmpRealAlt           = 0;

byte                relayBuff            = B00000000; // kimeno rele vezerlo adatbajt

DateTime            now;
int                 hours                = 0;
int                 minutes              = 0;
int                 seconds              = 0;
unsigned long       mills                = 0;
unsigned long       subtract             = 0;
/******************** Communication of nodes ***********************/

byte                devID                  = B00000101;
byte                commandWl              = B00000000;
byte                receiveCommand         = B00000000;
byte                receiveID              = B00000000;

/******************* Buffers for the transreceiver *****************/
byte                txbuff[15];                        // kuldesre varo adatok tombje
byte                rxbuff[15];                        // bejovo adatok tombje



