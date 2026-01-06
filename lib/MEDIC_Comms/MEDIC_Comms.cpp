#include "Arduino.h"
#include "MEDIC_Comms.h"
#include "Wire.h"


static MEDIC_RECEIVER *globalLib;

void staticOnReceiveHandler(int numBytesReceived) {
    globalLib->onReceiveHandler(numBytesReceived);
}

void staticOnRequestHandler() {
    globalLib->onRequestHandler();
}

/* ---------------- MEDIC ---------------- */

MEDIC::MEDIC(int address) {
  // globalLib = this;
  _address = address;
}

/*
Start i2c connections
*/
void MEDIC::begin() {
	Wire.begin(_address);  // open i22
}

/* ---------------- MEDIC_CONNTROLLER ---------------- */

MEDIC_CONNTROLLER::MEDIC_CONNTROLLER(): MEDIC(CONTROLLER_ADDRESS) {
}

// set target device to a response mode
bool MEDIC_CONNTROLLER::SetUnitToMode(int targetAddress, mode selectedMode) {
	static SendMessageStruct modeBlock;
	modeBlock.targetMode = selectedMode;
	
	Wire.beginTransmission(targetAddress);
	Wire.write((byte*) &modeBlock, sizeof(modeBlock));
	Wire.endTransmission();
	
	return true;
}

// get IdentifyStatusStruct from target device
void MEDIC_CONNTROLLER::requestIdentifyStatus(byte address) {
	identifyStatus = IdentifyStatusStruct(); // clear identifyStatus struct
	byte numBytes = sizeof(identifyStatus);  // get size
	// request the data be send. returns false if not a valid sender
	_request(address, numBytes, IDENTIFY);
	Wire.readBytes( (byte*) &identifyStatus, numBytes); // read identifyStatus
}

// get status from the power board device
void MEDIC_CONNTROLLER::requestPowerStatus() {	
	powerBoardStatus = PowerBoardStatusStruct(); // clear powerBoardStatus struct
	byte numBytes = sizeof(powerBoardStatus); // get size
	// request the data be send. returns false if not a valid sender
	_request(POWER_DISTRO_BOARD_ADDRESS, numBytes, STATUS);
	Wire.readBytes( (byte*) &powerBoardStatus, numBytes); // read powerBoardStatus
}

// get status from the fire control device
void MEDIC_CONNTROLLER::requestFireControlStatus() {	
	fireControlStatus = fireControlStatusStruct(); // clear fireControlStatus struct
	byte numBytes = sizeof(fireControlStatus); // get size
	// request the data be send. returns false if not a valid sender
	_request(FIRE_CONTROL_BOARD_ADDRESS, numBytes, STATUS);
	Wire.readBytes( (byte*) &fireControlStatus, numBytes); // read fireControlStatus
}

// get status from the chrono device
void MEDIC_CONNTROLLER::requestChronoStatus() {	
	chronoStatus = chronoStatusStruct(); // clear chronoStatus struct
	byte numBytes = sizeof(chronoStatus); // get size
	// request the data be send. returns false if not a valid sender
	_request(CHRONO_BOARD_ADDRESS, numBytes, STATUS);
	Wire.readBytes( (byte*) &chronoStatus, numBytes); // read chrono Status 
}

void MEDIC_CONNTROLLER::requestPowerSettings() {
	powerBoardSettings = PowerBoardSettingsStruct(); // clear powerBoardSettings struct
	byte numBytes = sizeof(powerBoardSettings); // get size
	// request the data be send. returns false if not a valid sender
	_request(POWER_DISTRO_BOARD_ADDRESS, numBytes, SETTINGS);
	Wire.readBytes( (byte*) &powerBoardSettings, numBytes); // read powerBoardSettings
}

void MEDIC_CONNTROLLER::requestFireControlSettings() {
	fireControlSettings = fireControlSettingsStruct(); // clear fireControlSettings struct
	byte numBytes = sizeof(fireControlSettings); // get size
	// request the data be send. returns false if not a valid sender
	_request(FIRE_CONTROL_BOARD_ADDRESS, numBytes, SETTINGS);
	Wire.readBytes( (byte*) &fireControlSettings, numBytes); // read fireControlSettings
}

void MEDIC_CONNTROLLER::requestChronoSettings() {
	chronoSettings = chronoSettingsStruct(); // clear chronoSettings struct
	byte numBytes = sizeof(chronoSettings); // get size
	// request the data be send. returns false if not a valid sender
	_request(CHRONO_BOARD_ADDRESS, numBytes, SETTINGS);
	Wire.readBytes( (byte*) &chronoSettings, numBytes); // read chronoSettings
}

void MEDIC_CONNTROLLER::setPowerSettings() {}
void MEDIC_CONNTROLLER::setFireControlSettings() {}
void MEDIC_CONNTROLLER::setChronoSettings() {}



