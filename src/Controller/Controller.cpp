#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Controller.h"
#include "MEDIC_Comms.h"

const char version[6] = "V0.1";


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


MEDIC_CONNTROLLER communicator;
//GFXcanvas1 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);



void setup() {
  Serial.begin(9600);

  // Wait for display
  delay(500);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;) {
      Serial.println(F("SSD1306 allocation failed"));
      delay(1000);
    } // Don't proceed, loop forever
    // TODO handle failure differently
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  display.clearDisplay();
  drawTestPattern();
  delay(1000); // Pause for 1 seconds

  communicator = MEDIC_CONNTROLLER();
  communicator.begin();
  delay(1000); // Pause for 1 seconds
  powerBoardPresent = communicator.checkDeviceInSystem(POWER_DISTRO_BOARD_ADDRESS);
  fireControlPresent = communicator.checkDeviceInSystem(FIRE_CONTROL_BOARD_ADDRESS);
  chronoPresent = communicator.checkDeviceInSystem(CHRONO_BOARD_ADDRESS);

  char powerBoardVersion[6] = "N/A";  // char[6] 
  char fireControlVersion[6] = "N/A";  // char[6] 
  char chronoVersion[6] = "N/A";  // char[6] 
  if (powerBoardPresent) {
    communicator.requestIdentifyStatus(POWER_DISTRO_BOARD_ADDRESS);
    strcpy(powerBoardVersion, communicator.identifyStatus.version);
  }
  if (fireControlPresent) {
    communicator.requestIdentifyStatus(FIRE_CONTROL_BOARD_ADDRESS);
    strcpy(powerBoardVersion, communicator.identifyStatus.version);
  }
  if (chronoPresent) {
    communicator.requestIdentifyStatus(CHRONO_BOARD_ADDRESS);
    strcpy(powerBoardVersion, communicator.identifyStatus.version);
  }
  Serial.println(powerBoardVersion);
  drawNetworkScreenBackground();
  drawNetworkScreensInfo(powerBoardVersion, fireControlVersion, chronoVersion);
  delay(1000);
  canvasChronoScreenBackground();
}


int i = 0;
void loop() {
  delay(100);
  communicator.requestChronoStatus();
  canvasChronoScreenInfo();
  Serial.println(i);
  i++;
  // drawTestPattern();
}

// --------------------- screens ---------------------
void drawTestPattern(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void drawNetworkScreenBackground() {
  display.fillScreen(SSD1306_BLACK);  // clear screen
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);  // boarder rect
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // init text settings
  display.setTextSize(1);

  display.setCursor(82, 3);
  display.print("Version");

  display.setCursor(3, 13);
  display.print("Controller  :");
  display.setCursor(3, 23);
  display.print("Power Board :");
  display.setCursor(3, 33);
  display.print("Fire Control:");
  display.setCursor(3, 43);
  display.print("Chrono      :");
  display.display();
}

void drawNetworkScreensInfo(char *powerBoardVersion, char *FireControlVersion, char *ChronoVersion) {
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // init text settings
  display.setTextSize(1);
  display.setCursor(82, 13);
  display.print(version);
  display.setCursor(82, 23);
  display.print(powerBoardVersion);
  display.setCursor(82, 33);
  display.print(FireControlVersion);
  display.setCursor(82, 43);
  display.print(ChronoVersion);

  display.display();
}

void canvasChronoScreenBackground(void) {
  display.fillScreen(SSD1306_BLACK);  // clear screen
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);  // boarder rect
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // init text settings
  display.setTextSize(1);

  // draw lines
  display.drawFastHLine(0, 32, SCREEN_WIDTH, SSD1306_WHITE);
  display.drawFastHLine(0, 48, SCREEN_WIDTH, SSD1306_WHITE);
  display.drawFastVLine(32, 64, 32, SSD1306_WHITE);

  display.setCursor(20, 6);
  display.print("Last FPS:");

  // FPS
  display.setCursor(20, 22);
  display.print("FPS");
  display.setCursor(2, 35);
  display.print("Max:");
  display.setCursor(2, 51);
  display.print("Min:");

  // DPS
  display.setCursor(90, 22);
  display.print("DPS");
  display.setCursor(66, 35);
  display.print("Last:");
  display.setCursor(66, 51);
  display.print("Max:");

  display.display();
}

void canvasChronoScreenInfo(void) {
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // init text settings
  display.setTextSize(1);

  display.setCursor(75, 6);
  display.print(communicator.chronoStatus.lastFPS, 1);

  display.setCursor(27, 35);
  display.print(communicator.chronoStatus.maxFPS, 1);
  display.setCursor(27, 51);
  display.print(communicator.chronoStatus.minFPS, 1);

  display.setCursor(96, 35);
  display.print(communicator.chronoStatus.lastDPS, 1);
  display.setCursor(91, 51);
  display.print(communicator.chronoStatus.maxDPS, 1);

  display.display();
}
