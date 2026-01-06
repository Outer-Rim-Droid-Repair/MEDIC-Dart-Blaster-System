// input pins
#define TRIGGER_PIN_NUMBER 19
#define BREACH_PIN_NUMBER 18
#define PLUNGER_PIN_NUMBER 20   
#define SAFETY_PIN_NUMBER 12
#define FIRE_SELECT_PIN_1_NUMBER 21
#define FIRE_SELECT_PIN_2_NUMBER 22
// output pins
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
  LEAVING_STARTING_POSSITION, // make sure system is not in the start stop possition
  CYCLE_TO_PRIMED,            // if idle mode is PRIMED_IDLE go to FIRE_READY
  CYCLE_TO_DEPRIMED,          //if idle mode is DEPRIMED_IDLE go to CLOSED_BREACH
  COMPLETE_STATE,             // firing cycle complete
  ERROR_STATE
};
const char* stateMachineStr[] = {"LEAVING_STARTING_POSSITION", "CYCLE_TO_PRIMED", "CYCLE_TO_DEPRIMED", "COMPLETE_STATE", "ERROR_STATE"};

enum idleMode       //possible idle possitions
{                   // BREACH_PIN,PLUNGER_PIN
  DEPRIMED_IDLE,    // 1,0: full idle. spring deprimmed. safe possition. should return here before stoage
  PRIMED_IDLE       // 1,1: fire prime. most responsive possition
};
const char* idleModeStr[] = {"DEPRIMED_IDLE", "PRIMED_IDLE"};

// tracking
volatile int currentTriggerState = LOW;           // Is the trigger pulled? High: pulled, LOW: released           
int lastTriggerState = LOW;                       // History of currentTriggerState 
volatile int currentSensorState = CLOSED_BREACH;  // Internal fire sensors read. Default to CLOSED_BREACH as this should be power down state
volatile int safetyState = LOW;                   // Is the Safty Enabled? High: Enabled, LOW: Disabled  
volatile fireMode selectedFireMode = SINGLE_FIRE; // User selected fire mode. TODO allow a user selected array of fire modes
states nextState;                                 // Firing State machine state. 

// User controlled settings.
idleMode idlePossition = PRIMED_IDLE;             // where should the firing cycle end?
int burstLimit = 3;                               // How many darts in a burst fire.
// Could make this a by mode option. i.e. user could select burst 2 and burst 4 as their fire modes
int maxDPS = 5;                                  // limit on number of darts per second


// functions
void fire();
void fireStateMachine();
bool waitTillSensorChange(int initial_state);
void _getAllSensorStatesBut(int *list, int state);
bool waitTillSensorChangeToValue(int target_state);
bool waitTillSensorChangeToValue(int target_states[], int length);
bool isValueInList(int value, int list[], int length);
void run_motor();
void stop_motor();
void update_sensor_state();
void update_trigger_state();
void update_safety_state();
void update_fire_mode();

void dev_write_serial_all_states();
