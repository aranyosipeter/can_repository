CallGSM call;
SMSGSM  sms;

#define ARMNUMBER "+36305503879"

boolean initGSM(){
  if (gsm.begin(9600)){
    Serial.println("\nstatus=READY");
    return true;
  }
  else {
    Serial.println("\nstatus=IDLE"); 
    return false;
  }
}

boolean notifyOwnerAboutAlarm(){
  
}

boolean setSystemStateByIncomingGSMCalls(char number[], boolean armStateFlag){
  byte callState; 
  callState = call.CallStatusWithAuth(number, 2, 2);
  if (callState == CALL_INCOM_VOICE_AUTH) {
    Serial.println("Incoming call to Arm or Disarm!");
    armStateFlag =!armStateFlag;
    return true;
  }
  else return false;
}
