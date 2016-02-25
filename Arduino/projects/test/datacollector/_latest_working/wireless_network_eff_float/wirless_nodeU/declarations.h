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

#define DataScreenChange 50
#define IRQ 2
#define TX 3
#define RX 4
#define Baudrate 9600

/******************* Device flags ***********************/
boolean            systemInit        = true;
boolean            sensorInit        = false;
boolean            displayInit       = false;
boolean            realTimeClockInit = false;
boolean            wirelessModInit   = false;
//boolean            gpsInit           = false; 

/***********************************************************************/
/*                          Device settings                            */
/***********************************************************************/
boolean            debugFlag         = false;

/******************** Timer values *****************/

unsigned long      alarmTimer              = 0;
unsigned long      tmpTimer                = 0;
unsigned long      time                    = 0;
unsigned long      timeOut                 = 0;
unsigned long      clearTime               = 0;
unsigned long      clearPrevTime           = 0;

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

float               dhtTemp              = 0;
float               dhtHum               = 0;
int                 val                  = 0;
float               light                = 0;
long                bmpPres              = 0;
float               bmpPreshPa           = 0;

float               dhtFah               = 0;
float               bmpTemp              = 0;
float               bmpSeaLev            = 0;
float               bmpRealAlt           = 0;
//double              GPSAlt               = 0;
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
float               txbuff[7];                        // kuldesre varo adatok tombje
float               rxbuff[7];                        // bejovo adatok tombje



