#include "Arduino.h"
#include "MEDB_Comms.h"
#include "Wire.h"


static MEDB_RECEIVER *globalLib;

void staticOnReceiveHandler(int numBytesReceived) {
    globalLib->onReceiveHandler(numBytesReceived);
}

void staticOnRequestHandler() {
    globalLib->onRequestHandler();
}

/* ---------------- MEDB ---------------- */

MEDB::MEDB(int address) {
  // globalLib = this;
  _address = address;
}

/*
Start i2c connections
*/
void MEDB::begin() {
	Wire.begin(_address);  // open i22
}

/* ---------------- MEDB_CONNTROLLER ---------------- */

MEDB_CONNTROLLER::MEDB_CONNTROLLER(): MEDB(CONTROLLER_ADDRESS) {
}

// set target device to a response mode
bool MEDB_CONNTROLLER::SetUnitToMode(int targetAddress, mode selectedMode) {
	static SendMessageStruct modeBlock;
	modeBlock.targetMode = selectedMode;
	
	Wire.beginTransmission(targetAddress);
	Wire.write((byte*) &modeBlock, sizeof(modeBlock));
	Wire.endTransmission();
	
	return true;
}

// get IdentifyStatusStruct from target device
bool MEDB_CONNTROLLER::requestIdentifyStatus(byte address) {
	identifyStatus = IdentifyStatusStruct(); // clear identifyStatus struct
	byte numBytes = sizeof(identifyStatus);  // get size
	// request the data be send. returns false if not a valid sender
	_request(address, numBytes, IDENTIFY);
	Wire.readBytes( (byte*) &identifyStatus, numBytes); // read identifyStatus
	return true;
}

// get status from the power board device
bool MEDB_CONNTROLLER::requestPowerStatus() {	
	powerBoardStatus = PowerBoardStatusStruct(); // clear powerBoardStatus struct
	byte numBytes = sizeof(powerBoardStatus); // get size
	// request the data be send. returns false if not a valid sender
	_request(POWER_DISTRO_BOARD_ADDRESS, numBytes, STATUS);
	Wire.readBytes( (byte*) &powerBoardStatus, numBytes); // read powerBoardStatus
	return true;
}

// get status from the fire control device
bool MEDB_CONNTROLLER::requestFireControlStatus() {	
	fireControlStatus = fireControlStatusStruct(); // clear fireControlStatus struct
	byte numBytes = sizeof(fireControlStatus); // get size
	// request the data be send. returns false if not a valid sender
	_request(FIRE_CONTROL_BOARD_ADDRESS, numBytes, STATUS);
	Wire.readBytes( (byte*) &fireControlStatus, numBytes); // read fireControlStatus
	return true;
}

// get status from the chrono device
bool MEDB_CONNTROLLER::requestChronoStatus() {	
	chronoStatus = chronoStatusStruct(); // clear chronoStatus struct
	byte numBytes = sizeof(chronoStatus); // get size
	// request the data be send. returns false if not a valid sender
	_request(CHRONO_BOARD_ADDRESS, numBytes, STATUS);
	Wire.readBytes( (byte*) &chronoStatus, numBytes); // read chronoStatus
	return true;
	 
}

// attempts to connect to a given device
bool MEDB_CONNTROLLER::checkDeviceInSystem(byte address) {
	requestIdentifyStatus(address);  // get device status
	if (strlen(identifyStatus.version) != 0){  // see if a version was placed
		return true;
	}
	return false;
}

// Handles requesting data froma device
bool MEDB_CONNTROLLER::_request(byte address, byte numBytes, mode type) {
	SetUnitToMode(address, type); // set to the given type
	byte stop = true;
	Wire.requestFrom(address, numBytes, stop);  // request data
	return true;
}


/* ---------------- MEDB_RECEIVER ---------------- */

MEDB_RECEIVER::MEDB_RECEIVER(int address): MEDB(address) {
  globalLib = this;
}

void MEDB_RECEIVER::begin() {
	Wire.begin(_address);  // open i2c

	Wire.onReceive(staticOnReceiveHandler); 
	Wire.onRequest(staticOnRequestHandler);
}

void MEDB_RECEIVER::onReceiveHandler(int numBytesReceived) {
	Wire.readBytes( (byte*) &recivedData, numBytesReceived);
	_currentMode = recivedData.targetMode;
}

// add a function to call if a status request is made
void MEDB_RECEIVER::connectOnRequestStatusFunction(void (*funct)()) {	
	_onRequestStatusFunction = funct;
}

// add a function to call if a identify request is made
void MEDB_RECEIVER::connectOnRequestIdentifyFunction(void (*funct)()) {	
	_onRequestIdentifyFunction = funct;
}

// add a function to call if a settings request is made
void MEDB_RECEIVER::connectOnRequestSettingsFunction(void (*funct)()) {	
	_onRequestSettingsFunction = funct;
}

// Handles what function to call on a requestbased on current mode
void MEDB_RECEIVER::onRequestHandler() {
	if (_currentMode == STATUS){
		_onRequestStatusFunction();
	} else if (_currentMode == IDENTIFY) {
		_onRequestIdentifyFunction();
	} else if (_currentMode == SETTINGS) {
		_onRequestSettingsFunction();
	} else { // invalid _currentMode
		return;
	}
	Wire.write((byte*) &sendStruct, sizeof(sendStruct));  // send data
}





