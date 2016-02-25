#define LED                       6
#define IRQ                       2
#define PIEZZO                    3

#define ALARMTIME                 60000
#define ARMINGTIME                30000

// I/O init
void initIO(){
  pinMode(IRQ, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(PIEZZO, OUTPUT);
}

boolean saveGyroPosition(float x, float y){
  calculateGyroscopeData(x, y);
  armedGyroStateX = x;
  armedGyroStateY = y;
  return true;
}

boolean checkGyroPosition(){
}


void drivePiezzo(){
  mills = millis()
  //if (alarmState && )
}

boolean setTimingToArmSystem(){
  if (armState){
    if (!armTimeSaved) {
      armStartedTime = glbTimer; // nagyonkerdeses...
      armTimeSaved = true;
    }
    if ((armStartedTime + ARMINGTIME) < glbTimer){
      if (saveGyroPosition(axisX, axisY)){
        systemArmed = true;
        armState = false;
        return true;
      }
      else return false;
    }
    else return false;
  }
}

void resetAlarmFlags(){
  if (!systemArmed) {
    alarmState = false;
    sirenState = false;
    armTimeSaved = false;
  }
}

void disArmSystem() {
  if (!armState) resetAlarmFlags();
}
