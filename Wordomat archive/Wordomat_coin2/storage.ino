
int loadFromEeprom() {
  int address = 4;
  int intValue = 0;
  if (EEPROM.read(0) == CONFIG_VERSION[0] &&
      EEPROM.read(1) == CONFIG_VERSION[1] &&
      EEPROM.read(2) == CONFIG_VERSION[2]){

    for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
      EEPROM.get(address, intValue);
      prices[i] = intValue;
      address += sizeof(intValue);
    }
  
    for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
      EEPROM.get(address, intValue);
      stash[i] = intValue;
      address += sizeof(intValue);
    }
  
    for (int i = 0; i < NUMBER_OF_COINS; i++) {
      EEPROM.get(address, intValue);
      coinValues[i] = intValue;
      address += sizeof(intValue);
    }
    return 1;
  }else{
    return 0;
  }
}

void writeToEeprom() {
  int address = 4;
  for(int i=0;i<3;i++){
   EEPROM.write(i,CONFIG_VERSION[i]);
  }

  for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
    EEPROM.put(address, prices[i]);
    address += sizeof(prices[i]);
  }

  for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
    EEPROM.put(address, stash[i]);
    address += sizeof(stash[i]);
  }

  for (int i = 0; i < NUMBER_OF_COINS; i++) {
    EEPROM.put(address, coinValues[i]);
    address += sizeof(coinValues[i]);
  }
}
