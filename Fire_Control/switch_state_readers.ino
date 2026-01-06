/* Functions to read various sensors */

/*
Get internal switch reading and set it to currentSensorState. 
Use sensorState enum to access by name. 
format: BREACH_PIN,PLUNGER_PIN
*/
void update_sensor_state() {
  currentSensorState = 2 * (!breachPin) + (!plungerPin);
}

/*
Get the current trigger state and sets it to currentTriggerState. 
Uses none blocking debounced to prevent doble fires when trigger is released.
*/
void update_trigger_state() {
  const unsigned long triggerDebounceTime = 2;
  static unsigned long lastTriggerDebounce = 0;
  int reading = !tiggerPin;
  if (reading != lastTriggerState) {
    lastTriggerDebounce = millis();
    lastTriggerState = reading;
  }
  if ((millis() - lastTriggerDebounce) > triggerDebounceTime) {
    lastTriggerState = currentTriggerState;
    currentTriggerState = reading;
  }
}

/*
Get safty switch state and sets it to safetyState.
*/
void update_safety_state() {
  safetyState = safetyPin;
}

/*
Get fire mode option fromthe selector switch and sets it to selectedFireMode.
Use fireMode enum to access by name. 
*/
void update_fire_mode() {
  // TODO update with selecting fire mode from a user selected array
  if (!FireModePin1) {  // first possition
    selectedFireMode = SINGLE_FIRE;
  } else if (!FireModePin2) { // third possition
    selectedFireMode = AUTO_FIRE;
  } else {  // second possition
    selectedFireMode = BURST_FIRE;
  }
}