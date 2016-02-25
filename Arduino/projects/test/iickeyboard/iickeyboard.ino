#include <Wire.h>
#define keyboard 0x21

char c;
const int numRows = 4;       // number of rows in the keypad
const int numCols = 4;       // number of columns
const int debounceTime = 10; // number of milliseconds for switch to be stable
byte columns[5] = {
  00001000, 00000100,00000010,00000001};
const char keymap[numRows][numCols] = {
  { 
    '1', '2', '3', 'A'             }
  ,
  { 
    '4', '5', '6', 'B'             }
  ,
  { 
    '7', '8', '9', 'C'             }
  ,
  { 
    '*', '0', '#', 'D'             }
};

const int rowPins[numRows] = { 
  7, 6, 5, 4 }; 
const int colPins[numCols] = { 
  3, 2, 1, 0 };    

byte kInit = B00000000;
byte sendByte = B11111111;
byte recByte = B00000000;

void setup(){
  Wire.begin();
  Serial.begin(9600);
  //keyInit();
}

void loop(){
  c = getKey();
  if (c =! 0){
    Serial.print("Karakter: ");
    Serial.println(c);
  }
}

/****************** Initalizing keyboard *******************/

void keyInit(){
  /*for (int row = 4; row < 8; row++){
    bitSet(kInit, row);
    Serial.print("Bit beallitas: "); 
    Serial.println(kInit,BIN);
  }
  for (int column = 3; column >= 0; column--){
    //bitSet(kInit, column);
  }*/
  byte initK = B11111111;
  Wire.beginTransmission(keyboard);
  Wire.write(initK);
  Wire.endTransmission();
  /*for (int i=0; i<8; i++){
    bitClear(kInit, i);
  }*/
}

/********** Getting keys from the keyboard ****************/

char getKey(){
  char key = 0;                                  
  for(int column = 3; column >= 0; column--){
    bitClear(sendByte, column);
    Wire.beginTransmission(keyboard);
    Wire.write(sendByte);
    Wire.endTransmission();
    Serial.print("Bit beallitas: "); 
    Serial.println(sendByte);
    Serial.println(sendByte, DEC);

    for(int row = 7; row >= 4; row--){     
      Wire.requestFrom(keyboard, 1);
      if(Wire.available())
      {
        recByte = Wire.read();
        Serial.print("Kapott bit: ");
        Serial.println(recByte, BIN);
        Serial.println(recByte, DEC);
        if(bitRead(recByte, row) == 0){
          key = keymap[3-(row-4)][3-column];
          Serial.print("Kapott billentyu: ");
          Serial.println(key);
        }
      }

    }
    bitSet(sendByte, column);
    Wire.beginTransmission(keyboard);
    Wire.write(sendByte);
    Wire.endTransmission();
    Serial.print("Bit kinyiras: ");
    Serial.println(sendByte,BIN);  
 delay(1000);   
  }
  return key;  
}



