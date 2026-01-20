#ifndef Controller_h
#define Controller_h

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// keypad
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
int last_rotary = 0;

enum SCREEN_STATE {
    VERSION,
    CHRONO_STATUS,
    FIRE_MODE_STATUS,
    POWER_STATUS,
    OTHER
};
const SCREEN_STATE screenOrder[] = {VERSION, CHRONO_STATUS, FIRE_MODE_STATUS, POWER_STATUS};
SCREEN_STATE selectedScreenState = VERSION;
SCREEN_STATE currentScreenState = OTHER;

bool powerBoardPresent;
bool fireControlPresent;
bool chronoPresent;

void readKeypad(void);

void drawTestPattern(void);
void updateConnectedDevices(void);
void updateVersionScreen(void);
void updateChronoStatusScreen(void);
void updateFireModeScreen(void);


void drawNetworkScreenBackground() ;
void drawNetworkScreensInfo(char *powerBoardVersion, char *FireControlVersion, char *ChronoVersion);
void drawChronoScreenBackground(void);
void drawChronoScreenInfo(void);
void drawFireModeScreenBackground(void);
void drawFireModeScreenInfo(void);

#endif