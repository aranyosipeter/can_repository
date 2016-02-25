#ifndef header_h
#define header_h


typedef struct node {
  char address[6];
  byte serv;
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
