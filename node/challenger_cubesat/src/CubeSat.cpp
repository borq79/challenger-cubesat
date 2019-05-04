// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "CubeSat.h"
#include "CubeSatSSD1306Display.h"

CubeSat::CubeSat() {
  cubeSatState = INITIAL_STATE;
  this->cubeSatDisplay = new CubeSatSSD1306Display();
}

void CubeSat::timerEvent() {
  this->readSensors();
}

// Called when the button is pressed
void CubeSat::pcf8574Interrupt() {
  if ((millis() - lastInterruptTime) > PCF8574_INT_DEBOUNCE_TIME) {
    lastInterruptTime = millis();
    this->debugger->logln(DEBUG_LEVEL_INFO, "PCF8574 Interrupt Fired");

    // Record the curent values as the previous values
    uint8_t currentPrimarySwitchState   = pcf8574->digitalRead(P1);
    uint8_t currentSecondarySwitchState = pcf8574->digitalRead(P0);

    if (this->dataSample.primarySwitchState != currentPrimarySwitchState) {
      this->dataSample.previousPrimarySwitchState = this->dataSample.primarySwitchState;
      this->dataSample.primarySwitchState         = currentPrimarySwitchState;
    }

    if (this->dataSample.secondarySwitchState != currentSecondarySwitchState) {
      this->dataSample.previousSecondarySwitchState = this->dataSample.secondarySwitchState;
      this->dataSample.secondarySwitchState         = currentSecondarySwitchState;
    }

    this->checkIfTheStateShouldChange();
  }
}

void CubeSat::begin(PCF8574 *pcf8574) {
  this->debugger = CubeSatDebug::getCubeSatDebugger();
  this->pcf8574  = pcf8574;
  this->pcf8574->pinMode(P0, INPUT);
  this->pcf8574->pinMode(P1, INPUT);
  this->pcf8574->pinMode(P4, OUTPUT);
  this->pcf8574->begin();

  config.loadConfigurationFile();
  this->debugger->setDebugLevel(config.getDebugLevel());

  this->cubeSatDisplay->init(this->config);

  this->debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  this->debugger->logln(DEBUG_LEVEL_INFO, "                                Challenger CubeSat ");
  this->debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  this->debugger->logln(DEBUG_LEVEL_INFO, "Starting ...");

  this->wsWifi.enableAP(config);
  this->server.init(&this->config, this->cubeSatDisplay);

  this->initializeSensors();
}

void CubeSat::applicationLoop() {
  //this->server.applicationLoop();
  // showLookingForSignal
  if (cubeSatState == INITIAL_STATE) {
    // this->cubeSatDisplay->showInitialState();
  } else if (cubeSatState == SEARCHING_FOR_CLIENT) {
    // this->cubeSatDisplay->showLookingForSignal();
  }

}

void CubeSat::initializeSensors() {
  // Setup the A0 pin (GPIO) to Input of the Primary Knob
  pinMode(PRIMARY_KNOB_INPUT_PIN, INPUT);

  // Communication Mode Intialization
  bme280.settings.commInterface = BME_COMM_MODE;
  bme280.settings.I2CAddress = BME_ADDRESS;

  bme280.settings.runMode         = 3;
  bme280.settings.tStandby        = 0;
  bme280.settings.filter          = 0;
  bme280.settings.tempOverSample  = 1;
  bme280.settings.pressOverSample = 1;
  bme280.settings.humidOverSample = 1;

  // Give the BME280 enough time to start up (takes 2ms)
  delay(10);
  bme280.begin();
}

void CubeSat::readSensors() {
  dataSample.lastRead         = millis();
  dataSample.tempF            = bme280.readTempF();
  dataSample.humidity         = bme280.readFloatHumidity();
  dataSample.pressure         = bme280.readFloatPressure() * PRESSURE_CONVERSION;
  dataSample.primaryKnobValue = this->getLatestKnobValue();

  this->debugger->log(DEBUG_LEVEL_INFO, "Sensor Values\nT: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.tempF, 2));
  this->debugger->log(DEBUG_LEVEL_INFO, "\tH: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.humidity, 2));
  this->debugger->log(DEBUG_LEVEL_INFO, "\tP: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.pressure, 2));
  this->debugger->log(DEBUG_LEVEL_INFO, "\tK: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.primaryKnobValue));
  this->debugger->log(DEBUG_LEVEL_INFO, "\n");

  //cubeSatDisplay->showInternalSystemStatus(dataSample);
}

const CubeSatData& CubeSat::getCubeSatData() {
  return this->dataSample;
}

CubeSatConfig& CubeSat::getCubeSatConfig() {
  return this->config;
}


boolean CubeSat::didTheKnobGetTurned() {
  boolean wasAChange = false;
  uint16_t knobValue = getLatestKnobValue();

  if (abs(this->lastKnobValue - knobValue) > PRIMARY_KNOB_DELTA_THRESHOLD) {
      wasAChange = true;
  }

  return wasAChange;
}

uint16_t CubeSat::getLatestKnobValue() {
  // Read the dial 3 times and find out what the average is to get rid of any outliers
  uint16_t rawValue1 = analogRead(PRIMARY_KNOB_INPUT_PIN);
  //Serial.print(rawValue1); Serial.print("\t");
  delay(PRIMARY_KNOB_READ_DELAY);
  uint16_t rawValue2 = analogRead(PRIMARY_KNOB_INPUT_PIN);
  //Serial.print(rawValue2); Serial.print("\t");
  delay(PRIMARY_KNOB_READ_DELAY);
  uint16_t rawValue3 = analogRead(PRIMARY_KNOB_INPUT_PIN);
  //Serial.print(rawValue3); Serial.print("\t");

  uint16_t average = (rawValue1 + rawValue2 + rawValue3) / 3;
  //Serial.println(average);

  return average;
}

void CubeSat::checkIfTheStateShouldChange() {
  this->debugger->log(DEBUG_LEVEL_TRACE, "Current State: "); this->debugger->logln(DEBUG_LEVEL_TRACE, String(cubeSatState));
  if (cubeSatState == INITIAL_STATE) {
    if (this->dataSample.previousPrimarySwitchState == LOW && this->dataSample.primarySwitchState == HIGH) {
      cubeSatState == SEARCHING_FOR_CLIENT;
      this->cubeSatDisplay->showLookingForSignal();
    }
  }
  this->debugger->log(DEBUG_LEVEL_TRACE, "New State: "); this->debugger->logln(DEBUG_LEVEL_TRACE, String(cubeSatState));
}
