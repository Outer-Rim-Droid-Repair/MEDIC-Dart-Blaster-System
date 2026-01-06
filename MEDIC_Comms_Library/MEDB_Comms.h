#ifndef MEDB_Comms_h
#define MEDB_Comms_h

#include "Arduino.h"
#include "Wire.h"

const byte CONTROLLER_ADDRESS = 8;
const byte POWER_DISTRO_BOARD_ADDRESS = 9;
const byte FIRE_CONTROL_BOARD_ADDRESS = 10;
const byte CHRONO_BOARD_ADDRESS = 11;


struct IdentifyStatusStruct {
	char version[16];		// 16
	byte padding[16];		// 16
                            //------
    // total                // 32
};

struct PowerBoardStatusStruct {
    float totalVoltage;     //  4
    float cell_1;           //  4
    float cell_2;           //  4
    float cell_3;           //  4
    float cell_4;           //  4
    float cell_5;           //  4
    float cell_6;           //  4
    int cellCount;          //  2
    byte padding[2];        //  2
                            //------
    // total                // 32
};

struct fireControlStatusStruct {
  int safteyState;        //  2
  int triggerState;       //  2
  int currentFireMode;    //  2
  byte padding[26];       // 26
  //------
  // total                // 32
};

struct chronoStatusStruct {
  float lastFPS;          //  4
  float minFPS;           //  4
  float maxFPS;           //  4
  float rollingAverage;   //  4
  byte padding[16];       // 16
  //------
  // total                // 32
};

enum mode {
	IDENTIFY,	// to check if a device is in the network
	STATUS,		// to get a status form the device
	SETTINGS	// to setup the device
};

struct SendMessageStruct {
	mode targetMode;			//  2
	byte padding[14];       // 14
	//------
	// total                // 16
};



void staticOnReceiveHandler(int numBytesReceived);
void staticOnRequestHandler(int numBytesReceived);

class MEDB {
	public:
		MEDB(int address);
		void begin();
	protected:
		int _address;
		SendMessageStruct recivedData;
	private:
		
};

class MEDB_CONNTROLLER: public MEDB {
  public:
	MEDB_CONNTROLLER();
	bool checkDeviceInSystem(byte address);
	bool SetUnitToMode(int targetAddress, mode selectedMode);
	
	bool requestIdentifyStatus(byte address);
    bool requestPowerStatus();
    bool requestFireControlStatus();
    bool requestChronoStatus();
	
	IdentifyStatusStruct identifyStatus;
	PowerBoardStatusStruct powerBoardStatus;
	fireControlStatusStruct fireControlStatus;
	chronoStatusStruct chronoStatus;
	
  private:
	bool _request(byte address, byte numBytes, mode type);
};

class MEDB_RECEIVER: public MEDB {
	public:
		MEDB_RECEIVER(int address);
		void begin();
	
		void connectOnRequestStatusFunction(void (*funct)());
		void connectOnRequestIdentifyFunction(void (*funct)());
		void connectOnRequestSettingsFunction(void (*funct)());
		byte sendStruct[32];
		
		void onReceiveHandler(int numBytesReceived);
		void onRequestHandler();
	private:
		mode _currentMode;
		
		void(*_onRequestStatusFunction)();
		void(*_onRequestIdentifyFunction)();
		void(*_onRequestSettingsFunction)();
		
		
		
};




#endif