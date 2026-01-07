#ifndef CHRONO_h
#define CHRONO_h

#define BACK_SENSOR_PIN 4
#define FRONT_SENSOR_PIN 3

const float MPS2FSP = 3.28084;

enum chronoStates {
  WAITING_FOR_INPUT,
  WAITING_TO_EXIT,
  CALCULATING
};
chronoStates current_state = WAITING_FOR_INPUT;


unsigned long enterTime_us;
unsigned long exitTime_us;

float distance_m = 90. / 1000.;  // mm to m
bool useFPS = true;

float lastMPS;
float maxMPS = -1.;
float minMPS = -1.;
float previousMPS[100];
int currentPreviousMPSIndex = 0;
int currentRollingLength = 10;

unsigned long fireTime_ms = 0;
unsigned long lastFireTime_ms = 0;
unsigned long timeoutDPS_ms = 1000;
float maxDPS = -1.;
int DPSAverageLength = 5;  // this is -1 the number of darts thatneet to be fired to get the average
unsigned long previousTimeBetweenShots[20];
int PreviousTimeBetweenShotsIndex = 0;

float calculateAverageMPS();
float calculateAverageMPS();
float calculateDPS();
void setUnits(bool isFPS);
void setUpRollingAverage(int rollingAmount);
void resetDPSReconrds();
void resetrecords();
void printStats();

void fillStatus();
void fillIdentifier();
void setSettings();
void fillSettings();

#endif