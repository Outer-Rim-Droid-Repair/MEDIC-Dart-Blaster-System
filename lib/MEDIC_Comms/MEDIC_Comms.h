#ifndef MEDIC_Comms_h
#define MEDIC_Comms_h

#include "Arduino.h"
#include "Wire.h"

const byte CONTROLLER_ADDRESS = 8;
const byte POWER_DISTRO_BOARD_ADDRESS = 9;
const byte FIRE_CONTROL_BOARD_ADDRESS = 10;
const byte CHRONO_BOARD_ADDRESS = 11;


struct IdentifyStatusStruct {
	char version[16];		// 16
	bool heartbeat;			//  1
	byte padding[16];		// 15
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
	bool heartbeat;			//  1
    byte padding[1];        //  1
                            //------
    // total                // 32
};

struct PowerBoardSettingsStruct {
	float warningLevel;		//  4
	bool heartbeat;			//  1
    byte padding[3];        //  3
							//------
    // total                //  8
};

struct fireControlStatusStruct {
	int safteyState;        //  2
	int triggerState;       //  2
	int currentFireMode;    //  2
	bool heartbeat;			//  1
	byte padding[25];       // 25
	//------
	// total                // 32
};

struct fireControlSettingsStruct {  // TODO figure out how to handdle changin possitional settings
	bool heartbeat;			//  1
	byte padding[7];      	//  7
  	//------
  	// total                // 8
};

struct chronoStatusStruct {
	float lastFPS;          //  4
	float minFPS;           //  4
	float maxFPS;           //  4
	float maxDPS;		    //  4
	float lastDPS;			//  4
	bool heartbeat;			//  1
	byte padding[11];       // 11
	//------
	// total                // 32
};

struct chronoSettingsStruct {
	int MPSRollingLength;   //  2  // TODO investigate condensing multiple fools down into a byte
	int DPSAverageLength;   //  2
	int timeoutDPS_ms;		//  2
	bool useFPS;			//  1
	bool doReset;			//  1
	bool heartbeat;			//  1
	byte padding[7];        //  7
  	//------
  	// total                // 16
};

enum mode {
	IDENTIFY,	// to check if a device is in the network
	STATUS,		// to get a status form the device
	SETTINGS	// to setup the device
};

struct SendMessageStruct {
	mode targetMode;		//  2
	byte padding[14];       // 14
	//------
	// total                // 16
};



void staticOnReceiveHandler(int numBytesReceived);
void staticOnRequestHandler(int numBytesReceived);

class MEDIC {
	public:
		MEDIC(int address);
		void begin();
	protected:
		int _address;
		SendMessageStruct recivedData;
	private:
		
};

class MEDIC_CONNTROLLER: public MEDIC {
  public:
	MEDIC_CONNTROLLER();
	bool checkDeviceInSystem(byte address);
	bool SetUnitToMode(int targetAddress, mode selectedMode);
	
	void requestIdentifyStatus(byte address);
    void requestPowerStatus();
    void requestFireControlStatus();
    void requestChronoStatus();

	void requestPowerSettings();
    void requestFireControlSettings();
    void requestChronoSettings();

	void setPowerSettings();
    void setFireControlSettings();
    void setChronoSettings();
	
	IdentifyStatusStruct identifyStatus;

	PowerBoardStatusStruct powerBoardStatus;
	fireControlStatusStruct fireControlStatus;
	chronoStatusStruct chronoStatus;

	PowerBoardSettingsStruct powerBoardSettings;
	fireControlSettingsStruct fireControlSettings;
	chronoSettingsStruct chronoSettings;
	
  private:
	bool _request(byte address, byte numBytes, mode type);
};

class MEDIC_RECEIVER: public MEDIC {
	public:
		MEDIC_RECEIVER(int address);
		void begin();
	
		void connectOnRequestStatusFunction(void (*funct)());
		void connectOnRequestIdentifyFunction(void (*funct)());
		void connectOnRequestSettingsFunction(void (*funct)());
		
		virtual void onReceiveHandler(int numBytesReceived);
		virtual void onRequestHandler();

		IdentifyStatusStruct identifyStruct;
	protected:
		mode _currentMode;
		
		void(*_onRequestStatusFunction)();
		void(*_onRequestIdentifyFunction)();
		void(*_onRequestSettingsFunction)();
};

class MEDIC_POWER_BOARD_RECEIVER: public MEDIC_RECEIVER {
	public:
		MEDIC_POWER_BOARD_RECEIVER(): MEDIC_RECEIVER(POWER_DISTRO_BOARD_ADDRESS) {}
		void onReceiveHandler(int numBytesReceived) override;
		void onRequestHandler() override;
		PowerBoardSettingsStruct settingStruct;
		PowerBoardStatusStruct statusStruct;
};

class MEDIC_FIRE_CONTROL_RECEIVER: public MEDIC_RECEIVER {
	public:
		MEDIC_FIRE_CONTROL_RECEIVER(): MEDIC_RECEIVER(FIRE_CONTROL_BOARD_ADDRESS) {}
		void onReceiveHandler(int numBytesReceived) override;
		void onRequestHandler() override;
		fireControlSettingsStruct settingStruct;
		fireControlStatusStruct statusStruct;
};

class MEDIC_CHRONO_RECEIVER: public MEDIC_RECEIVER {
	public:
		MEDIC_CHRONO_RECEIVER(): MEDIC_RECEIVER(CHRONO_BOARD_ADDRESS) {}
		void onReceiveHandler(int numBytesReceived) override;
		void onRequestHandler() override;
		chronoSettingsStruct settingStruct;
		chronoStatusStruct statusStruct;
};

#endif