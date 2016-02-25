#include <avr/pgmspace.h> // allows use of PROGMEM to store patterns in flash

#define CUBESIZE 4
#define PLANESIZE CUBESIZE*CUBESIZE
#define PLANETIME 3333 
#define TIMECONST 5 


int LEDPin[] = {
  17, 10, 2, 3, 16, 11, 9, 4, 15, 12, 8, 5, 14, 13, 7, 6};
int LEDPinCount = 16;
int PlanePin[] = {
  19, 18 ,1 , 0};
int PlanePinCount  = 4;
int flag=0;
int l=0;
int button = 0;
int options = 0;


void setup()
{
  randomSeed(analogRead(6));
  int pin; 
  for (pin=0; pin<PLANESIZE; pin++){
    pinMode( LEDPin[pin], OUTPUT );
  }
  for (pin=0; pin<CUBESIZE; pin++){
    pinMode( PlanePin[pin], OUTPUT );
  }
}

void loop(){
  
  button = analogRead(7);
  if ( button > 1000 ){
    options++;
  }
  if ( options == 0 ){
    cubeOff();
    loopLEDTest();
  }
  else if ( options == 1 ){
    cubeOff();
    loopHeart();
  }
  else if ( options == 2 ){
    cubeOff();
    loopCube();
  }
  else if ( options == 3 ){
    cubeOff();
    loopWall();
  }
  else if ( options == 4 ){
    cubeOff();
    snake();
  }
  else if ( options == 5 ){
    cubeOff();
    loopColum();
  }
  else if ( options == 6 ){
    cubeOff();
    loopLEDTest();
    loopColum();
    loopHeart();
    loopWall();
    loopCube(); 
    snake();
  }
  else{
    options = 0; 
  }
}

/********************** LEDTest ***********************/

void loopLEDTest()
{
  planesOff();
  int i=0;
  int j=0;
  
  for( j=0; j<4; j++){
    for( i=0;i<16;i++){
      digitalWrite(PlanePin[j], LOW);
      digitalWrite(LEDPin[i], HIGH);
      delay(100);
      digitalWrite(LEDPin[i], LOW);
    }
    digitalWrite(PlanePin[j], HIGH);
  }
  cubeOff(); 
}

/*********************** Heart ***********************/

void loopHeart()
{ 
  int i=0;
  int j=0;
  planesOff();

  for(i=0;i<4;i++){
    /*----------------Also szint----------------*/
    // 16., 15., 12. LEDs
    digitalWrite(PlanePin[i],LOW);
    digitalWrite(LEDPin[15],HIGH);
    digitalWrite(LEDPin[14],HIGH);
    digitalWrite(LEDPin[11],HIGH);

    delay(50);
    /*-------------Masodik szint-------------*/
    // 14-es LED
    digitalWrite(PlanePin[i],LOW);
    digitalWrite(LEDPin[13],HIGH);
    digitalWrite(LEDPin[7],HIGH);  
    delay(50);

    /*-------------Harmadik szint-----------*/
    // 3-as LED
    digitalWrite(PlanePin[i],LOW);
    digitalWrite(LEDPin[2],HIGH);
    digitalWrite(LEDPin[3],HIGH);
    digitalWrite(LEDPin[8],HIGH);
    digitalWrite(LEDPin[12],HIGH);
    delay(50);

    /*---------------Felso reteg--------------*/
    // 2-es LED
    digitalWrite(PlanePin[i],LOW);
    digitalWrite(LEDPin[1],HIGH);
    digitalWrite(LEDPin[5],HIGH);
    digitalWrite(LEDPin[4],HIGH);
    delay(50);


    if(i==3){
      delay(500);
      for(j=3;j>=0;j--){
        digitalWrite(PlanePin[i],HIGH);
        digitalWrite(LEDPin[15],LOW);
        digitalWrite(LEDPin[14],LOW);
        digitalWrite(LEDPin[11],LOW);
        //-----------------------------
        delay(100);  
        digitalWrite(PlanePin[i],HIGH);
        digitalWrite(LEDPin[13],LOW);
        digitalWrite(LEDPin[7],LOW);
        //------------------------------
        delay(100);
        digitalWrite(PlanePin[i],HIGH);
        digitalWrite(LEDPin[2],LOW);
        digitalWrite(LEDPin[3],LOW);
        digitalWrite(LEDPin[8],LOW);
        digitalWrite(LEDPin[12],LOW);
        //------------------------------
        delay(100);
        digitalWrite(PlanePin[i],HIGH);
        digitalWrite(LEDPin[1],LOW);
        digitalWrite(LEDPin[5],LOW);
        digitalWrite(LEDPin[4],LOW);
        delay(200);
      }
    }
  }
  cubeOff();
} 

