/******************** Function for sending packet ********************/
boolean sendPacket(){
    txbuff[0]  = devID;
    txbuff[1]  = commandWl;
    txbuff[2]  = dhtTemp;
    txbuff[3]  = dhtHum;
    txbuff[4]  = bmpPreshPa;
    txbuff[5]  = bmpTemp;
    txbuff[6]  = 0; //GPSAlt;
    txbuff[7]  = bmpSeaLev;
    Mirf.send((byte *)&txbuff);
    while(Mirf.isSending()){
    }
    if (Mirf.isSending()) return true; 
    else return false;
}

/****************** Filling up server data *******************/
void getPacket(){
  if (Mirf.dataReady() == true){
    Mirf.getData((byte *) &rxbuff); 
  }
}

/********************** Response ***********************/
void sendResponse(){
  getPacket();
  sendPacket();
  dataRec = true;
}

/******************* Processing data *******************/
void processPacket(){
  receiveID      = rxbuff[0];
  receiveCommand = rxbuff[1]; 
}


