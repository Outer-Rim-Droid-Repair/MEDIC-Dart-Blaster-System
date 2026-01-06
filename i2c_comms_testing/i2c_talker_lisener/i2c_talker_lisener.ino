#include <MEDIC_Comms_Library.h>

char version[16] = "22222.0";

MEDB_RECEIVER a(POWER_DISTRO_BOARD_ADDRESS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  a.connectOnRequestIdentifyFunction(fillIdentifyStruct);
  a.connectOnRequestStatusFunction(fillPowerBoardStatusStruct);
  a.begin();
}

void loop() {
  //if (newRxEvent) {
  //  a.readNewMessage();
  //}
}

void fillIdentifyStruct() {
  IdentifyStatusStruct toFill;
  strcpy(toFill.version, version);
  memcpy(a.sendStruct, &toFill, sizeof(a.sendStruct));  // todo make class function that does
}

void fillPowerBoardStatusStruct() {
    
  PowerBoardStatusStruct toFill;
  toFill.cell_1 = 0.1;
  toFill.cell_2 = 1.2;
  toFill.cell_3 = 2.3;
  toFill.cell_4 = 3.4;
  toFill.cell_5 = 4.5;
  toFill.cell_6 = 5.6;
  toFill.totalVoltage = 123.45;
  toFill.cellCount = 6;
  //a.updateSendStruct(toFill);
  memcpy(a.sendStruct, &toFill, sizeof(a.sendStruct));  // todo make class function that does
}