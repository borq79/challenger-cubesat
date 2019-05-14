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
#define PRIMARY_KNOB_OFF_THRESHOLD    50
#define SWITCH0_BASE_PIN              0
#define SWITCH1_BASE_PIN              2
#define SENSOR_SAMPLE_RATE            (1000)
#define PRESSURE_CONVERSION           0.000295299830714 /* merc: 0.000295299830714 / psi: 0.000145037738*/

enum CUBESAT_STATE {
  NEEDS_INITIALIZATION    = 0,
  INITIAL_STATE           = 1,
  SEARCHING_FOR_CLIENT    = 2,
  WAITING_FOR_FIRMWARE    = 3,
  DOWNLOADING_FIRMWARE    = 4,
  REBOOT_NEW_FIRMWARE     = 5,
  DISPLAY_METRICS         = 6,
  NEEDS_FAN_ADJUST        = 7
};

enum CUBESAT_SWITCH_POSITION {
  OFF          = 0,
  POSITION_ONE = 1,
  POSITION_TWO = 2
};

static String messages[] = {
  "NO SIGNAL",
  "NEEDS INIT",
  "SEARCHING FOR CLIENT",
  "WAITING FOR FIRMWARE",
  "DOWNLOADING FIRMWARE",
  "REBOOTING",
  "DISPLAY METRICS",
  "NEEDS_FAN_ADJUST"
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


  private:
    CubeSatConfig     config;
    CubeSatData       dataSample;
    BME280            bme280;
    CubeSatWiFi       wsWifi;
    CubeSatDebug     *debugger;
    CubeSatServer     server;
    CubeSatDisplayI  *cubeSatDisplay;
    PCF8574          *pcf8574;
    uint16_t          previousValueKnob0;
    uint16_t          currentValueKnob0;
    CUBESAT_SWITCH_POSITION currentValueSwitch0;
    CUBESAT_SWITCH_POSITION currentValueSwitch1;
    CUBESAT_SWITCH_POSITION previousValueSwitch0;
    CUBESAT_SWITCH_POSITION previousValueSwitch1;
    int               lastInterruptTime  = 0;
    boolean           interruptTriggered = false;
    String            readBuffer;
    CUBESAT_STATE     cubeSatState;
    boolean           cubeSatStateChanged = false;

    void      loadConfigurationFile();
    void      initializeSensors();
    boolean   didTheKnobGetTurned();
    uint16_t  getLatestKnobValue();
    void      readSensors();
    void      setTheNewStateBasedOnInputs();
    void      sendDataToClientBasedOnState();
    boolean   didServerReceivedDataFromClient();
    boolean   didSwitchGetToggled(CUBESAT_SWITCH_POSITION &currentPosition, CUBESAT_SWITCH_POSITION &previousPosition, uint8_t basePin);
    boolean   isInInitialState();
    CUBESAT_STATE stateInitial();
    CUBESAT_STATE stateNeedsInit();
    CUBESAT_STATE stateSearchingForClient();
    CUBESAT_STATE waitingForFirmware();
    CUBESAT_STATE downloadingFirmware();
    CUBESAT_STATE rebootWithNewFirmware();
    CUBESAT_STATE displayMetrics();
    CUBESAT_STATE needsFanAdjustment();
    CUBESAT_SWITCH_POSITION getSwitchPosition(uint8_t basePin);
};


#endif
