
int tempCounter = 0;
char rxTempPC[50];
char rxTempSie[50];
//char txTemp[50];
char command[50];
char comp;
char comp2;
int i = 0;
int chars = 0;
int count = 0;
int cr = 0x0D;

long prevTime = 0;

boolean stringPC = false;
boolean stringSie = false;
boolean SieVsPC = false;
boolean timerFlag = false;

void setup(){
  Serial.begin(9600); 
}

void loop(){
  if ( SieVsPC == false ){
    serialPCTx();
    delay(1000);
    serialSiemensTx();
    delay(500);
  }
  if ( SieVsPC == true ){
    serialSiemensRx();
  }
}

void serialCollect(){
  while( Serial.available()){
    comp = Serial.read();
    rxTempPC[chars] = comp;
    chars++;
    if ( comp == '\n' ){
      stringPC = true;
    }
  }
}

void serialCollectSiemens(){
  long time = millis();
  if ( timerFlag == false ){
  prevTime = time;
  timerFlag = true;
  }
 if ( time-prevTime < 6000 ){
   if ( Serial.available() > 0 ){ 
      comp2 = Serial.read();
      rxTempSie[count] = comp2;
      count++;
      if ( ( comp2 == cr ) || ( comp2 == cr ) || ( comp2 == cr ) || ( count > 19 ) ){
        stringSie = true;
      }
    }
  }
  if ( time-prevTime > 6000 ){
    Serial.println("Nincs ertekelheto adat vagy lejart az ido");
    SieVsPC = false;
    timerFlag = false;
  }
}

void serialPCTx(){
  serialCollect();
  dataRxToTx();
  if ( stringPC == true ){
    Serial.print("Kapott parancs: ");
    Serial.println(rxTempPC);
    for ( i=0; i<=chars; i++){
      rxTempPC[i] = '\0';
    }
    chars = 0;
    stringPC = false;  
  }
}

void dataRxToTx(){
  for ( i=0; i<=chars; i++ ){  
    command[i] = rxTempPC[i];
  }
  tempCounter = chars;
}


void serialSiemensTx(){
  int length = 0;
  if ( tempCounter == 0 ){
    Serial.println("Nincs mit kuldeni");  
    return;
  }
  for ( i=0; i<=tempCounter; i++ ){
    if ( (command[i] == '\n') || (command[i] == '\0') ){
      length = i ;
    }
    else{
      length = tempCounter;
    }
  }
  char txTemp[length+2];
  for ( i=0; i<=length; i++ ){
    txTemp[i] = command[i];
    command[i] = '\0';
    txTemp[i+1] = cr;
  }
  Serial.print(txTemp);
  SieVsPC = true;
}

void serialSiemensRx(){ 
  serialCollectSiemens(); 
  if ( stringSie == true ){
    Serial.print("Kapott valasz: ");
    Serial.println(rxTempSie);
    for ( i=0; i<=count; i++){
      rxTempSie[i] = '\0';
    }
    count = 0;
    stringSie = false;  
  }
}


