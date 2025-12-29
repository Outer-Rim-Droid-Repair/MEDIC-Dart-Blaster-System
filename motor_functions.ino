/* Functions to manage motors */


void run_motor() {
  setMotorSpeed(255);
}

void stop_motor() {
  setMotorSpeed(0);
}

void setMotorSpeed(int speed) {
  analogWrite(MOTOR_PIN, speed);
}