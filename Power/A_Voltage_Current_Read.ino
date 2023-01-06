float average_five_read(byte analog_pin) {
  float val = 0;
  for (uint8_t i = 0; i < 5; i++) {
    val = val + analogRead(analog_pin);
    delay(1);
  }
  return val / 5;
}

float read_current(bool calibrating) {
  static uint16_t zero_ref = 512;

  if (calibrating && relay_state) {
    digitalWrite(Pink_LED, HIGH);
    reset_relay();
    delay(500);
    zero_ref = average_five_read(current_sensor);
    debug_serial_print(debug_print_on , (String)"calibrated: zero_ref = " + zero_ref);
    digitalWrite(Pink_LED, LOW);
    set_relay();
  } else if (calibrating && !relay_state) {
    digitalWrite(Pink_LED, HIGH);
    reset_relay();
    delay(500);
    zero_ref = average_five_read(current_sensor);
    debug_serial_print(debug_print_on, (String)"calibrated: zero_ref = " + zero_ref);
    digitalWrite(Pink_LED, LOW);
  }

  float raw_current = average_five_read(current_sensor);
  float processed_current = (zero_ref - raw_current) * 125 / 1024;
  //  debug_serial_print(debug_print_on, (String)"raw_current = " + raw_current + "    processed_current = " + processed_current);

  return processed_current;
}

float read_voltage(bool batt, bool calibrating) {
  static float batt_A_multiplier = custom_EEPROM_get_float(batt_A_calibrated_address);
  static float batt_B_multiplier = custom_EEPROM_get_float(batt_B_calibrated_address);

  if (calibrating) {
    if (!batt) {
      digitalWrite(Pink_LED, HIGH);
      delay(500);
      batt_A_multiplier = 16.8 / average_five_read(batt_A_sensor);
      EEPROM.put(batt_A_calibrated_address, batt_A_multiplier);
      debug_serial_print(debug_print_on, (String)"Calibrated battery A, batt_A_multiplier = " + batt_A_multiplier);
      digitalWrite(Pink_LED, LOW);
    } else {
      digitalWrite(Pink_LED, HIGH);
      delay(500);
      batt_B_multiplier = 16.8 / average_five_read(batt_B_sensor);
      EEPROM.put(batt_B_calibrated_address, batt_B_multiplier);
      debug_serial_print(debug_print_on, (String)"Calibrated battery B, batt_B_mulitplier = " + batt_B_multiplier);
      digitalWrite(Pink_LED, LOW);
    }
  }

  if (!batt) {
    return average_five_read(batt_A_sensor) * batt_A_multiplier;
  } else {
    return average_five_read(batt_B_sensor) * batt_B_multiplier;
  }
}
