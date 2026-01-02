#include "pins_arduino.h"
// input pins
#define TRIGGER_PIN_NUMBER 19
#define BREACH_PIN_NUMBER 18
#define PLUNGER_PIN_NUMBER 20   
#define SAFETY_PIN_NUMBER 12
#define FIRE_SELECT_PIN_1_NUMBER 21
#define FIRE_SELECT_PIN_2_NUMBER 22
// output pins
#define LED_PIN_NUMBER 13
#define MOTOR_PIN 11
// LCD Pins
// curently not used but reserved if needed
#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5

// input high speed pins
DigitalPin<TRIGGER_PIN_NUMBER> tiggerPin;
DigitalPin<BREACH_PIN_NUMBER> breachPin;
DigitalPin<PLUNGER_PIN_NUMBER> plungerPin;
DigitalPin<SAFETY_PIN_NUMBER> safetyPin;
DigitalPin<FIRE_SELECT_PIN_1_NUMBER> FireModePin1;
DigitalPin<FIRE_SELECT_PIN_2_NUMBER> FireModePin2;
// output high speed pins
DigitalPin<LED_PIN_NUMBER> LEDPin;

// enums
enum fireMode // valid firemodes
{
  SINGLE_FIRE, // Fire one dart
  BURST_FIRE, // Fire darts till burstLimit is hit. Stop on trigger release
  AUTO_FIRE  // Fire untill trigger released
};
const char* fireModeStr[] = {"Single Fire", "Burst Fire", "Full Auto"};

enum sensorState   // Internal fire sensors
{                  // BREACH_PIN,PLUNGER_PIN
  MID_CYCLE,       // b00 no dart loaded and plubger not in possition
  PRIMED,          // b01 No dart loaded and plunger in possition.
  CLOSED_BREACH,   // b10 mid cycle or post firing pre cycle. dart may or may not be loaded with plunger not in back possition
  FIRE_READY       // b11  Ready to fire. Dart loaded, plunger primmed
};
const char* sensorStateStr[] = {"MID_CYCLE", "PRIMED", "CLOSED_BREACH", "FIRE_READY"};

enum states           // States used for the fire state machine
{
  DEPRIME_STATE,      // Loader foward, Plunger forward
  LOADING_STATE,      // Loader moving back, Plunger moving back
  LOADED_STATE,       // Loader forwad, Plunger moving back
  PRIMED_STATE,       // Loader moving forward, Plunger back
  FIRE_READY_STATE,   // Loader forward, Plunger back
  FIRING_STATE,       // Loader forward, Plunger moving forward
  COMPLETE_STATE,     // firing cycle complete
  ERROR_STATE,
  LEAVING_STARTING_POSSITION,
  CYCLE_TO_END
};
const char* stateMachineStr[] = {"DEPRIME_STATE", "LOADING_STATE", "LOADED_STATE", "PRIMED_STATE", "FIRE_READY_STATE", "FIRING_STATE", "COMPLETE_STATE", "ERROR_STATE"};

enum idleMode       //possible idle possitions
{                   // BREACH_PIN,PLUNGER_PIN
  DEPRIMED_IDLE,    // 1,0: full idle. spring deprimmed. safe possition. should return here before stoage
  PRIMED_IDLE       // 1,1: fire prime. most responsive possition
};
const char* idleModeStr[] = {"DEPRIMED_IDLE", "PRIMED_IDLE"};

// tracking
// TODO some of these where added then never used as read
volatile int currentTriggerState = LOW;           // Is the trigger pulled? High: pulled, LOW: released           
int lastTriggerState = LOW;                       // History of currentTriggerState 
volatile int currentSensorState = CLOSED_BREACH;  // Internal fire sensors read. Default to CLOSED_BREACH as this should be power down state
int lastSensorState = CLOSED_BREACH;              // History of currentSensorState
volatile int safetyState = LOW;                   // Is the Safty Enabled? High: Enabled, LOW: Disabled  
volatile fireMode selectedFireMode = SINGLE_FIRE; // User selected fire mode. TODO allow a user selected array of fire modes
states nextState = DEPRIME_STATE;                // Firing State machine state. Default to DEPRIME_STATE as this should be power down state

bool triggerReleased = true;                      // Has the trigger been released after the last shot. Used to force trigger release when needed
int burstCount = 0;                               // How many shots have been fired 

bool blasterSetup = false;                        // cycles to correct starting possition
long triggerHoldTime = 0;                         // Used for timeing how long the trigger has beenheld down

unsigned long triggerDebounceTime = 2;            // for trigger debounce
unsigned long lastTriggerDebounce = 0;            // for trigger debounce


long lastDevMessage = 0;

// User controlled settings.
idleMode idlePossition = PRIMED_IDLE;          // where should the firing cycle end?
int burstLimit = 3;                               // How many darts in a burst fire.
// Could make this a by mode option. i.e. user could select burst 2 and burst 4 as their fire modes
int maxDPS = 5;                                  // limit on number of darts per second