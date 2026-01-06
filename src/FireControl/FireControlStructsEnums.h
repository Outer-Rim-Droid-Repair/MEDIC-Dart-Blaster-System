#ifndef FIRE_CONTROL_ENUMS_h
#define FIRE_CONTROL_ENUMS_h

#include "Arduino.h"

// TODO don't like that this has to be here for it to build
// if this needs to be here then it needs to be renamed to be clear what each do
// convert to s


enum fireMode // valid firemodes
{
  SINGLE_FIRE, // Fire one dart
  BURST_FIRE, // Fire darts till burstLimit is hit. Stop on trigger release
  AUTO_FIRE  // Fire untill trigger released
};
const char* fireModeStr[] = {"Single Fire", "Burst Fire", "Full Auto"};

struct fireModeSetup {
  fireMode mode;
  int burstCount;
};

enum sensorState   // Internal fire sensors
{                  // BREACH_PIN,PLUNGER_PIN
  MID_CYCLE,       // b00 no dart loaded and plubger not in possition
  PRIMED,          // b01 No dart loaded and plunger in possition.
  CLOSED_BREACH,   // b10 mid cycle or post firing pre cycle. dart may or may not be loaded with plunger not in back possition
  FIRE_READY       // b11  Ready to fire. Dart loaded, plunger primmed
};
const char* sensorStateStr[] = {"MID_CYCLE", "PRIMED", "CLOSED_BREACH", "FIRE_READY"};

enum firingStates           // States used for the fire state machine
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

#endif