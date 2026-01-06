#define BACK_SENSOR_PIN 4
#define FRONT_SENSOR_PIN 3

const float MPS2FSP = 3.28084;

enum states {
  WAITING_FOR_INPUT,
  WAITING_TO_EXIT,
  CALCULATING
};
states current_state = WAITING_FOR_INPUT;


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
unsigned long timeoutDPS_ms = 10000;
float maxDPS = -1.;
const int DPSAverageLength = 5;  // this is -1 the number of darts thatneet to be fired to get the average
unsigned long previousTimeBetweenShots[DPSAverageLength];
int PreviousTimeBetweenShotsIndex = 0;


void setup() {
  //start serial connection
  Serial.begin(9600);

  pinMode(BACK_SENSOR_PIN, INPUT);
  pinMode(FRONT_SENSOR_PIN, INPUT);
  digitalWrite(BACK_SENSOR_PIN, HIGH); // turn on the pullup
  digitalWrite(FRONT_SENSOR_PIN, HIGH); // turn on the pullup

  pinMode(13, OUTPUT);
}

void loop() {
  static int reading = 0; 

  switch (current_state) {
    case WAITING_FOR_INPUT:
      reading = digitalRead(BACK_SENSOR_PIN);
      // Serial.println(reading);
      if (reading == LOW) {  // beam broken
        enterTime_us = micros();
        fireTime_ms = millis();
        current_state = WAITING_TO_EXIT;
      }
      break;
    case WAITING_TO_EXIT:
      reading = digitalRead(FRONT_SENSOR_PIN);
      if (reading == LOW) {
        exitTime_us = micros();
        current_state = CALCULATING;
      } else if (millis() - fireTime_ms > 250) { //time out after 0.25 seconds
        current_state = WAITING_FOR_INPUT;
      }
      break;
    case CALCULATING:
      float airTime_us = (exitTime_us-enterTime_us);
      float airTime_s = airTime_us / 1000000.;
      lastMPS = distance_m / airTime_s;

      if (lastMPS > maxMPS or maxMPS < 0) {
        maxMPS = lastMPS;
      }
      if (lastMPS < minMPS or minMPS < 0) {
        minMPS = lastMPS;
      }

      if (currentPreviousMPSIndex >= currentRollingLength) {
        currentPreviousMPSIndex = 0;
      }
      previousMPS[currentPreviousMPSIndex] = lastMPS;
      currentPreviousMPSIndex++;

      if (lastFireTime_ms != 0) {
        if ((fireTime_ms - lastFireTime_ms) > timeoutDPS_ms) {  // if too long between shots reset
          calculateDPS();
          resetDPSReconrds();
        } else {
                    Serial.println(PreviousTimeBetweenShotsIndex);
          previousTimeBetweenShots[PreviousTimeBetweenShotsIndex] = fireTime_ms - lastFireTime_ms;
          PreviousTimeBetweenShotsIndex++;

          if (PreviousTimeBetweenShotsIndex >= DPSAverageLength) {
            calculateDPS();
            PreviousTimeBetweenShotsIndex = 0;
          }
        }
      }
      lastFireTime_ms = fireTime_ms;

      printStats();

      current_state = WAITING_FOR_INPUT;
      break;
  } 

}

float calculateAverageMPS() {
  float total = 0.;
  int i;
  for (i=0; i<currentRollingLength; i++) {
    if (previousMPS[i] == 0) {
      break;
    }
    total += previousMPS[i];
  }
  return total / (float) i; 

}

float calculateDPS() {
  float total = 0.;
  int i;
  for (i=0; i<DPSAverageLength; i++) {
    if (previousTimeBetweenShots[i] == 0) {
      break;
    }
    total += previousTimeBetweenShots[i];
  }
  if (i == 0) {
    return -1;
  }
  float average =  total / (float) i;  // average time between shots
  float dps = 1000. / average;  // 1 second / average time between shots

  if (dps > maxDPS or maxDPS < 0) {
    maxDPS = dps;
  } 
  return dps;
}

void setUnits(bool isFPS) {
  useFPS = isFPS;
}

void setUpRollingAverage(int rollingAmount) {
  currentRollingLength = rollingAmount;
}

void resetDPSReconrds() {
  Serial.println("reset");
  fireTime_ms = 0;
  lastFireTime_ms = 0;
  memset(previousTimeBetweenShots, 0, sizeof(previousTimeBetweenShots));
  PreviousTimeBetweenShotsIndex = 0;
}

void resetrecords() {
  maxMPS = -1;
  minMPS = -1;
  memset(previousMPS, 0, sizeof(previousMPS));
  currentPreviousMPSIndex = 0;
  maxDPS = -1.;
}

void printStats() {
  float fps = lastMPS * MPS2FSP;
  float average = calculateAverageMPS();
  average = average * MPS2FSP;
  Serial.print("fps: ");
  Serial.print(fps);
  Serial.print(" min: ");
  Serial.print(minMPS*MPS2FSP);
  Serial.print(" max: ");
  Serial.print(maxMPS*MPS2FSP);
  Serial.print(" average: ");
  Serial.print(average);
  Serial.print(" max DPS: ");
  Serial.print(maxDPS);

  Serial.println("");
}
