#ifndef _CUBESAT_DATA_H_
#define _CUBESAT_DATA_H_

struct CubeSatData {
  unsigned long lastRead;
  float tempF;
  float humidity;
  float pressure;
  int primaryKnobValue;
  uint8_t primarySwitchState;
  uint8_t secondarySwitchState;
  uint8_t previousPrimarySwitchState;
  uint8_t previousSecondarySwitchState;
};

#endif
