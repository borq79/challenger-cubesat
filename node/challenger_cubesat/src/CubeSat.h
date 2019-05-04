// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_H_
#define _CUBESAT_H_

#include "CubeSatCommon.h"
#include "CubeSatWiFi.h"
#include "CubeSatData.h"
#include "CubeSatDebug.h"
#include "CubeSatDisplayI.h"
#include "CubeSatServer.h"


#define BME_COMM_MODE                 I2C_MODE
#define PCF8574_INT_PIN               D6
#define PRIMARY_KNOB_INPUT_PIN        A0
#define PCF8574_INT_DEBOUNCE_TIME     300
#define PRIMARY_KNOB_READ_DELAY       10
#define PRIMARY_KNOB_DELTA_THRESHOLD  20
#define PRIMARY_KNOB_MAX              1024
#define PRIMARY_KNOB_MIN              0
#define SENSOR_SAMPLE_RATE            (1000)
#define PRESSURE_CONVERSION           0.000295299830714 /* merc: 0.000295299830714 / psi: 0.000145037738*/

enum CUBESAT_STATE {
  INITIAL_STATE,
  SEARCHING_FOR_CLIENT
};

class CubeSat {

  public:
    CubeSat();
    void begin(PCF8574 *pcf8574);
    void applicationLoop();
    void timerEvent();
    void pcf8574Interrupt();
    const CubeSatData& getCubeSatData();
    CubeSatConfig& getCubeSatConfig();
    CUBESAT_STATE cubeSatState;

  private:
    CubeSatConfig     config;
    CubeSatData       dataSample;
    BME280            bme280;
    CubeSatWiFi       wsWifi;
    CubeSatDebug     *debugger;
    CubeSatServer     server;
    CubeSatDisplayI  *cubeSatDisplay;
    PCF8574          *pcf8574;
    uint16_t          lastKnobValue;
    int               lastInterruptTime = 0;

    void      loadConfigurationFile();
    void      initializeSensors();
    boolean   didTheKnobGetTurned();
    uint16_t  getLatestKnobValue();
    void      readSensors();
    void      checkIfTheStateShouldChange();
};


#endif
