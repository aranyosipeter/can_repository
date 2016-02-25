// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>

#define BS_SPLIT_MASK   255
#define BS_PDU_SPEC       8
#define BS_PDU_FORMAT    16
#define BS_PDU_PROP      24

#define BS_PRIO           2
#define BS_PRIO_MASK      B00000111


unsigned long rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

byte          Prio = 0;
boolean       DP   = false;
boolean       Res  = false;
byte          PF   = 0;
byte          PS   = 0;
byte          SA   = 0;
unsigned long PGN  = 0;

MCP_CAN CAN0(9);                               // Set CS to pin 10


void setup(){
  Serial.begin(115200);
  if (CAN0.begin(CAN_250KBPS) == CAN_OK)  
    Serial.println("CAN init OK!");
  else
    Serial.println("CAN init ERROR!");
}

void loop(){
  if (CAN0.readMsgBuf(&len, rxBuf) == CAN_OK){
    rxId = CAN0.getCanId(); 
    
    processCanID(&rxId, &Prio, Res, DP, &PF, &PS, &SA);
                       // Get message ID
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    
    Serial.print("Prio: ");
    Serial.println(Prio);
    Serial.print("Data Page: ");
    Serial.println(DP);
    Serial.print("PDU Format: ");
    Serial.println(PF);
    Serial.print("PDU Specific: ");
    Serial.println(PS);
    Serial.print("Source Addres: ");
    Serial.println(SA, HEX);
    Serial.print("PGN: ");
    Serial.println(SA, HEX);
    
    Serial.print("  Data: ");
    for (int i = 0; i<len; i++){
      if (rxBuf[i] < 0x10){
         Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void processCanID(unsigned long* ID, byte* Prio, boolean Res, boolean DP, byte* PF, byte* PS, byte* SA){
  byte splittedID[3]; 
  // Let's split uint32 ID
  splittedID[0] = (byte)(*ID);
  splittedID[1] = (byte)((*ID >> BS_PDU_SPEC) && BS_SPLIT_MASK);
  splittedID[2] = (byte)((*ID >> BS_PDU_FORMAT) && BS_SPLIT_MASK);
  splittedID[3] = (byte)((*ID >> BS_PDU_PROP) && BS_SPLIT_MASK);
  
  *Prio = ((splittedID[3] >> BS_PRIO) && BS_PRIO_MASK);
  Res = bitRead(splittedID[3], 1);
  DP = bitRead(splittedID[3], 0);
  *PF = splittedID[2];
  *PS = splittedID[1];
  *SA = splittedID[0];
}

