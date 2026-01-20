#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RotaryEncoder.h>

#include "Controller.h"
#include "MEDIC_Comms.h"
#include "FireControl/FireControlStructsEnums.h"

const char version[6] = "V0.1";


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


MEDIC_CONNTROLLER communicator;


void setup() {
  Serial.begin(9600);

  pinMode(COM_A, OUTPUT);
  digitalWrite(COM_A, LOW);
  pinMode(COM_B, OUTPUT);
  digitalWrite(COM_B, LOW);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_IN, INPUT_PULLUP);

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
  display.display();
  display.clearDisplay();
  communicator = MEDIC_CONNTROLLER();
  communicator.begin();

  drawTestPattern();
  delay(1000); // Pause for 1 seconds

  updateVersionScreen();
  delay(1000);
}


int i = 0;
unsigned long lastupdate = 0;
const int updateSpeed = 100;

void loop() {
  if (millis() - lastupdate >= updateSpeed){
    //updateChronoStatusScreen();
    //updateFireModeScreen();
  }
  readKeypad();
  delay(20);
}

void readKeypad(void) {
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
}

void updateConnectedDevices(void) {
  powerBoardPresent = communicator.checkDeviceInSystem(POWER_DISTRO_BOARD_ADDRESS);
  fireControlPresent = communicator.checkDeviceInSystem(FIRE_CONTROL_BOARD_ADDRESS);
  chronoPresent = communicator.checkDeviceInSystem(CHRONO_BOARD_ADDRESS);
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
  currentScreenState = OTHER;
}

// Version
void updateVersionScreen(void) {
  if (currentScreenState != VERSION) {
      drawNetworkScreenBackground();
  }
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
  drawNetworkScreensInfo(powerBoardVersion, fireControlVersion, chronoVersion);
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
  currentScreenState = VERSION;
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

// Chrono
void updateChronoStatusScreen(void) {
  if (currentScreenState != CHRONO_STATUS) {
    drawChronoScreenBackground();
  }
  communicator.requestChronoStatus();
  drawChronoScreenInfo();
}

void drawChronoScreenBackground(void) {
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
  currentScreenState = CHRONO_STATUS;
}

void drawChronoScreenInfo(void) {
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

// Fire Mode
void updateFireModeScreen(void) {
  if (currentScreenState != FIRE_MODE_STATUS) {
    drawFireModeScreenBackground();
  }
  //communicator.requestFireControlStatus();
  drawFireModeScreenInfo();
}

void drawFireModeScreenBackground(void) {
  display.fillScreen(SSD1306_BLACK);  // clear screen
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);  // boarder rect
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // init text settings
  display.setTextSize(1);

  display.drawFastVLine(12, 0, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawFastVLine(60, 0, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawFastHLine(0, 16, SCREEN_WIDTH, SSD1306_WHITE);
  display.drawFastHLine(0, 32, SCREEN_WIDTH, SSD1306_WHITE);
  display.drawFastHLine(0, 48, SCREEN_WIDTH, SSD1306_WHITE);

  display.setCursor(15, 5);
  display.print("Mode");
  display.setCursor(63, 5);
  display.print("Setting");

  display.setCursor(3, 20);
  display.print("1");
  display.setCursor(3, 36);
  display.print("2");
  display.setCursor(3, 52);
  display.print("3");

  display.display();
  currentScreenState = FIRE_MODE_STATUS;
}

void drawFireModeScreenInfo(void) {
  display.setCursor(15, 20);
  
  display.print(fireModeStr[communicator.fireControlSettings.selectableFireModes[0]]);
  display.setCursor(15, 36);
  display.print(fireModeStr[communicator.fireControlSettings.selectableFireModes[1]]);
  display.setCursor(15, 52);
  display.print(fireModeStr[communicator.fireControlSettings.selectableFireModes[2]]);

  display.setCursor(63, 20);
  display.print(communicator.fireControlSettings.selectableBurstAmounts[0], 1);
  display.setCursor(63, 36);
  display.print(communicator.fireControlSettings.selectableBurstAmounts[1], 1);
  display.setCursor(63, 52);
  display.print(communicator.fireControlSettings.selectableBurstAmounts[2], 1);

  display.display();
}
