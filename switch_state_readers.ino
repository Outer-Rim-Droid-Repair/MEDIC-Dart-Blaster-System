/* Functions to read various sensors */


void update_sensor_state() {
  lastSensorState = currentSensorState;
  currentSensorState = 2 * (!breachPin) + (!plungerPin);
}

void update_trigger_state() {
  lastTriggerState = currentTriggerState;
  currentTriggerState = !tiggerPin;
}

void update_safety_state() {
  safetyState = safetyPin;
}

void update_fire_mode() {
  // TODO update with selecting fire mode from a user selected array
  if (!FireModePin1) {
    selectedFireMode = SINGLE_FIRE;
  } else if (!FireModePin2) {
    selectedFireMode = AUTO_FIRE;
  } else {
    selectedFireMode = BURST_FIRE;
  }
}