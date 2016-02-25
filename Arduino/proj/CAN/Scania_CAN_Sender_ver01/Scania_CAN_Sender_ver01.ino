// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN0(9);  

byte Data[8];

void setup(){
  Serial.begin(115200);
  if(CAN0.begin(CAN_250KBPS) == CAN_OK) 
    Serial.print("CAN init OK!");
  else 
    Serial.print("CAN init fail!!");
  initCanData(Data);
}


void loop(){
  CAN0.sendMsgBuf(0x18FEBF27, 1, 8, Data); 
  Serial.println("CAN msg sent!");
  delay(100);                      
}

void initCanData(byte* Data){
  Data[0] = B01011010;
  Data[1] = B00000000;
  Data[2] = 0x7c;
  Data[3] = 0x7c;
  Data[4] = 0x7c;
  Data[5] = 0x7c;
  Data[6] = 0x7c;
  Data[7] = 0x7c;
}
