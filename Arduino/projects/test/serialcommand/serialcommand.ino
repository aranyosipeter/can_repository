/******************** Timer values *****************/
unsigned long     alarmTimer     = 0;
unsigned long     tmpTimer       = 0;
unsigned long     time           = 0;
unsigned long     timeOut        = 0;
unsigned long     clearTime      = 0;
unsigned long     clearPrevTime  = 0;

/************************* Flags *******************/
boolean     dataRec              = false;      // nincs hasznalatban
boolean     handShake            = false;      // kommunikacio szinkron flag
boolean     commComp             = false;
boolean     dataFlag             = false;
boolean     relayFlag            = false;

/*********************** Declaration of parameters ********************/
char                commandChar;
char                relayComChar;
char                command[16]            = "";
char                test[16]               = "data";
char                dataChar[5]            = "data";
char                relayChar[6]           = "relay";
volatile char       trueChar[5]            = "TRUE";
volatile char       falseChar[6]           = "FALSE";
volatile char       empty                  =' ';

int         j, k, s              = 0;         // for ciklus valtozoja
int         relayComPin          = 0;

/************************************* Setup *******************************/
void setup()  {
  Serial.begin(9600);
}

/************************************* Loop *******************************/
void loop()   {
  if (commComp) {
    for ( k = 0; k < s; k++ ){
    Serial.print(command[k]);  
    }
    Serial.println("");
    commandProc();
    //for ( k = 0; k < s; k++ ){
    //command[s] = empty;
    commComp = false;
    s = 0;
    //} 
  } 
}

/*************************** SerialEvent *********************/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char commandChar = (char)Serial.read(); 
    // add it to the inputString:
    command[s] = commandChar;
    s++;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (commandChar == '\n') {
      commComp = true;
    } 
  }
}

/************************ Commands *************************/
void commandProc(){
  Serial.println("teszteles jelleggel/ sajnos csak igy mukodik");
  Serial.println(&test[0]);
  Serial.println("dataChar[]");
  Serial.println(&dataChar[0]);
  Serial.println("relayChar[]");
  Serial.println(&relayChar[0]);
  if ( (command[0] == dataChar[0]) && (command[1] == dataChar[1]) && (command[2] == dataChar[2]) && (command[3] == dataChar[3])) {
    dataFlag = true;
    Serial.println("data parancs fogadva"); 
  }
  else {
    dataFlag = false;
    Serial.println(" data parancs nem jo"); 
  } 
  if ( command[0] == relayChar[0] && command[1] == relayChar[1] && command[2] == relayChar[2] && command[3] == relayChar[3] && command[4] == relayChar[4] ) {
    relayFlag = true;
    Serial.println(" relay parancs fogadva");
  }
  else {
    relayFlag = false;
    Serial.println(" relay parancs nem jo");
  }
  if ( relayFlag == true ){
    relayComPin = atoi(&command[6]) + 1;
    Serial.print("Bekapcsolt rele szama: ");
    Serial.println(relayComPin, DEC);
    if ( command[10] == trueChar[0]  && command[11] == trueChar[1] && command[12] == trueChar[2] && command[13] == trueChar[3] ){
      Serial.print("Allapota: ");
      Serial.println("Igaz"); 
      //bitSet(relayBuff, relayComPin);
    }
    if ( command[10] == falseChar[0]  && command[11] == falseChar[1] && command[12] == falseChar[2] && command[13] == falseChar[3] && command[14] == falseChar[4] ){
      Serial.print("Allapota: "); 
      Serial.println("Hamis"); 
      //bitClear(relayBuff, relayComPin);
    }
  }
}
