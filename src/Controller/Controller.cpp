#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Controller.h"
#include "MEDIC_Comms.h"



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MEDIC_CONNTROLLER communicator;

void setup() {
  Serial.begin(9600);

  // Wait for display
  delay(500);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    // TODO handle failure differently
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  display.clearDisplay();
  testdrawroundrect(); // Draw rounded rectangles (outlines)
  delay(1000); // Pause for 1 seconds

  communicator = MEDIC_CONNTROLLER();
  communicator.begin();
  delay(1000); // Pause for 1 seconds
  powerBoardPresent = communicator.checkDeviceInSystem(POWER_DISTRO_BOARD_ADDRESS);
  fireControlPresent = communicator.checkDeviceInSystem(FIRE_CONTROL_BOARD_ADDRESS);
  chronoPresent = communicator.checkDeviceInSystem(CHRONO_BOARD_ADDRESS);

}

void loop() {
}

// --------------------- screens ---------------------
void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}


void drawNetworkScreen(void) {
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  display.print(F("Power Board Present: "));
  if (powerBoardPresent) {
    display.println(F("True"));
  } else {
    display.println(F("False"));
  }

  display.print(F("Fire Control Board Present: "));
  if (fireControlPresent) {
    display.println(F("True"));
  } else {
    display.println(F("False"));
  }

  display.print(F("Chrono Board Present: "));
  if (chronoPresent) {
    display.println(F("True"));
  } else {
    display.println(F("False"));
  }

  display.display();
}

