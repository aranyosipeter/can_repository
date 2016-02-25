// nRF24 module methods

/*
2015.07.24 [General]: Created/add header file => copy existed methods
2015.07.27 [Modification]: Delete all mods in connection with GPS module
2015.08.12 [Modification]: Use bytes in communacion
*/

/************************************** Processing data ***************************************/
void processData(byte presRo, byte presFrac, byte tempRo, byte tempFrac, byte seaLevRo, byte seaLevFrac, float *pres, float *temp, float *seaLev){
  *pres = (presRo * 10) + (float(presFrac) / 10);
  *temp = tempRo + (float(tempFrac) / 10);
  *seaLev = (seaLevRo * 10) + (float(seaLevFrac) / 10);  
}

/******************************* Function for sending packet ***********************************/
boolean sendPacket(/*byte mode*/){
    byte txbuff[15];
    //if (mode == 0) {
      txbuff[0] = devID;
      txbuff[1] = command;
      txbuff[2] = (updateTime / 1000);
      txbuff[3] = 0;
      //txbuff[4] = 0;
      //txbuff[5] = 0;
      //txbuff[6] = 0;
      //txbuff[7] = 0;
    //}
    /*else if (mode == 1) {
      txbuff[0] = devID;
      txbuff[1] = command;
      txbuff[2] = 0;
      txbuff[3] = 0;
      txbuff[4] = 0;
      txbuff[5] = 0;
      txbuff[6] = 0;
      txbuff[7] = 0;
    }*/
     Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    if (!Mirf.isSending()) return true; 
    else return false;
}

/*********************************** Filling up server data **********************************/
void getPacket(){
  byte rxbuff[15];
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);
  }
  dataRec = true;
  receiveID      = rxbuff[0];
  receiveCommand = rxbuff[1];
  dhtTemp        = rxbuff[2];
  dhtHum         = rxbuff[3];
  
  processData(rxbuff[4], rxbuff[5], rxbuff[6], rxbuff[7], rxbuff[8], rxbuff[9], &bmpPreshPa, &bmpTemp, &bmpSeaLev);
  blinkLED(16, 3, false);
}

/****************************************** Response *******************************************/
void sendResponse(){
  getPacket();
  sendPacket();
}


