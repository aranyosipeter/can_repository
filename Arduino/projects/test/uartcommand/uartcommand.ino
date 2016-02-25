

char rxTemp[50];
char command[40];
char comp;
int i = 0;
int chars = 0;

boolean stringComplete = 0;

void setup(){
  Serial.begin(9600); 
}

void loop(){
  serialCollect();
  if ( stringComplete == 1 ){
    Serial.print("Kapott parancs: ");
    Serial.println(rxTemp);
    for ( i=0; i<=chars; i++){
      rxTemp[i] = '\0';
    }
    chars = 0;
    stringComplete = 0;  
  }

}

void serialCollect(){
  while( Serial.available()){
    comp = Serial.read();
     if ( comp == '\n' ){
      stringComplete = 1;
      return 0;
     }
    rxTemp[chars] = comp;
    chars++;
  }
}



