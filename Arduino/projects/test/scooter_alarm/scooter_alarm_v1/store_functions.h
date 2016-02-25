boolean storeStatesInEEPROM(byte operation, int address, byte states){
  // operation - 0: write, 1: read
  // address - address in EEPROM
  // state - state to store
  if (operation == cREADFROMEEPROM){
    EEPROM.update(address, states);
  }
  else (operation ==cWRITETOEEPROM){
    
  }
}