/*********************** Snake ***********************/

void snake(){
  int i;
  int j;
  int snakeCub[]={
    0, 4, 8, 12, 13, 14, 15, 11, 7, 3, 2, 1, 0 };

  planesOff();

  for(i=0;i<4;i++){
    digitalWrite(PlanePin[i],LOW);
    for(j=0; j<12; j++){
      if(i==0){
        digitalWrite(LEDPin[snakeCub[j]],HIGH);
        delay(100);
        digitalWrite(LEDPin[snakeCub[j]],LOW); 
      }
      if(i>0){
        digitalWrite(LEDPin[snakeCub[j+1]],HIGH);
        delay(100);
        digitalWrite(LEDPin[snakeCub[j+1]],LOW);
      }
    }
    digitalWrite(PlanePin[i],HIGH);  


  }
  cubeOff();
}

/*********************** Wall ***********************/

void loopWall(){
  int i=0;
  int j=0;
  int k=0;
  int cubeOne[]={
    14, 15, 16, 17    };
  int cubeTwo[]={
    14, 13, 7, 6    };
  //planesOff();
  
   for(i=3;i>=0;i--){
    for(j=0;j<100;j++){
      digitalWrite(PlanePin[i],LOW);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],HIGH);
      }

      delay(1);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],LOW);
      }

     /* if (j==99){
        for(k=0;k<16;k++){
          digitalWrite(LEDPin[k],HIGH);
        }
      }  */
      digitalWrite(PlanePin[3],LOW);      
      digitalWrite(PlanePin[2],LOW);
      digitalWrite(PlanePin[1],LOW);
      digitalWrite(PlanePin[0],LOW);
      for(k=0;k<4;k++){
        digitalWrite(cubeOne[k],HIGH);
        digitalWrite(cubeTwo[k],HIGH);
      }
      delay(1);
      digitalWrite(PlanePin[3],HIGH);
      digitalWrite(PlanePin[2],HIGH);
      digitalWrite(PlanePin[1],HIGH);
      digitalWrite(PlanePin[0],HIGH);          
    }

    if(i>0)
    {
      digitalWrite(PlanePin[i+1],HIGH);
    }   
    //delay(1);
  }      
  
  for(i=1;i<4;i++){
    for(j=0;j<100;j++){
      digitalWrite(PlanePin[i],LOW);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],HIGH);
      }

      delay(1);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],LOW);
      }

     /* if (j==99){
        for(k=0;k<16;k++){
          digitalWrite(LEDPin[k],HIGH);
        }
      }  */
      digitalWrite(PlanePin[3],LOW);      
      digitalWrite(PlanePin[2],LOW);
      digitalWrite(PlanePin[1],LOW);
      digitalWrite(PlanePin[0],LOW);
      for(k=0;k<4;k++){
        digitalWrite(cubeOne[k],HIGH);
        digitalWrite(cubeTwo[k],HIGH);
      }
      delay(1);
      digitalWrite(PlanePin[3],HIGH);
      digitalWrite(PlanePin[2],HIGH);
      digitalWrite(PlanePin[1],HIGH);
      digitalWrite(PlanePin[0],HIGH);          
    }

    if(i>0)
    {
      digitalWrite(PlanePin[i-1],HIGH);
    }   
    //delay(1);
  }      
  /********************************Elorol-hatra mozgatas*************************************/
  /*                                    Hatra                                               */
  /******************************************************************************************/

  int cubeRight[4][4]={
    {
      12, 13, 14, 15        }
    ,
    {
      8, 9, 10, 11        }
    ,
    {
      4, 5, 6, 7        }
    ,
    {
      0, 1, 2, 3        }
  };
  for(i=0;i<4;i++){
    for(j=0;j<100;j++){
      digitalWrite(PlanePin[3],LOW);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],HIGH);
      }

      delay(1);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],LOW);
      }

      //      if (j==99){
      //       for(k=0;k<16;k++){
      //        digitalWrite(LEDPin[k],HIGH);
      //           }
      //      }  
      digitalWrite(PlanePin[3],LOW);      
      digitalWrite(PlanePin[2],LOW);
      digitalWrite(PlanePin[1],LOW);
      digitalWrite(PlanePin[0],LOW);
      for(k=0;k<4;k++){  
        digitalWrite(LEDPin[cubeRight[i][k]],HIGH);
        digitalWrite(cubeOne[k],HIGH);
      }
      delay(1);         
      digitalWrite(PlanePin[3],HIGH);
      digitalWrite(PlanePin[2],HIGH);
      digitalWrite(PlanePin[1],HIGH);
      digitalWrite(PlanePin[0],HIGH); 
      for(k=0;k<4;k++){  
        digitalWrite(LEDPin[cubeRight[i][k]],LOW);
      }



    }
    //delay(1);
  }      

  /********************************Jobbra balra mozgatas*************************************/
  /*                                    Jobbra                                              */
  /******************************************************************************************/

  cubeOff();
  for(i=3;i>=0;i--){
    for(j=0;j<99;j++){

      digitalWrite(PlanePin[3],LOW);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],HIGH);
      }

      delay(1);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],LOW);
      }

      //  if (j==99){
      //    for(k=0;k<16;k++){
      //      digitalWrite(LEDPin[k],HIGH);
      //          }
      //      }  
      digitalWrite(PlanePin[3],LOW);      
      digitalWrite(PlanePin[2],LOW);
      digitalWrite(PlanePin[1],LOW);
      digitalWrite(PlanePin[0],LOW);
      for(k=0;k<4;k++){
        digitalWrite(LEDPin[cubeRight[i][k]],HIGH);
        digitalWrite(cubeOne[k],HIGH);
      }
      delay(1);          
      digitalWrite(PlanePin[3],HIGH);
      digitalWrite(PlanePin[2],HIGH);
      digitalWrite(PlanePin[1],HIGH);
      digitalWrite(PlanePin[0],HIGH);   
      for(k=3;k>=0;k--){
        digitalWrite(LEDPin[cubeRight[i][k]],LOW);

      }       
    }
    //delay(1);
  }


  /*****************************************************************************/
  /*                             Balrol-jobbra                                 */
  /*****************************************************************************/


  int cubeFor[4][4]={
    {
      0, 4, 8, 12        }
    ,
    {
      1, 5, 9, 13        }
    ,
    {
      2, 6, 10, 14        }
    ,
    {
      3, 7, 11, 15        }
  };
  for(i=0;i<4;i++){
    for(j=0;j<100;j++){
      digitalWrite(PlanePin[3],LOW);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],HIGH);
      }

      delay(1);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],LOW);
      }

      //      if (j==99){
      //       for(k=0;k<16;k++){
      //        digitalWrite(LEDPin[k],HIGH);
      //           }
      //      }  
      digitalWrite(PlanePin[3],LOW);      
      digitalWrite(PlanePin[2],LOW);
      digitalWrite(PlanePin[1],LOW);
      digitalWrite(PlanePin[0],LOW);
      for(k=0;k<4;k++){  
        digitalWrite(LEDPin[cubeFor[i][k]],HIGH);
        digitalWrite(cubeTwo[k],HIGH);
      }
      delay(1);         
      digitalWrite(PlanePin[3],HIGH);
      digitalWrite(PlanePin[2],HIGH);
      digitalWrite(PlanePin[1],HIGH);
      digitalWrite(PlanePin[0],HIGH); 
      for(k=0;k<4;k++){  
        digitalWrite(LEDPin[cubeFor[i][k]],LOW);
      }



    }
    //delay(1);
  }      

  /*********************************************************************/

  for(i=2;i>=0;i--){
    for(j=0;j<100;j++){
      digitalWrite(PlanePin[3],LOW);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],HIGH);
      }

      delay(1);
      for(k=0;k<16;k++){
        digitalWrite(LEDPin[k],LOW);
      }

      //      if (j==99){
      //       for(k=0;k<16;k++){
      //        digitalWrite(LEDPin[k],HIGH);
      //           }
      //      }  
      digitalWrite(PlanePin[3],LOW);      
      digitalWrite(PlanePin[2],LOW);
      digitalWrite(PlanePin[1],LOW);
      digitalWrite(PlanePin[0],LOW);
      for(k=0;k<4;k++){  
        digitalWrite(LEDPin[cubeFor[i][k]],HIGH);
        digitalWrite(cubeTwo[k],HIGH);
      }
      delay(1);         
      digitalWrite(PlanePin[3],HIGH);
      digitalWrite(PlanePin[2],HIGH);
      digitalWrite(PlanePin[1],HIGH);
      digitalWrite(PlanePin[0],HIGH); 
      for(k=3;k>=0;k--){  
        digitalWrite(LEDPin[cubeFor[i][k]],LOW);
      }



    }
    //delay(1);
  }      

}

