
int i = 2;
boolean o = false;

void setup(){
  for (i = 2; i < 10; i++){
    pinMode(i, OUTPUT);
  } 
  for (i = 2; i < 10; i++){
    digitalWrite(i, LOW);
  }
  delay(2000);
  for (i = 2; i < 10; i++){
    digitalWrite(i, HIGH);
  }
  i = 2;
  Serial.begin(9600);
}

void loop(){  
  if (!o) {
    for(i = 2; i < 11; i++){
      digitalWrite(i, LOW);
      digitalWrite(i - 1, HIGH);
      delay(100);
      if (i == 9) o = true;
      Serial.println(i);
    }
  }
  delay(100);
  if (o) {
    for(i = 9; i > 0; i--){
      digitalWrite(i, LOW);
      digitalWrite(i + 1, HIGH);
      delay(100);
      if (i == 2) o = false;
      Serial.println(i);
    }
  } 
  delay(500);
}
