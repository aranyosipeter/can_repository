//Serial dataprint methods

/*
2015.07.28 [General]: Created/add header file => copy existed methods
2015.09.16. [Performance] Use F() function to store strings in progmem
 */
 
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
  if ( (command[0] == dataChar[0]) && (command[1] == dataChar[1]) && (command[2] == dataChar[2]) && (command[3] == dataChar[3])) {
    dataFlag = true;
    Serial.println(F("data parancs fogadva")); 
  }
  else {
    dataFlag = false;
    Serial.println(F("nem data parancs")); 
  } 
  if (( dataFlag == true ) && ( serialDataFlag == false )) serialDataFlag = true;
  if (( dataFlag == false ) && ( serialDataFlag == true )) serialDataFlag = false; 
  if ( command[0] == relayChar[0] && command[1] == relayChar[1] && command[2] == relayChar[2] && command[3] == relayChar[3] && command[4] == relayChar[4] ) {
    relayFlag = true;
    Serial.println(F("relay parancs fogadva"));
  }
  else {
    relayFlag = false;
    Serial.println(F("nem relay parancs"));
  }
  if ( relayFlag == true ){
    relayComPin = atoi(&command[6]) - 1;
    Serial.print(F("Vezerelt rele szama: "));
    Serial.println(relayComPin, DEC);
    if ( command[10] == trueChar[0]  && command[11] == trueChar[1] && command[12] == trueChar[2] && command[13] == trueChar[3] ){
      Serial.print(F("Allapota: "));
      Serial.println(F("Bekapcsolva")); 
      bitSet(relayBuff, relayComPin);
      Serial.println(relayBuff, BIN);
    }
    if ( command[10] == falseChar[0]  && command[11] == falseChar[1] && command[12] == falseChar[2] && command[13] == falseChar[3] && command[14] == falseChar[4] ){
      Serial.print(F("Allapota: ")); 
      Serial.println(F("Kikapcsolva")); 
      bitClear(relayBuff, relayComPin);
      Serial.println(relayBuff, BIN);
    }
  }
}

/******************************************- Serial data ************************************/
void printSerialData(){
  if ( serialDataFlag){
    Serial.print(F("Paratartalom: "));
    Serial.print(dhtHum, DEC);
    Serial.println(F(" %"));
    Serial.print(F("Homerseklet: "));
    Serial.print(dhtTemp, DEC);
    Serial.println(F(" C"));
    Serial.print(F("Legnyomas: "));
    Serial.print(bmpPreshPa, DEC);
    Serial.println(F(" kPa"));  
  }
  
}

/********************* Adjust time and date ************************/
/*void adjTimeAndDate(){
  if (adjFlag) {
    if (
  }
}*/
