// Initialize device connections/settings

/*
2015.07.27 [Modification]: Delete all code in connection with GPS module
2015.09.16. [Performance] Use F() function to store strings in progmem
*/

// LCD display connection
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

// RTC settings
RTC_DS1307 RTC;

// BMP085
Adafruit_BMP085 bmp;

//DHT11
dht11 DHT11;

/******************* Initalizing BMP085 **********************/
boolean bmpInit(){
    if (!bmp.begin()) return false;
    else return true;
}

/********************** DHT11 Check **************************/
boolean dhtInit(){
  boolean returnValue = true;
  int chk = DHT11.read(DHT11PIN);
  
  if (serialDataFlag) Serial.print(F("Read sensor: "));
  switch (chk){
    case 0:
      if (serialDataFlag) Serial.println(F("OK"));
      break;
    case -1: 
      if (serialDataFlag) Serial.println(F("Checksum error")); 
      returnValue = false;
      break;
    case -2: 
      if (serialDataFlag) Serial.println(F("Time out error")); 
      returnValue = false;
      break;
    default: 
      if (serialDataFlag) Serial.println(F("Unknown error"));
      returnValue = false; 
      break;
    }
  return returnValue;
}

/****************** Initalizing LCD screen ********************/
boolean lcdInit(){
  lcd.begin (16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(LED_ON);
  return true;
}

/******************** Initalizing RTC ************************/
boolean rtcInit(){
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  return true;
}

/******************* Initalizing MiRF ************************/
boolean mirfInit(){
  Mirf.payload = 16;
  Mirf.channel = 80;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"nodeU");
  Mirf.setTADDR((byte *)"servU");
  Mirf.config();
  return true;
}




