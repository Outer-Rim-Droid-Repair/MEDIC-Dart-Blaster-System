#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RotaryEncoder.h>

#include "Controller.h"
#include "MEDIC_Comms/MEDIC_Comms.h"
#include "MEDIC_Comms/MEDIC_Comms.cpp"
#include "MEDIC_Comms/MEDIC_Screens.h"
#include "MEDIC_Comms/MEDIC_Screens.cpp"
#include "FireControl/FireControlStructsEnums.h"


const char version[6] = "V0.1";


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


MEDIC_CONNTROLLER communicator;

Version_Screen Version_Screen_Control;
Chrono_Screen Chrono_Screen_Control;
Fire_Control_Screen Fire_Control_Screen_Control;

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

  Version_Screen_Control = Version_Screen(display, &communicator);
  Chrono_Screen_Control = Chrono_Screen(display, &communicator);
  Fire_Control_Screen_Control = Fire_Control_Screen(display, &communicator);

  Version_Screen_Control.drawTestPattern();
  delay(1000); // Pause for 1 seconds

  updateVersionScreen();
  delay(1000);
}


int i = 0;
unsigned long lastupdate = 0;
const int updateSpeed = 100;
bool buttonUpdate = false;
bool editMode = false;
bool redrawBackground = true;

void loop() {
  
  if ((millis() - lastupdate >= updateSpeed) and (!editMode)){
    if (selectedScreenState == VERSION) {
      updateVersionScreen();
    } else if (selectedScreenState == FIRE_MODE_STATUS) {
      updateFireModeScreen();
    } else if (selectedScreenState == CHRONO_STATUS) {
      updateChronoStatusScreen();
    } else if (selectedScreenState == POWER_STATUS) {
      display.fillScreen(SSD1306_BLACK);  // clear screen
    } 
    // Version_Screen_Control.invertSection(20, 20 , 60, 60);
    lastupdate = millis();
  }
  readKeypad();
  if (buttonUpdate) {
    if (lastPressed == IN) {
      editMode = !editMode;
    }
    if (!editMode) {
      if (lastPressed == LEFT) {
        findNextValidScreen(false);
      } else if (lastPressed == RIGHT) {
        findNextValidScreen(true);
      }
    } else {
      if (currentScreenState == CHRONO_STATUS) {
        if (lastPressed == IN) {
          Chrono_Screen_Control.drawQuestionBox("Reset Data?");
          redrawBackground = true;       
        } else if (lastPressed == UP) {
          // reset chrono
          Serial.println("up");
          editMode = false;
        } else if (lastPressed == DOWN) {
          Serial.println("down");
          editMode = false;
        }
      }
    }
    buttonUpdate = false;
  }
  delay(20);
}

void findNextValidScreen(bool countUp) {
  int direction = 1;
  if (!countUp) {
    direction = -1;
  }

  unsigned int index = selectedScreenState;
  SCREEN_STATE state;
  while (true) {
    if (index <= 0 and !countUp) {
      index = sizeof(screenOrder);
    }
    index = (index + direction) % sizeof(screenOrder);
    state = screenOrder[index];

    if (state == VERSION) {
      selectedScreenState = VERSION;
      break;
    } else if ((state == CHRONO_STATUS) and (chronoPresent)) {
      selectedScreenState = CHRONO_STATUS;
      break;
    } else if ((state == FIRE_MODE_STATUS) and (!fireControlPresent)) {
      selectedScreenState = FIRE_MODE_STATUS;
      break;
    } else if ((state == POWER_STATUS) and (powerBoardPresent)) {
      selectedScreenState = POWER_STATUS;
      break;
    }

    if (index == selectedScreenState) {
      break;
    }
  }
  

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
  if (buttonState != lastButtonState && lastPressed != NONE) {
    //Serial.println(lastPressed);
    buttonUpdate = true;
  }

  lastButtonState = buttonState;
}

void updateConnectedDevices(void) {
  powerBoardPresent = communicator.checkDeviceInSystem(POWER_DISTRO_BOARD_ADDRESS);
  fireControlPresent = communicator.checkDeviceInSystem(FIRE_CONTROL_BOARD_ADDRESS);
  chronoPresent = communicator.checkDeviceInSystem(CHRONO_BOARD_ADDRESS);
}

// --------------------- screens ---------------------
// Version
void updateVersionScreen(void) {
  if ((currentScreenState != VERSION) or redrawBackground) {
      Version_Screen_Control.drawBackgrond();
      currentScreenState = VERSION;
      redrawBackground = false;
  }
  char controllerVersion[6] = "";
  char powerBoardVersion[6] = "N/A";  // char[6] 
  char fireControlVersion[6] = "N/A";  // char[6] 
  char chronoVersion[6] = "N/A";  // char[6] 

  strcpy(controllerVersion, version);
  if (powerBoardPresent) {
    communicator.requestIdentifyStatus(POWER_DISTRO_BOARD_ADDRESS);
    strcpy(powerBoardVersion, communicator.identifyStatus.version);
  }
  if (fireControlPresent) {
    communicator.requestIdentifyStatus(FIRE_CONTROL_BOARD_ADDRESS);
    strcpy(fireControlVersion, communicator.identifyStatus.version);
  }
  if (chronoPresent) {
    communicator.requestIdentifyStatus(CHRONO_BOARD_ADDRESS);
    strcpy(chronoVersion, communicator.identifyStatus.version);
  }
  Version_Screen_Control.drawInfo(controllerVersion, powerBoardVersion, fireControlVersion, chronoVersion);
}

// Chrono
void updateChronoStatusScreen(void) {
  if ((currentScreenState != CHRONO_STATUS) or redrawBackground) {
    Chrono_Screen_Control.drawBackgrond();
    currentScreenState = CHRONO_STATUS;
    redrawBackground = false;
  }
  //communicator.requestChronoStatus();
  //Chrono_Screen_Control.drawInfo();
}

// Fire Mode
void updateFireModeScreen(void) {
  if ((currentScreenState != FIRE_MODE_STATUS) or redrawBackground) {
    Fire_Control_Screen_Control.drawBackgrond();
    currentScreenState = FIRE_MODE_STATUS;
    redrawBackground = false;
  }
  //communicator.requestFireControlStatus();
  //Fire_Control_Screen_Control.drawInfo();
}
