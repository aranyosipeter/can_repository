// Manage SQL queries

/*
2015.09.16. [Performance] Use F() function to store strings in progmem
*/

/*********************************** Data log to SQL database **********************************/
void dataLogSQL(){
    char query[64];
    for(int i = 0; i < 8; i++) {
      switch(i) {
        case 0:  
          sprintf(query, UPDATE, dhtTemp, i);
          break;
        case 1: 
          sprintf(query, UPDATE, dhtHum, i);
          break;
        case 2: 
          sprintf(query, UPDATE, bmpTemp, i);
          break;
        case 3: 
          sprintf(query, UPDATE, bmpPreshPa, i);
          break;
        case 4: 
          sprintf(query, UPDATE, dhtHum, i);
          break;
        case 5: 
          sprintf(query, UPDATE, bmpPreshPa, i);
          break;
        case 6: 
          sprintf(query, UPDATE, buttonValue, i);
          break;
        //case 7: 
        //  sprintf(query, UPDATE, val, i);
        //  break;
      }
      my_conn.cmd_query(query);
      delay(250);
    }
    Serial.println(F("UPDATED!!"));
}


