#include <Wire.h>
#define keyboard 0x21

char c;
const int numRows = 4;       // number of rows in the keypad
const int numCols = 4;       // number of columns
const int debounceTime = 10; // number of milliseconds for switch to be stable
int columns[8] = {
  1, 2, 4, 8, 240, 240, 240, 240};
const char keymap[numRows][numCols] = {
  { 
    '1', '2', '3', 'A'                 }
  ,
  { 
    '4', '5', '6', 'B'                 }
  ,
  { 
    '7', '8', '9', 'C'                 }
  ,
  { 
    '*', '0', '#', 'D'                 }
};

const int rowPins[numRows] = { 
  7, 6, 5, 4 }; 
const int colPins[numCols] = { 
  3, 2, 1, 0 };    

byte kInit = B00000000;
byte sendByte = B00000000;
byte recByte = B00000000;
char key;

void setup(){
  Wire.begin();
  Serial.begin(9600);
  //keyInit();
}

void loop(){
  for(int column = 3; column >= 0; column++){
    //bitSet(sendByte, column);
    byte keys = B11110000;
    Wire.beginTransmission(keyboard);
    Wire.write(/*columns[column]*/ keys);
    Wire.endTransmission();
    Serial.print("Beállított: "); 
    Serial.println(/*columns[column]*/ keys, BIN);
    Serial.println(/*columns[column]*/ keys, DEC);

    Wire.requestFrom(keyboard, 1);
    if(Wire.available())
    {
      recByte = Wire.read();
      Serial.print("Kapott bit: ");
      Serial.println(recByte,BIN);
      Serial.println(recByte, DEC);
      delay(3000);
    }
  }
}
  void keyInit(){
    byte initByte = B1111000;
    Wire.beginTransmission(keyboard);
    Wire.write(kInit);
    Wire.endTransmission();
    for (int i=0; i<8; i++){
      bitClear(kInit, i);
    }
 }





