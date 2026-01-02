#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include "DigitalIO.h"

#include "Parameters.h"

// quick accesses settings
#define DEBUG_MODE true

void setup() {
  // inputs
  tiggerPin.mode(INPUT_PULLUP);
  breachPin.mode(INPUT_PULLUP);
  plungerPin.mode(INPUT_PULLUP);
  safetyPin.mode(INPUT_PULLUP);
  FireModePin1.mode(INPUT_PULLUP);
  FireModePin2.mode(INPUT_PULLUP);
  // outputs
  LEDPin.mode(OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  stop_motor(); // turn off motor on boot

  Serial.begin(9600); // initialize serial communication:
  Serial.println("starting up");
}

void loop() {
  if (DEBUG_MODE) {  // if debug print states
    if (millis() - lastDevMessage >= 2*1000UL) 
    {
      lastDevMessage = millis();
      // dev_write_serial_all_states();
    }
  }

  // update all
  update_trigger_state();
  update_sensor_state();
  update_safety_state();
  update_fire_mode();
  
  // if trigger has been released clear variables
  if (!currentTriggerState) {
    triggerReleased = true;
    burstCount = 0;
  }
  if (safetyState) { // safty is on
    if (currentTriggerState and !blasterSetup and triggerReleased) {  // do initial setup
      Serial.println("setting up");
      dev_write_serial_all_states();
      triggerReleased = false;  // Require the trigger to be released
      sensorState targetsensorState;
      if (idlePossition == DEPRIMED_IDLE) {
        targetsensorState = CLOSED_BREACH;
      } else if (idlePossition == PRIMED_IDLE) {
        targetsensorState = FIRE_READY;
      } else {
        Serial.println("invalid idle possition");
        return;
      }
      if (currentSensorState != targetsensorState) {
        run_motor();
        // waitTillSensorChangeDebounce(currentSensorState, targetsensorState);
        stop_motor();
      }
      dev_write_serial_all_states();
      blasterSetup = true;
    } else if (currentTriggerState) { // deprime
      if (triggerHoldTime == 0) {  // first time through after trigger pull
        if (triggerReleased) {
          Serial.println("starting trigger time");
          triggerReleased = false;
          triggerHoldTime = millis();
        }
      } else {  //trigger is being held down
        if ((millis() - triggerHoldTime) > 5000) {  // require 5 second hold to deprime
          if (currentSensorState != CLOSED_BREACH) {
            run_motor();
            // waitTillSensorChangeDebounce(currentSensorState, CLOSED_BREACH);
            stop_motor();
          }
          blasterSetup = false;
          triggerHoldTime = 0;
        }
      }
    }
  } else if (currentTriggerState and triggerReleased) {  // fire next dart
    switch (selectedFireMode) {
      case SINGLE_FIRE:
        fire();
        triggerReleased = false;  // Require the trigger to be released
        break;
      case BURST_FIRE:
        fire();
        burstCount += 1;
        if (burstCount >= burstLimit) { // once burst limit has been reached
          triggerReleased = false; // Require the trigger to be released
        }
        break;
      case AUTO_FIRE:
        fire();
        break;
    }
  } 
}

void fire() {
  // default state machine to expected start location.
  Serial.println("-------------------- Firing --------------------");
  int fireStart = millis(); // start fire rate timmer   
  fireStateMachine(); // go through firing process
  int fireStop = millis();  // stop fire timer
  //               max fire rate - time firing
  int delaytime = (1000/maxDPS) - (fireStop - fireStart);
  delay(delaytime);
  if (delaytime < 10) { // if delay too low
    delaytime = 10;
  }
  Serial.println(delaytime);
}

void fireStateMachine() {
  // TODO this is blocking. should it be? can this be improved. 
  // issue could happen where user releases trigger during firing and pull again before completion. 
  // Dart won't fire but user would expect it to.
  sensorState startStopPossition;
  if (idlePossition == DEPRIMED_IDLE) {
    startStopPossition = CLOSED_BREACH;
  } else if (idlePossition == PRIMED_IDLE) {
    startStopPossition = FIRE_READY;
  }
  nextState = LEAVING_STARTING_POSSITION;
  while(true) { // loop till firing complete.
    if (DEBUG_MODE) { // if debug print states
      update_sensor_state();
      dev_write_serial_all_states();
    }
    switch (nextState) {
      case LEAVING_STARTING_POSSITION:
        run_motor();
        if (!waitTillSensorChangeDebounce(startStopPossition)) { // make sure motor drives off of base possition
          Serial.println("Error from leaving start possition");
          nextState = ERROR_STATE;
          break;
        }
        nextState = CYCLE_TO_END;
        break;
      case CYCLE_TO_END:
        run_motor();
        // TODO could hit 0,1 before 1,1 need to handle
        if (!waitTillSensorChangeDebounce(currentSensorState)) { // , startStopPossition)) { // drive till end possition
          Serial.println("Error from predrive LOADING_STATE");
          nextState = ERROR_STATE;
          break;
        }
        stop_motor();
        nextState = COMPLETE_STATE;
        break;
      case COMPLETE_STATE:
        // clear firing related variables
        stop_motor();
        delay(100);  // here for debugging only TODO remove
        update_sensor_state();
        Serial.println(currentSensorState);
        return;
      case ERROR_STATE:
        //something went wrong
        // TODO inform user
        stop_motor();
        Serial.println("Error State");
        update_trigger_state();
        update_sensor_state();
        update_safety_state();
        update_fire_mode();
        dev_write_serial_all_states();
        return;
      default:
        stop_motor();
        return;
    }
  }
}

bool waitTillSensorChangeDebounce(int initial_state) {
  // blocking

  unsigned long total_wait_time = millis(); // start timeout timer
  unsigned long wait_debounce_time = micros(); // start time of debounce

  while (true) {
    update_sensor_state();
    if (currentSensorState == initial_state){ // we want any non initial state
      wait_debounce_time = micros();  // reset the debouncing timer
    }
    
    if ((micros() - wait_debounce_time) > 100) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:
      if (currentSensorState != initial_state){ // we want any non initial state
        break;
      }   
    }
    if ((millis() - total_wait_time) > 200) { // 200mSec is one cycle time at 5 dps. should reduce.
      return false;
    }
  }

  return true;
}

bool waitTillSensorChangeDebounce_old(int initial_state, list target_state) {
  // blocking

  unsigned long total_wait_time = millis(); // start timeout timer
  unsigned long wait_debounce_time = micros(); // start time of debounce

  while (true) {
    update_sensor_state();
    if (target_state == -1) {  // -1 means any other state is acceptable
      if (currentSensorState == initial_state){ // we want any non initial state
        wait_debounce_time = micros();  // reset the debouncing timer
      }
    } else {
      if (currentSensorState != target_state){ // we want the target state
        wait_debounce_time = micros();  // reset the debouncing timer
      }
    }

    if ((micros() - wait_debounce_time) > 100) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:
      if (target_state == -1) { // -1 means any other state is acceptable
        if (currentSensorState != initial_state){ // we want any non initial state
          break;
        }
      } else {
        if (currentSensorState == target_state){ // we want the target state
          break;
        }
      }     
    }
    if ((millis() - total_wait_time) > 200) { // 200mSec is one cycle time at 5 dps. should reduce.
      return false;
    }
  }

  return true;
}

// motor function
void run_motor() {
  analogWrite(MOTOR_PIN, 255);
}

void stop_motor() {
  analogWrite(MOTOR_PIN, 0);
}