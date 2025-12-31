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
      dev_write_serial_all_states();
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
        waitTillSensorChangeDebounce(currentSensorState, targetsensorState);
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
            waitTillSensorChangeDebounce(currentSensorState, CLOSED_BREACH);
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
  switch (currentSensorState) {
    case MID_CYCLE:
      nextState = LOADING_STATE;
      break;
    case PRIMED:
      // can only happen when breach is closing. 
      // Should be long closed before this check
      nextState = ERROR_STATE;   
      break;
    case CLOSED_BREACH:
      nextState = LOADING_STATE;
      break;
    case FIRE_READY:
      nextState = FIRING_STATE;
      break;
    default:
      Serial.println("invalid idle possition");
      return;
  }

  int fireStart = millis(); // start fire rate timmer   
  fireStateMachine(); // go through firing process
  int fireStop = millis();  // stop fire timer
  //               max fire rate - time firing
  int delaytime = (1000/maxDPS) - (fireStop - fireStart);
  if (delaytime < 10) { // if delay too low
    delaytime = 10;
  }
  delay(delaytime);
  Serial.println(delaytime);
}

void fireStateMachine() {
  // TODO this is blocking. should it be? can this be improved. issue could happen where user releases trigger during firing and pull again before completion. Dart won't fire but user would expect it to.
  while(true) { // loop till firing complete.
    if (DEBUG_MODE) { // if debug print states
      update_sensor_state();
      dev_write_serial_all_states();
    }
    switch (nextState) {
      case DEPRIME_STATE:
        // next state: LOADING_STATE or COMPLETE_STATE
        // if using deprimed idle go to complete
        // otherwise go to loading state
        stop_motor();
        if (idlePossition == DEPRIMED_IDLE) { // complete firing if required
          nextState = COMPLETE_STATE;
        } else {
          nextState = LOADING_STATE;
        }
        break;
      case LOADING_STATE:
        // next state: LOADED_STATE or PRIMED_STATE or FIRE_READY_STATE
        // run motor till sensor state is 0,1 or 1,0 or 1,1
        // if 0,1 go to loaded state
        // if 1,0 go to primed state
        // if 1,1 go to fire ready
        run_motor();
        if (!waitTillSensorChangeDebounce(CLOSED_BREACH, MID_CYCLE)) { // make sure motor drives off of base possition
          Serial.println("Error from predrive LOADING_STATE");
          nextState = ERROR_STATE;
          break;
        }

        if (waitTillSensorChangeDebounce(MID_CYCLE, -1)) {  // drive motor till state changes
          if (currentSensorState == PRIMED) {
            nextState = PRIMED_STATE;
          } else if (currentSensorState == CLOSED_BREACH) {
            nextState = LOADED_STATE;
          } else if (currentSensorState == FIRE_READY) {
            nextState = FIRE_READY_STATE;
          }
        } else {
          Serial.println("Error from main drive LOADING_STATE");
          nextState = ERROR_STATE;
        }
        break;
      case LOADED_STATE:
        // next state: FIRE_READY_STATE
        // run motor 
        // wait till sensor state is 1,1 then go to fire ready state
        run_motor();
        if (waitTillSensorChangeDebounce(CLOSED_BREACH, FIRE_READY)) { // drive motor till fire ready
          nextState = FIRE_READY_STATE;
        } else {
          Serial.println("Error from LOADED_STATE");
          nextState = ERROR_STATE;
        }
        break;
      case PRIMED_STATE:
        // next state: FIRE_READY_STATE
        // stop motor 
        // wait till sensor state is 1,1 then go to fire ready state
        stop_motor();
        if (waitTillSensorChangeDebounce(PRIMED, FIRE_READY)) { // wait for breach to fully close
          nextState = FIRE_READY_STATE;
        } else {
          Serial.println("Error from PRIMED_STATE");
          nextState = ERROR_STATE;
        }
        break;
      case FIRE_READY_STATE:
        // next state: FIRING_STATE or COMPLETE_STATE
        // stop motor
        // if using primed idle go to complete state
        // otherwise go to firing state
        stop_motor();
        if (idlePossition == PRIMED_IDLE) { // complete firing if required
          nextState = COMPLETE_STATE;
        } else {
          nextState = FIRING_STATE;
        }
        break;
      case FIRING_STATE:
        // next state: DEPRIME_STATE
        // run motor 
        // wait till sensor state is not 1,1 then go to DEPRIME_STATE
        run_motor();
        if (waitTillSensorChangeDebounce(FIRE_READY, -1)) { // run motor till off back sensor
          nextState = DEPRIME_STATE;
        } else {
          Serial.println("Error from FIRING_STATE");
          nextState = ERROR_STATE;
        }
        break;
      case COMPLETE_STATE:
        // clear firing related variables
        stop_motor();
        return;
      case ERROR_STATE:
        //something went wrong
        // TODO inform user
        stop_motor();
        return;
      default:
        stop_motor();
        return;
    }
  }
}

bool waitTillSensorChangeDebounce(int initial_state, int target_state) {
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