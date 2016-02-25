/******************** GPS Altitude ***********************/
void getGPSDateTime(){
  while (ss.available()) gps.encode(ss.read());
  if (gps.date.isValid()){
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", gps.date.month(), gps.date.day(), gps.date.year());
    if (debugFlag) Serial.print(sz);
  }
  
  if (gps.time.isValid()){
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.print(sz);
  }
  
  //if (gps.altitude.isValid()) GPSAlt = gps.altitude.meters(); else GPSAlt = -1;
}




