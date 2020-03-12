void setupExpanders() {
  // Pusher & motor board
  expander1.begin(EXPANDER_1_I2C_ADDRESS);
  expander2.begin(EXPANDER_2_I2C_ADDRESS);

  // Button board
  expander3.begin(EXPANDER_3_I2C_ADDRESS);
  expander4.begin(EXPANDER_4_I2C_ADDRESS);

  // Inner row
  pushers[0] = {&expander1, 10};
  pushers[1] = {&expander1, 11};
  pushers[2] = {&expander1, 12};
  pushers[3] = {&expander1, 13};
  pushers[4] = {&expander1, 14};
  pushers[5] = {&expander1, 15};
  pushers[6] = {&expander2, 9};
  pushers[7] = {&expander2, 8};
  pushers[8] = {&expander1, 6};

  // Outer row
  pushers[9] = {&expander1, 5};
  pushers[10] = {&expander1, 4};
  pushers[11] = {&expander1, 3};
  pushers[12] = {&expander1, 2};
  pushers[13] = {&expander1, 1};
  pushers[14] = {&expander1, 0};
  pushers[15] = {&expander1, 8};
  pushers[16] = {&expander1, 9}; // svarar inte

  buttons[0] = {&expander4, 5};
  buttons[1] = {&expander4, 6};
  buttons[2] = {&expander4, 7};
  buttons[3] = {&expander3, 8};
  buttons[4] = {&expander3, 9};
  buttons[5] = {&expander3, 10};
  buttons[6] = {&expander3, 11};
  buttons[7] = {&expander3, 12};
  buttons[8] = {&expander3, 13};
  buttons[9] = {&expander3, 14};
  buttons[10] = {&expander3, 15};
  buttons[11] = {&expander3, 0};
  buttons[12] = {&expander3, 1};
  buttons[13] = {&expander3, 2};
  buttons[14] = {&expander3, 3};
  buttons[15] = {&expander3, 4};
  buttons[16] = {&expander3, 5};

  for (int i = 0; i < NUMBER_OF_PUSHERS; i++) {
    pushers[i].expander->pinMode(pushers[i].pin, OUTPUT);
    pushers[i].expander->digitalWrite(pushers[i].pin, LOW);
  }

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    buttons[i].expander->pinMode(buttons[i].pin, INPUT);
    buttons[i].expander->pullUp(buttons[i].pin, HIGH);
  }

  motorPin.expander->pinMode(motorPin.pin, OUTPUT);
  motorPin.expander->digitalWrite(motorPin.pin, LOW);

  motorSensorPin.expander->pinMode(motorSensorPin.pin, INPUT);
  motorSensorPin.expander->pullUp(motorSensorPin.pin, HIGH);

  lockButtonsPin.expander->pinMode(lockButtonsPin.pin, OUTPUT);
  lockButtonsPin.expander->digitalWrite(lockButtonsPin.pin, LOW);
}

void loadValuesIntoMenu() {
  slot_1_price.set_value(prices[0]);
  slot_2_price.set_value(prices[1]);
  slot_3_price.set_value(prices[2]);
  slot_4_price.set_value(prices[3]);
  slot_5_price.set_value(prices[4]);
  slot_6_price.set_value(prices[5]);
  slot_7_price.set_value(prices[6]);
  slot_8_price.set_value(prices[7]);
  slot_9_price.set_value(prices[8]);
  slot_10_price.set_value(prices[9]);
  slot_11_price.set_value( prices[10]);
  slot_12_price.set_value( prices[11]);
  slot_13_price.set_value( prices[12]);
  slot_14_price.set_value( prices[13]);
  slot_15_price.set_value( prices[14]);
  slot_16_price.set_value( prices[15]);
  slot_17_price.set_value( prices[16]);

  slot_1_stash.set_value(stash[0]);
  slot_2_stash.set_value(stash[1]);
  slot_3_stash.set_value(stash[2]);
  slot_4_stash.set_value(stash[3]);
  slot_5_stash.set_value(stash[4]);
  slot_6_stash.set_value(stash[5]);
  slot_7_stash.set_value(stash[6]);
  slot_8_stash.set_value(stash[7]);
  slot_9_stash.set_value(stash[8]);
  slot_10_stash.set_value(stash[9]);
  slot_11_stash.set_value(stash[10]);
  slot_12_stash.set_value(stash[11]);
  slot_13_stash.set_value(stash[12]);
  slot_14_stash.set_value(stash[13]);
  slot_15_stash.set_value(stash[14]);
  slot_16_stash.set_value(stash[15]);
  slot_17_stash.set_value(stash[16]);

  coin_1_value.set_value(coinValues[0]);
  coin_2_value.set_value(coinValues[1]);
  coin_3_value.set_value(coinValues[2]);
  coin_4_value.set_value(coinValues[3]);
  coin_5_value.set_value(coinValues[4]);
  coin_6_value.set_value(coinValues[5]);
}