/*********************** Cube ***********************/

void loopCube(){
  int i=0;
  int j=0;
  int k=0;
  int Cube[]={
    0, 1, 2, 3, 7, 11, 15, 14, 13, 12, 8, 4 };
  int Column[]={
    0, 3, 15, 12 };

  planesOff();
  for(j=0;j<350;j++){ 
    for(i=0;i<4;i++)
    {
      if(i==0)
      {
        digitalWrite(PlanePin[i],LOW);
        for(k=0;k<13;k++){
          digitalWrite(LEDPin[Cube[k]],HIGH);
        }
        delay(5);
        digitalWrite(PlanePin[i],HIGH);
      }

      if(i==3)
      {
        digitalWrite(PlanePin[i],LOW);
        for(k=0;k<13;k++){
          digitalWrite(LEDPin[Cube[k]],HIGH);
        }
        delay(5);
        digitalWrite(PlanePin[i],HIGH);

      }

      if(i==1 || i==2)
      {
        digitalWrite(PlanePin[i],LOW);
        for(k=0;k<13;k++){
          digitalWrite(LEDPin[Cube[k]],LOW);
        }
        for(k=0;k<5;k++){
          digitalWrite(LEDPin[Column[k]],HIGH);
        }
        delay(5);
        digitalWrite(PlanePin[i],HIGH);
      }

    }
  }
  cubeOff();
}

