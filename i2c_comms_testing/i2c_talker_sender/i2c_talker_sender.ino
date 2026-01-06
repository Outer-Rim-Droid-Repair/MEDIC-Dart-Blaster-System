#include <MEDB_Comms.h>

MEDB_CONNTROLLER a;

bool PowerBoardPresent;
bool fireControlPresent;
bool chronoStatusPresent;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  a.begin();
  delay(500);
  PowerBoardPresent = a.checkDeviceInSystem(POWER_DISTRO_BOARD_ADDRESS);
  fireControlPresent = a.checkDeviceInSystem(FIRE_CONTROL_BOARD_ADDRESS);
  chronoStatusPresent = a.checkDeviceInSystem(CHRONO_BOARD_ADDRESS);
}

void loop() {
  Serial.print("Power Supply Present: ");
  Serial.print(PowerBoardPresent);
  Serial.print(", Fire Control Present: ");
  Serial.print(fireControlPresent);
  Serial.print(", Chrono Present: ");
  Serial.print(chronoStatusPresent);
  Serial.println("");
    
  delay(1000);
}