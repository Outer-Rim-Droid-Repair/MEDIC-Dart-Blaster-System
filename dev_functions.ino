/* Functions to show varables for dev work */


void dev_write_serial_all_states() {
  /* Write variables to serial port */
  Serial.print("System State: ");
  Serial.print("Safety: ");
  Serial.print(safetyState);
  Serial.print(" Trigger: ");
  Serial.print(currentTriggerState);
  Serial.print(" Fire Mode: ");
  Serial.print(fireModeStr[selectedFireMode]);
  Serial.print(" Loading State: ");
  Serial.print(currentSensorState);
  Serial.print(": ");
  Serial.print(sensorStateStr[currentSensorState]);
  Serial.print(" State: ");
  Serial.print(stateMachineStr[nextState]);
  Serial.println("");
}