void buildMenu() {
  ms.get_root_menu().add_item(&mm_mi1);
  ms.get_root_menu().add_item(&mm_mi2);
  ms.get_root_menu().add_item(&mm_mi3);
  ms.get_root_menu().add_item(&mm_mi4);
  ms.get_root_menu().add_item(&mm_mi5);

  ms.get_root_menu().add_item(&slot_1_price);
  ms.get_root_menu().add_item(&slot_2_price);
  ms.get_root_menu().add_item(&slot_3_price);
  ms.get_root_menu().add_item(&slot_4_price);
  ms.get_root_menu().add_item(&slot_5_price);
  ms.get_root_menu().add_item(&slot_6_price);
  ms.get_root_menu().add_item(&slot_7_price);
  ms.get_root_menu().add_item(&slot_8_price);
  ms.get_root_menu().add_item(&slot_9_price);
  ms.get_root_menu().add_item(&slot_10_price);
  ms.get_root_menu().add_item(&slot_11_price);
  ms.get_root_menu().add_item(&slot_12_price);
  ms.get_root_menu().add_item(&slot_13_price);
  ms.get_root_menu().add_item(&slot_14_price);
  ms.get_root_menu().add_item(&slot_15_price);
  ms.get_root_menu().add_item(&slot_16_price);
  ms.get_root_menu().add_item(&slot_17_price);

  ms.get_root_menu().add_item(&slot_1_stash);
  ms.get_root_menu().add_item(&slot_2_stash);
  ms.get_root_menu().add_item(&slot_3_stash);
  ms.get_root_menu().add_item(&slot_4_stash);
  ms.get_root_menu().add_item(&slot_5_stash);
  ms.get_root_menu().add_item(&slot_6_stash);
  ms.get_root_menu().add_item(&slot_7_stash);
  ms.get_root_menu().add_item(&slot_8_stash);
  ms.get_root_menu().add_item(&slot_9_stash);
  ms.get_root_menu().add_item(&slot_10_stash);
  ms.get_root_menu().add_item(&slot_11_stash);
  ms.get_root_menu().add_item(&slot_12_stash);
  ms.get_root_menu().add_item(&slot_13_stash);
  ms.get_root_menu().add_item(&slot_14_stash);
  ms.get_root_menu().add_item(&slot_15_stash);
  ms.get_root_menu().add_item(&slot_16_stash);
  ms.get_root_menu().add_item(&slot_17_stash);

  ms.get_root_menu().add_item(&coin_1_value);
  ms.get_root_menu().add_item(&coin_2_value);
  ms.get_root_menu().add_item(&coin_3_value);
  ms.get_root_menu().add_item(&coin_4_value);
  ms.get_root_menu().add_item(&coin_5_value);
  ms.get_root_menu().add_item(&coin_6_value);
}

void setupLcd() {
  byte arrow[8] = {
    B00000,
    B10000,
    B11000,
    B11100,
    B11000,
    B10000,
    B00000,
    B00000,
  };

  lcd.begin(16, 2);
  lcd.createChar(0, arrow);
}