/*********************** Column ***********************/

void loopColum()
{
  int i=0;
  int r=0;
  int k=0;
  int j=0;
  int ledArray[16];

  //Tömb feltöltése
  for(i=0;i<16;i++)
  {
    ledArray[i]=i;
  }

  // Tömb randomizálása
  for(i=0;i<3;i++)
  {
    r=random(15);
    if(r==0){
      k=ledArray[r];
      ledArray[r]=ledArray[r+1];
      ledArray[r+1]=k;
    }
    if(r==15){
      k=ledArray[r];
      ledArray[r]=ledArray[r-1];
      ledArray[r-1]=k;
    }
    if(r%2==0){
      k=ledArray[r];
      ledArray[r]=ledArray[r-2];
      ledArray[r-2]=k;
    }
    if(r%2==1){
      k=ledArray[r];
      ledArray[r]=ledArray[r+2];
      ledArray[r+2]=k;
    }
  }
  planesOff();
  for(i=0;i<16;i++){
    for(j=3;j>=0;j--){
      digitalWrite(PlanePin[j],LOW);
      digitalWrite(LEDPin[ledArray[i]],HIGH);

      delay(50);

      digitalWrite(PlanePin[j],HIGH);
      digitalWrite(LEDPin[ledArray[i]],LOW);
    }
  }

  cubeOff();
}  

/*********************** EQU ***********************/

void equalizer(){
  int cubePlane[15];
  int i = 0;
  int j = 0;
  cubeOff();
}

/******************** PlanesOff *********************/

void planesOff(){
  for(int thisPlane = 0; thisPlane < PlanePinCount; thisPlane++){
    digitalWrite(PlanePin[thisPlane],HIGH);
  }
}

/********************* CubeOff **********************/

void cubeOff(){
  int i;
  for(i=0;i<16;i++)
  {
    digitalWrite(LEDPin[i],LOW);
  }
  for(i=0;i<4;i++)
  { 
    digitalWrite(PlanePin[i],HIGH);
  }
}


