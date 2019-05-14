#ifndef _CUBESAT_DATA_H_
#define _CUBESAT_DATA_H_

struct CubeSatData {
  unsigned long lastRead;
  float tempF;
  float humidity;
  float pressure;
};

#endif
