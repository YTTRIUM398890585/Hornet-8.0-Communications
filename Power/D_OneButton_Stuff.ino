void checkTicks() {
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

// this function will be called when the button was pressed 1 time only.
// on relay
void singleClick() {
  set_relay();
}   

// this function will be called when the button was pressed 2 times in a short timeframe.
// calibrate current
void doubleClick() {
  read_current(true);
}

// this function will be called when the button was pressed multiple times in a short timeframe.
void multiClick() {
  int n = button.getNumberClicks();
  if (n == 3) {
    // calibrate voltage
    read_voltage(0, true);
    read_voltage(1, true);
  } else if (n == 4) {
    Serial.println("quadrupleClick detected.");
  } else {
    Serial.print("multiClick(");
    Serial.print(n);
    Serial.println(") detected.");
  }
} // multiClick


// this function will be called when the button was held down for 1 second or more.
// off relay
void pressStart() {
  Serial.println("pressStart()");
  reset_relay();
  pressStartTime = millis() - 1000; // as set in setPressTicks()
} // pressStart()


// this function will be called when the button was released after a long hold.
void pressStop() {
  Serial.print("pressStop(");
  Serial.print(millis() - pressStartTime);
  Serial.println(") detected.");
} // pressStop()
  
