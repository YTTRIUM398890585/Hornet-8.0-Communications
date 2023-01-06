void debug_serial_print(bool debug_on, String msg) {
  if (debug_on) {
    Serial.println(msg);
  }
}

float custom_EEPROM_get_float(uint8_t address) {
  float x;
  EEPROM.get(address, x);
  return x;
}
