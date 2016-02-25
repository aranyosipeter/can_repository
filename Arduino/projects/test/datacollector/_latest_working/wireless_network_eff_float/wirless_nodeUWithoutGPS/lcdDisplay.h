//LCD Display methods

/*
2015.07.24 [General]: Created/add header file => copy existed methods
2015.07.27 [Modification]: Delete all code in connection with GPS module
 */


/**************************************************************************/
/*                           Clear the screen                             */
/**************************************************************************/
void clearScreen(){
  lcd.setCursor(0,1);
  lcd.print("                ");
}

/**************************************************************************/
/*                           Show actual time                             */
/**************************************************************************/
void printRealTime(){
  now = RTC.now();

  lcd.setCursor(0,0);
  lcd.print(now.year(), DEC);

  lcd.setCursor(4,0);
  lcd.print("/");

  if ( now.month() < 10 ){
    lcd.setCursor(5,0);
    lcd.print(0, DEC);
    lcd.setCursor(6,0);
    lcd.print(now.month(), DEC);
  }
  else {
    lcd.setCursor(5,0);
    lcd.print(now.month(), DEC);
  }

  lcd.setCursor(7,0);
  lcd.print("/");

  if ( now.day() < 10 ){
    lcd.setCursor(8,0);
    lcd.print(0, DEC);
    lcd.setCursor(9,0);
    lcd.print(now.day(), DEC);
  }
  else {
    lcd.setCursor(8,0);
    lcd.print(now.day(), DEC);
  }

  if ( now.hour() < 10 ){
    lcd.setCursor(11,0);
    lcd.print(0, DEC);
    lcd.setCursor(12,0);
    lcd.print(now.hour(), DEC);
  }
  else {
    lcd.setCursor(11,0);
    lcd.print(now.hour(), DEC);
  }


  if (( now.second() % 2 ) == 0 ){
    lcd.setCursor(13,0);
    lcd.print(":");
  }
  else{
    lcd.setCursor(13,0);
    lcd.print(" ");
  }

  if ( now.minute() < 10 ){
    lcd.setCursor(14,0);
    lcd.print(0, DEC);
    lcd.setCursor(15,0);
    lcd.print(now.minute(), DEC);
  }
  else {
    lcd.setCursor(14,0);
    lcd.print(now.minute(), DEC);
  }
} 

/**************************************************************************/
/*                        Show processed data                             */
/**************************************************************************/
void printLCDData(){
  if ((millis() - time  >= DataScreenChange) && NotInASec){
    cycleVar++;
    clearScreen();
    NotInASec = false;
    time = millis();
  }
  if ((millis() - time  < DataScreenChange) && !NotInASec){
    NotInASec = true;
  }

  switch (cycleVar){
  case 0: 
    lcd.setCursor(3,1);
    lcd.print("H: ");
    lcd.setCursor(6,1);
    lcd.print(bmpTemp,1);
    lcd.setCursor(11,1);
    lcd.print("C");
    break;
  case 1: 
    lcd.setCursor(2,1);
    lcd.print("Ny:");
    lcd.setCursor(6,1);
    lcd.print(bmpPreshPa, 1);
    lcd.setCursor(12,1);
    lcd.print("hPa");  
    break;
  case 2: 
    lcd.setCursor(2,1);
    lcd.print("S: ");
    lcd.setCursor(5,1);
    lcd.print(bmpSeaLev, 1);
    lcd.setCursor(11,1);
    lcd.print(" hPa");
    break;
  case 3: 
    lcd.setCursor(3,1);
    lcd.print("A: ");
    lcd.setCursor(6,1);
    lcd.print(bmpRealAlt, 1);
    lcd.setCursor(11,1);
    lcd.print(" m");
    break;
  case 4: 
    lcd.setCursor(3,1);
    lcd.print("T: ");
    lcd.setCursor(6,1);
    lcd.print(dhtTemp, 1);
    lcd.setCursor(11,1);
    lcd.print(" C");
    break;
  case 5: 
    lcd.setCursor(3,1);
    lcd.print("H: ");
    lcd.setCursor(6,1);
    lcd.print(dhtHum, 1);
    lcd.setCursor(11,1);
    lcd.print(" %");
    break;
  case 6: 
    lcd.setCursor(3,1);
    lcd.print("F: ");
    lcd.setCursor(6,1);
    lcd.print(dhtFah, 1);
    lcd.setCursor(11,1);
    lcd.print(" F");
    break;
  default:
    cycleVar = 0;
  }    
}



