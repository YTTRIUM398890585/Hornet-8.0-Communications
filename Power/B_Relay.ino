void set_relay() {
  if (!relay_state) {
    if (!(low_battery || shut_down)) {  // low batt means should resurface thus it would not turn on after turning off
      relay_state = HIGH;
      digitalWrite(WarmW_LED, relay_state);
      digitalWrite(relay_set, HIGH);
      delay(relay_set_pulse);
      digitalWrite(relay_set, LOW);
      debug_serial_print(debug_print_on, "relay on");
      MCP_begin();
    }
  }
}

void reset_relay() {
  if (relay_state) {
    relay_state = LOW;
    digitalWrite(WarmW_LED, relay_state);
    digitalWrite(relay_reset, HIGH);
    delay(relay_set_pulse);
    digitalWrite(relay_reset, LOW);
    debug_serial_print(debug_print_on, "relay off");
  }
}
