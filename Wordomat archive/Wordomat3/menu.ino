const String printAsInt(const float value) {
  return String((int) value);
}

class PriceMenuItem : public NumericMenuItem {
  public:
    PriceMenuItem(const char* basename, int number)
      : NumericMenuItem(basename, nullptr,
                        0,
                        0,
                        9001,
                        0.1)
    {
      _number = number;
    };

    Menu* select()
    {
      _has_focus = !_has_focus;

      // Only save values when the user is done editing the value
      if (!_has_focus) {
        prices[_number] = _value;
        writeToEeprom();
      }

      return nullptr;
    }

  protected:
    int _number;
};

class CoinValueMenuItem : public NumericMenuItem {
  public:
    CoinValueMenuItem(const char* basename, int number)
      : NumericMenuItem(basename, nullptr,
                        0,
                        0,
                        9001,
                        0.1)
    {
      _number = number;
    };

    Menu* select()
    {
      _has_focus = !_has_focus;

      // Only save values when the user is done editing the value
      if (!_has_focus) {
        coinValues[_number] = _value;
        writeToEeprom();
      }

      return nullptr;
    }

  protected:
    int _number;
};

class StashMenuItem : public NumericMenuItem {
  public:
    StashMenuItem(const char* basename, int number)
      : NumericMenuItem(basename, nullptr,
                        0,
                        0,
                        9001,
                        1,
                        printAsInt)
    {
      _number = number;
    };

    Menu* select()
    {
      _has_focus = !_has_focus;

      // Only save values when the user is done editing the value
      if (!_has_focus) {
        stash[_number] = (int) _value;
        writeToEeprom();
      }

      return nullptr;
    }

  protected:
    int _number;
};

// Menu variables



MenuItem mm_mi1("Test solenoids", &testAllSolenoids);
MenuItem mm_mi2("Test motor", &testMotorCallback);
MenuItem mm_mi3("Test buttons", &testButtonsCallback);
MenuItem mm_mi4("Open hatch", &openHatchCallback);
MenuItem mm_mi5("Close hatch", &closeHatchCallback);

PriceMenuItem slot_1_price("Slot 1 price", 0);
PriceMenuItem slot_2_price("Slot 2 price", 1);
PriceMenuItem slot_3_price("Slot 3 price", 2);
PriceMenuItem slot_4_price("Slot 4 price", 3);
PriceMenuItem slot_5_price("Slot 5 price", 4);
PriceMenuItem slot_6_price("Slot 6 price", 5);
PriceMenuItem slot_7_price("Slot 7 price", 6);
PriceMenuItem slot_8_price("Slot 8 price", 7);
PriceMenuItem slot_9_price("Slot 9 price", 8);
PriceMenuItem slot_10_price("Slot 10 price", 9);
PriceMenuItem slot_11_price("Slot 11 price", 10);
PriceMenuItem slot_12_price("Slot 12 price", 11);
PriceMenuItem slot_13_price("Slot 13 price", 12);
PriceMenuItem slot_14_price("Slot 14 price", 13);
PriceMenuItem slot_15_price("Slot 15 price", 14);
PriceMenuItem slot_16_price("Slot 16 price", 15);
PriceMenuItem slot_17_price("Slot 17 price", 16);

StashMenuItem slot_1_stash("Slot 1 stash", 0);
StashMenuItem slot_2_stash("Slot 2 stash", 1);
StashMenuItem slot_3_stash("Slot 3 stash", 2);
StashMenuItem slot_4_stash("Slot 4 stash", 3);
StashMenuItem slot_5_stash("Slot 5 stash", 4);
StashMenuItem slot_6_stash("Slot 6 stash", 5);
StashMenuItem slot_7_stash("Slot 7 stash", 6);
StashMenuItem slot_8_stash("Slot 8 stash", 7);
StashMenuItem slot_9_stash("Slot 9 stash", 8);
StashMenuItem slot_10_stash("Slot 10 stash", 9);
StashMenuItem slot_11_stash("Slot 11 stash", 10);
StashMenuItem slot_12_stash("Slot 12 stash", 11);
StashMenuItem slot_13_stash("Slot 13 stash", 12);
StashMenuItem slot_14_stash("Slot 14 stash", 13);
StashMenuItem slot_15_stash("Slot 15 stash", 14);
StashMenuItem slot_16_stash("Slot 16 stash", 15);
StashMenuItem slot_17_stash("Slot 17 stash", 16);

CoinValueMenuItem coin_1_value("Coin 1 value", 0);
CoinValueMenuItem coin_2_value("Coin 2 value", 1);
CoinValueMenuItem coin_3_value("Coin 3 value", 2);
CoinValueMenuItem coin_4_value("Coin 4 value", 3);
CoinValueMenuItem coin_5_value("Coin 5 value", 4);
CoinValueMenuItem coin_6_value("Coin 6 value", 5);