// attempts to connect to a given device
bool MEDIC_CONNTROLLER::checkDeviceInSystem(byte address) {
	requestIdentifyStatus(address);  // get device status
	if (strlen(identifyStatus.version) != 0){  // see if a version was placed
		return true;
	}
	return false;
}

// Handles requesting data froma device
bool MEDIC_CONNTROLLER::_request(byte address, byte numBytes, mode type) {
	SetUnitToMode(address, type); // set to the given type
	byte stop = true;
	Wire.requestFrom(address, numBytes, stop);  // request data
	return true;
}


/* ---------------- MEDIC_RECEIVER ---------------- */

MEDIC_RECEIVER::MEDIC_RECEIVER(int address): MEDIC(address) {
  globalLib = this;
}

void MEDIC_RECEIVER::begin() {
	Wire.begin(_address);  // open i2c

	Wire.onReceive(staticOnReceiveHandler); 
	Wire.onRequest(staticOnRequestHandler);
}

// add a function to call if a status request is made
void MEDIC_RECEIVER::connectOnRequestStatusFunction(void (*funct)()) {	
	_onRequestStatusFunction = funct;
}

// add a function to call if a identify request is made
void MEDIC_RECEIVER::connectOnRequestIdentifyFunction(void (*funct)()) {	
	_onRequestIdentifyFunction = funct;
}

// add a function to call if a settings request is made
void MEDIC_RECEIVER::connectOnRequestSettingsFunction(void (*funct)()) {	
	_onRequestSettingsFunction = funct;
}

void MEDIC_RECEIVER::onReceiveHandler(int numBytesReceived) {
	Wire.readBytes( (byte*) &recivedData, numBytesReceived);
	_currentMode = recivedData.targetMode;
}

// Handles what function to call on a requestbased on current mode
void MEDIC_RECEIVER::onRequestHandler() {
	
}

// --------------------------------
void MEDIC_POWER_BOARD_RECEIVER::onReceiveHandler(int numBytesReceived) {
	Wire.readBytes( (byte*) &recivedData, numBytesReceived);
	_currentMode = recivedData.targetMode;
}

void MEDIC_POWER_BOARD_RECEIVER::onRequestHandler() {
	if (_currentMode == STATUS){
		statusStruct = PowerBoardStatusStruct();
		_onRequestStatusFunction();
		Wire.write((byte*) &statusStruct, sizeof(statusStruct));  // send data
	} else if (_currentMode == IDENTIFY) {
		identifyStruct = IdentifyStatusStruct();
		_onRequestIdentifyFunction();
		Wire.write((byte*) &identifyStruct, sizeof(identifyStruct));  // send data
	} else if (_currentMode == SETTINGS) {
		settingStruct = PowerBoardSettingsStruct();
		_onRequestSettingsFunction();
		Wire.write((byte*) &settingStruct, sizeof(settingStruct));  // send data
	} else { // invalid _currentMode
		return;
	}
}

// --------------------------------
void MEDIC_FIRE_CONTROL_RECEIVER::onReceiveHandler(int numBytesReceived) {
	Wire.readBytes( (byte*) &recivedData, numBytesReceived);
	_currentMode = recivedData.targetMode;
}

void MEDIC_FIRE_CONTROL_RECEIVER::onRequestHandler() {
	if (_currentMode == STATUS){
		statusStruct = fireControlStatusStruct();
		_onRequestStatusFunction();
		Wire.write((byte*) &statusStruct, sizeof(statusStruct));  // send data
	} else if (_currentMode == IDENTIFY) {
		identifyStruct = IdentifyStatusStruct();
		_onRequestIdentifyFunction();
		Wire.write((byte*) &identifyStruct, sizeof(identifyStruct));  // send data
	} else if (_currentMode == SETTINGS) {
		settingStruct = fireControlSettingsStruct();
		_onRequestSettingsFunction();
		Wire.write((byte*) &settingStruct, sizeof(settingStruct));  // send data
	} else { // invalid _currentMode
		return;
	}
}

// --------------------------------
void MEDIC_CHRONO_RECEIVER::onReceiveHandler(int numBytesReceived) {
	Wire.readBytes( (byte*) &recivedData, numBytesReceived);
	_currentMode = recivedData.targetMode;
}

void MEDIC_CHRONO_RECEIVER::onRequestHandler() {
	// TODO add heart beat
	if (_currentMode == STATUS){
		statusStruct = chronoStatusStruct();
		_onRequestStatusFunction();
		Wire.write((byte*) &statusStruct, sizeof(statusStruct));  // send data
	} else if (_currentMode == IDENTIFY) {
		identifyStruct = IdentifyStatusStruct();
		_onRequestIdentifyFunction();
		Wire.write((byte*) &identifyStruct, sizeof(identifyStruct));  // send data
	} else if (_currentMode == SETTINGS) {
		settingStruct = chronoSettingsStruct();
		_onRequestSettingsFunction();
		Wire.write((byte*) &settingStruct, sizeof(settingStruct));  // send data
	} else { // invalid _currentMode
		return;
	}
}

