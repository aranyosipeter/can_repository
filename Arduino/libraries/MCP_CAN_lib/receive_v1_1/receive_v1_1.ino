// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

MCP_CAN CAN0(9);                               // Set CS to pin 10


void setup(){
  Serial.begin(115200);
  CAN0.begin(CAN_250KBPS);                       // init can bus : baudrate = 250k 
  Serial.println("MCP2515 Library Receive Example...");
}

void loop(){
  if (CAN0.readMsgBuf(&len, rxBuf) == CAN_OK){              // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();                    // Get message ID
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    Serial.print("  Data: ");
    for(int i = 0; i<len; i++){
      if(rxBuf[i] < 0x10){
         Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
