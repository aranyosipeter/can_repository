
// LCD display connection
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

// RTC settings
RTC_DS1307 RTC;

// BMP085
Adafruit_BMP085 bmp;

// TinyGPS++
TinyGPSPlus gps;

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

/******************* Initalizing BMP085 **********************/
boolean bmpInit(){
    if (!bmp.begin()) return false;
    else return true;
}

/****************** Initalizing LCD screen ********************/
boolean lcdInit(){
  lcd.begin (16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
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
  Mirf.payload = 32;
  Mirf.channel = 82;
  Mirf.csnPin = 14;
  Mirf.cePin = 15;
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"nodeU");
  Mirf.setTADDR((byte *)"servU");
  Mirf.config();
  return true;
}

/************************* GPS Init ***************************/
boolean GPSInit(){
  ss.begin(GPSBaud);
  unsigned long timeOut = millis();
  Serial.println(F("Try to communicate with GPS!"));
  while ((timeOut + 5000) > millis() && gps.charsProcessed() < 10){
    if (ss.available() > 0) gps.encode(ss.read());
  }
  if ((timeOut + 5000) > millis() && gps.charsProcessed() >= 10){
    Serial.println(F("GPS connected!"));
    return true;
  }
  else{
    Serial.println(F("Can't connect with GPS!"));
    return false;
  }
}



