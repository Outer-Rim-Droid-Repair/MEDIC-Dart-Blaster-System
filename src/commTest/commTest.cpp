#include <Arduino.h>

#include "MEDIC_Comms.h"

void fillStatus();
void fillIdentifier();
void setSettings();
void fillSettings();

const char version[6] = "V0.1";

MEDIC_CHRONO_RECEIVER communicator = MEDIC_CHRONO_RECEIVER();

void setup() {
  //start serial connection
  Serial.begin(9600);
  delay(1000);

  pinMode(13, OUTPUT);

  // communicator = MEDIC_RECEIVER(CHRONO_BOARD_ADDRESS); //MEDIC_CHRONO_RECEIVER();
  communicator.connectOnRequestIdentifyFunction(fillIdentifier);
  communicator.connectOnRequestSettingsFunction(fillSettings);
  communicator.connectSetSettingFunction(setSettings);
  communicator.connectOnRequestStatusFunction(fillStatus);
  communicator.begin();
}

void loop() {
    static bool heartbeat = true;

    heartbeat = !heartbeat;
    Serial.println(heartbeat);
    delay(1000);
}


void fillStatus() {
  float unitAdjuster;
  communicator.statusStruct.lastFPS = 123.4;
  communicator.statusStruct.maxFPS = 234.5;
  communicator.statusStruct.minFPS = 12.3;
  communicator.statusStruct.maxDPS = 345.6;
  communicator.statusStruct.lastDPS = -1;  // TODO implemet
}

void fillIdentifier() {
  strcpy(communicator.identifyStruct.version, version);
}

void setSettings() {
}

void fillSettings() {
  
}