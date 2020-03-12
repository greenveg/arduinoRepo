
void loadFromEeprom() {
  int address = 0;
  float floatValue = 0.0;
  int intValue = 0;

  for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
    EEPROM.get(address, floatValue);
    prices[i] = floatValue;
    address += sizeof(floatValue);
  }

  for (int i = 0; i < NUMBER_OF_SLOTS; i++) {
    EEPROM.get(address, intValue);
    stash[i] = intValue;
    address += sizeof(intValue);
  }

  for (int i = 0; i < NUMBER_OF_COINS; i++) {
    EEPROM.get(address, floatValue);
    coinValues[i] = floatValue;
    address += sizeof(floatValue);
  }
}

void writeToEeprom() {
  int address = 0;

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
