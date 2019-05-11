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
    interruptTriggered = true;
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

  this->initializeSensors();

  // Setup the A0 pin (GPIO) to Input of the Primary Knob
  pinMode(PRIMARY_KNOB_INPUT_PIN, INPUT);

  this->debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  this->debugger->logln(DEBUG_LEVEL_INFO, "                                Challenger CubeSat ");
  this->debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  this->debugger->logln(DEBUG_LEVEL_INFO, "Starting ...");

  this->wsWifi.enableAP(config);
  this->server.init(&this->config, this->cubeSatDisplay);

  // Read buffer from the client
  readBuffer.reserve(350);
}

void CubeSat::applicationLoop() {
  if (didThePrimarySwitchGetToggled()) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of the primary switch being toggled to: ");
    this->debugger->logln(DEBUG_LEVEL_INFO, String(this->dataSample.primarySwitchState));
  }

  if (didTheSecondarySwitchGetToggled()) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of the secondary switch being toggled to: ");
    this->debugger->logln(DEBUG_LEVEL_INFO, String(this->dataSample.secondarySwitchState));
  }

  if (didTheKnobGetTurned()) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of the knob being turned to: ");
    this->debugger->logln(DEBUG_LEVEL_INFO, String(dataSample.primaryKnobValue));
  }

  if (didServerReceivedDataFromClient()) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of client data on wire: [");
    this->debugger->log(DEBUG_LEVEL_INFO, readBuffer);
    this->debugger->logln(DEBUG_LEVEL_INFO, "]");
  }

  setTheNewStateBasedOnInputs();
  sendDataToClientBasedOnState();
  cubeSatStateChanged = false;
}

void CubeSat::initializeSensors() {
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
      this->lastKnobValue = knobValue;
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

  dataSample.primaryKnobValue = average;
  return average;
}

boolean CubeSat::didThePrimarySwitchGetToggled() {
  boolean switchToggled = false;

  uint8_t currentPrimarySwitchState = pcf8574->digitalRead(P1);

  if (this->dataSample.primarySwitchState != currentPrimarySwitchState) {
    this->dataSample.previousPrimarySwitchState = this->dataSample.primarySwitchState;
    this->dataSample.primarySwitchState         = currentPrimarySwitchState;
    switchToggled                               = true;
  }

  return switchToggled;
}

boolean CubeSat::didTheSecondarySwitchGetToggled() {
  boolean switchToggled = false;
  uint8_t currentSecondarySwitchState = pcf8574->digitalRead(P0);

  if (this->dataSample.secondarySwitchState != currentSecondarySwitchState) {
    this->dataSample.previousSecondarySwitchState = this->dataSample.secondarySwitchState;
    this->dataSample.secondarySwitchState         = currentSecondarySwitchState;
    switchToggled                                 = true;
  }

  return switchToggled;
}

boolean CubeSat::didServerReceivedDataFromClient() {
  return this->server.dataFromClientAvailable(&readBuffer);
}

void CubeSat::setTheNewStateBasedOnInputs() {
  this->debugger->log(DEBUG_LEVEL_TRACE, "Current State: "); this->debugger->logln(DEBUG_LEVEL_TRACE, String(cubeSatState));
  if (cubeSatState == INITIAL_STATE) {
    if (this->dataSample.previousPrimarySwitchState == LOW && this->dataSample.primarySwitchState == HIGH) {
      cubeSatState == SEARCHING_FOR_CLIENT;
      this->cubeSatDisplay->showLookingForSignal();
    }
  }
  this->debugger->log(DEBUG_LEVEL_TRACE, "New State: "); this->debugger->logln(DEBUG_LEVEL_TRACE, String(cubeSatState));
}

void CubeSat::sendDataToClientBasedOnState() {
  if (cubeSatStateChanged) {
    server.sendDataToClient("DORF:" + String(cubeSatState));
  }
}
