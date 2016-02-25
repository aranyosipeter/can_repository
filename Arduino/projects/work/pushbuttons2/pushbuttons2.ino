#define defTempr 8;
#define defTempm 120;
#define defHourt 1;
#define defHourm 48;

#include <LiquidCrystal.h>


int pushButtons[]={0, 1, 2, 3, 4, 5};
int outputs[]={14, 15};
LiquidCrystal lcd(12, 11, 9, 8, 7, 6);
int tempRaise=defTempr;
int tempHour=defHourt;
int tempMax=defTempm;
int hourMax=defHourm;
int tempr=0;
int temph;
int temptmax;
int temphmax;
int setupFlag;
int i,j,k;
unsigned long time;


void setup(){
  lcd.begin(16, 4);
  for (i=0;i<5;i++){
    pinMode(pushButtons[i], INPUT);
  }
  for (i=0;i<2;i++){
    pinMode(outputs[i], OUTPUT);
  }
}


void loop(){
  setupFlag=0;
  voidHourRaise();
  voidTempHour();
  voidMaxTemp();
  voidMaxHour();
  delay(250);
  
}
//Óránkénti emelkedés értékének a bekérése
void voidHourRaise(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fok/ora beallit.");
  delay(2000);
  lcd.clear();
  while(digitalRead(pushButtons[5])==LOW){
    lcd.setCursor(0, 0);
    lcd.print("Jelenlegi ertek: ");
    lcd.setCursor(4, 1);    
    lcd.print(tempRaise, DEC);
    lcd.setCursor(6, 1);
    lcd.print(" C/ora");
    delay(100);
         if (digitalRead(pushButtons[3])==HIGH){
            tempRaise++;  
            lcd.clear();    
              }
         if (digitalRead(pushButtons[4])==HIGH && tempRaise>1){
            tempRaise--;
            lcd.clear();    
              }    
  }
  
}

void voidTempHour(){
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Ido beallit.");
  delay(2000);
  lcd.clear();
  while(digitalRead(pushButtons[5])==LOW){
        lcd.setCursor(1, 0);
        lcd.print("Jelenlegi ido: ");
        lcd.setCursor(5, 1);
        lcd.print(tempHour, DEC);
        lcd.setCursor(7, 1);
        lcd.print("ora");
        delay(100);
        if (digitalRead(pushButtons[3])==HIGH){
            tempHour++;
            lcd.clear();
        }
         if (digitalRead(pushButtons[4])==HIGH && tempHour>1){
            tempHour--;
            lcd.clear();
        }      
  }

}

void voidMaxTemp(){
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Max. beallit.");
  delay(2000);
  lcd.clear();
  while(digitalRead(pushButtons[5])==LOW){
    lcd.setCursor(1, 0);
    lcd.print("Jelenlegi max: ");  
    lcd.setCursor(5, 1);  
    lcd.print(tempMax, DEC);
    lcd.setCursor(8, 1);
    lcd.print("°C");
        delay(100);
        if (digitalRead(pushButtons[3])==HIGH){
            tempMax++;
            lcd.clear();
        }
         if (digitalRead(pushButtons[4])==HIGH && tempMax>1){
            tempMax--;
            lcd.clear();
        }
  }

}

void voidMaxHour(){
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Max. idotart.");
  delay(2000);
  lcd.clear();
  while(digitalRead(pushButtons[5])==LOW){
    lcd.setCursor(1, 0);
    lcd.print("Jelenlegi max: ");  
    lcd.setCursor(4, 1);  
    lcd.print(hourMax, DEC);
    lcd.setCursor(6, 1);
    lcd.print("ora");
        delay(100);
        if (digitalRead(pushButtons[3])==HIGH){
            hourMax++;
            lcd.clear();
        }
         if (digitalRead(pushButtons[4])==HIGH && hourMax>1){
            hourMax--;
            lcd.clear();
        }
  }
}
void voidTimeChange(){
    
}
void voidOnProgress(){
  if(setupFlag==0){
      voidHourRaise();
      voidTempHour();
      voidMaxTemp();
      voidMaxHour();
      time=millis();
      setupFlag=1;
  }
  if(setupFlag==1){
      
  }
}
