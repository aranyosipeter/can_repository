// nRF24 module methods

/*
2015.07.24 [General]: Created/add header file => copy existed methods
2015.07.27 [Modification]: Delete all mods in connection with GPS module
2015.08.13 [Modification]: Create convertMeasData method
*/

/************************ Converting data ****************************/
void convertMeasData(float pres, float temp, float seaLev, byte *packet){
  packet[4] = pres / 10;
  packet[5] = ((pres - (packet[4] * 10)) * 10);
  packet[6] = temp / 1;
  packet[7] = ((temp - packet[6]) * 10);
  packet[8] = seaLev / 10;
  packet[9] = ((seaLev - (packet[8] * 10)) * 10);  
}


/******************** Function for sending packet ********************/
boolean sendPacket(){
    txbuff[0]  = devID;
    txbuff[1]  = commandWl;
    txbuff[2]  = dhtTemp;
    txbuff[3]  = dhtHum;

    convertMeasData(bmpPreshPa, bmpTemp, bmpSeaLev, txbuff); 
    
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    if (!Mirf.isSending()) return true; 
    else return false;
}

/****************** Filling up server data *******************/
void getPacket(){
  /*if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff);
  
    receiveID      = rxbuff[0];
    receiveCommand = rxbuff[1];
    updateTime     = (rxbuff[2] * 1000);
  }*/
}

/********************* Set IRQ flag ***********************/
void setIRQFlag(){
  dataRec = true;
}

/********************** Response ***********************/
void sendResponse(){
  getPacket();
  sendPacket();
  dataRec = true;
}




