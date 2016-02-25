#ifndef MyStructs_h
#define MyStructs_h


typedef struct node {
  byte manu;
  byte id;
  byte state;
  boolean role;
};

typedef struct nodepin {
  int pins[3];
  int mode[3];
  int value[3];
};

#endif 
