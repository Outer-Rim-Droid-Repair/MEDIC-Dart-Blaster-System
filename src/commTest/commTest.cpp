#include <RotaryEncoder.h>

#define PIN_ENCODER_A 13
#define PIN_ENCODER_B 12
#define COM_A    11
#define COM_B    A5
#define BUTTON_UP 5
#define BUTTON_LEFT A4
#define BUTTON_DOWN 9
#define BUTTON_RIGHT 6
#define BUTTON_IN 10

enum POSSITIONS {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  IN,
  NONE
};
POSSITIONS lastPressed = NONE;

RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B, RotaryEncoder::LatchMode::TWO03);
// This interrupt will do our encoder reading/checking!
void checkPosition() {
  encoder.tick(); // just call tick() to check the state.
}
int last_rotary = 0;


void setup(void) {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("ANO Rotary Encoder Demo");

  pinMode(COM_A, OUTPUT);
  digitalWrite(COM_A, LOW);
  pinMode(COM_B, OUTPUT);
  digitalWrite(COM_B, LOW);

  //attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), checkPosition, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), checkPosition, CHANGE);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_IN, INPUT_PULLUP);
}


void loop(void) {
  // read encoder
  encoder.tick();
  int curr_rotary = encoder.getPosition();
  RotaryEncoder::Direction direction = encoder.getDirection();
  
  if (curr_rotary != last_rotary) {
    Serial.print("Encoder value: ");
    Serial.print(curr_rotary);
    Serial.print(" direction: ");
    Serial.println((int)direction);
  }
  last_rotary = curr_rotary;
  int buttonState = LOW;
  static int lastButtonState = HIGH;

  if (! digitalRead(BUTTON_UP)) {
    lastPressed = UP;
  } else if (! digitalRead(BUTTON_LEFT)) {
    lastPressed = LEFT;
  } else if (! digitalRead(BUTTON_DOWN)) {
    lastPressed = DOWN;
  } else if (! digitalRead(BUTTON_RIGHT)) {
    lastPressed = RIGHT;
  } else if (! digitalRead(BUTTON_IN)) {
    lastPressed = IN;
  } else {
    lastPressed = NONE;
    buttonState = HIGH;
  }
  if (buttonState != lastButtonState) {
    Serial.println(lastPressed);
  }

  lastButtonState = buttonState;
  delay(20);
}