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
  this->pcf8574->pinMode(SWITCH0_BASE_PIN, INPUT);
  this->pcf8574->pinMode(SWITCH0_BASE_PIN + 1, INPUT);
  this->pcf8574->pinMode(SWITCH1_BASE_PIN, INPUT);
  this->pcf8574->pinMode(SWITCH1_BASE_PIN + 1, INPUT);
  // this->pcf8574->pinMode(P4, OUTPUT);
  this->pcf8574->begin();

  pinMode(PRIMARY_KNOB_INPUT_PIN, INPUT); // Setup the A0 pin (GPIO) to Input of the Primary Knob

  this->currentValueSwitch0  = getSwitchPosition(SWITCH0_BASE_PIN);
  this->previousValueSwitch0 = this->currentValueSwitch0;
  this->currentValueSwitch1  = getSwitchPosition(SWITCH1_BASE_PIN);
  this->previousValueSwitch1 = this->currentValueSwitch1;
  this->currentValueKnob0   = getLatestKnobValue();


  config.loadConfigurationFile();
  this->debugger->setDebugLevel(config.getDebugLevel());

  this->cubeSatDisplay->init(this->config);

  this->initializeSensors();

  this->debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  this->debugger->logln(DEBUG_LEVEL_INFO, "                                Challenger CubeSat ");
  this->debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  this->debugger->logln(DEBUG_LEVEL_INFO, "Starting ...");

  this->wsWifi.enableAP(config);
  this->server.init(&this->config, this->cubeSatDisplay);

  // Read buffer from the client
  readBuffer.reserve(350);
  cubeSatStateChanged = true; // Force the initial state to load
  cubeSatState = NEEDS_INITIALIZATION;
}

CUBESAT_SWITCH_POSITION CubeSat::getSwitchPosition(uint8_t basePin) {
  CUBESAT_SWITCH_POSITION switchPosition = OFF;

  boolean positionOneValue = pcf8574->digitalRead(basePin);
  boolean positionTwoValue = pcf8574->digitalRead(basePin + 1);

  if (positionOneValue == HIGH && positionTwoValue == LOW) {
    switchPosition = POSITION_ONE;
  } else if (positionOneValue == LOW && positionTwoValue == HIGH) {
    switchPosition = POSITION_TWO;
  }

  return switchPosition;
}

void CubeSat::applicationLoop() {
  if (didSwitchGetToggled(this->currentValueSwitch0, this->previousValueSwitch0, SWITCH0_BASE_PIN)) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of the primary switch being toggled to: ");
    this->debugger->logln(DEBUG_LEVEL_INFO, String(this->currentValueSwitch0));
    cubeSatStateChanged = true;
  }

  if (didSwitchGetToggled(this->currentValueSwitch1, this->previousValueSwitch1, SWITCH1_BASE_PIN)) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of the secondary switch being toggled to: ");
    this->debugger->logln(DEBUG_LEVEL_INFO, String(this->currentValueSwitch1));
    cubeSatStateChanged = true;
  }

  if (didTheKnobGetTurned()) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of the knob being turned to: ");
    this->debugger->logln(DEBUG_LEVEL_INFO, String(this->currentValueKnob0));
    cubeSatStateChanged = true;
  }

  if (didServerReceivedDataFromClient()) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Application loop notified of client data on wire: [");
    this->debugger->log(DEBUG_LEVEL_INFO, readBuffer);
    this->debugger->logln(DEBUG_LEVEL_INFO, "]");
    cubeSatStateChanged = true;
  }

  setTheNewStateBasedOnInputs();
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

  if (cubeSatState == DISPLAY_METRICS) {
    this->debugger->log(DEBUG_LEVEL_INFO, "Sensor Values\nT: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.tempF, 2));
    this->debugger->log(DEBUG_LEVEL_INFO, "\tH: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.humidity, 2));
    this->debugger->log(DEBUG_LEVEL_INFO, "\tP: "); this->debugger->log(DEBUG_LEVEL_INFO, String(dataSample.pressure, 2));
    this->debugger->log(DEBUG_LEVEL_INFO, "\n");

    cubeSatDisplay->showInternalSystemStatus(dataSample);
  }
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

  if (abs(this->previousValueKnob0 - knobValue) > PRIMARY_KNOB_DELTA_THRESHOLD) {
      wasAChange = true;
      this->previousValueKnob0 = knobValue;
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

  currentValueKnob0 = average;
  return average;
}

boolean CubeSat::didSwitchGetToggled(CUBESAT_SWITCH_POSITION &currentPosition, CUBESAT_SWITCH_POSITION &previousPosition, uint8_t basePin) {
  boolean switchToggled = false;

  CUBESAT_SWITCH_POSITION position = getSwitchPosition(basePin);

  if (currentPosition != position) {
    previousPosition = currentPosition;
    currentPosition  = position;
    switchToggled    = true;
  }

  return switchToggled;
}

boolean CubeSat::didServerReceivedDataFromClient() {
  return this->server.dataFromClientAvailable(&readBuffer);
}

void CubeSat::setTheNewStateBasedOnInputs() {
  // Only handle the state if it actually changed
  if (cubeSatStateChanged == false) { return; }

  this->debugger->log(DEBUG_LEVEL_INFO, "Current State: "); this->debugger->logln(DEBUG_LEVEL_INFO, String(cubeSatState));

  // TODO Make these use the command pattern
  switch(cubeSatState) {
    case NEEDS_INITIALIZATION: cubeSatState = stateNeedsInit(); break;
    case INITIAL_STATE:        cubeSatState = stateInitial(); break;
    case SEARCHING_FOR_CLIENT: cubeSatState = stateSearchingForClient(); break;
    case WAITING_FOR_FIRMWARE: cubeSatState = waitingForFirmware(); break;
    case DOWNLOADING_FIRMWARE: cubeSatState = downloadingFirmware(); break;
    case REBOOT_NEW_FIRMWARE:  cubeSatState = rebootWithNewFirmware(); break;
    case DISPLAY_METRICS:      cubeSatState = displayMetrics(); break;
    case NEEDS_FAN_ADJUST:     cubeSatState = needsFanAdjustment(); break;
    default: break;
  }

  // if (newState != cubeSatState) { cubeSatStateChanged = true; }

  this->debugger->log(DEBUG_LEVEL_INFO, "New State: "); this->debugger->logln(DEBUG_LEVEL_INFO, String(cubeSatState));
}

CUBESAT_STATE CubeSat::stateInitial() {
  CUBESAT_STATE newState = INITIAL_STATE; // stay the same by default

  if (this->currentValueSwitch0 == POSITION_ONE && this->currentValueSwitch1 == OFF){
    newState = SEARCHING_FOR_CLIENT;
    this->cubeSatDisplay->showLookingForSignal(this->currentValueKnob0 / 10);
    // server.sendDataToClient(messages[newState]);
  }

  return newState;
}

CUBESAT_STATE CubeSat::stateNeedsInit() {
  CUBESAT_STATE newState = NEEDS_INITIALIZATION; // Stay the same by default

  if (isInInitialState() == true) {
    newState = INITIAL_STATE;
    this->cubeSatDisplay->showInitialState();
  } else {
    this->cubeSatDisplay->showNeedsInitialization();
  }

  // server.sendDataToClient(messages[newState]);
  return newState;
}

CUBESAT_STATE CubeSat::waitingForFirmware() {
  CUBESAT_STATE newState = WAITING_FOR_FIRMWARE; // Stay the same by default

  if (readBuffer.startsWith("SENDINGFW")) {
    newState = DOWNLOADING_FIRMWARE;
    String messageToSendToClient = "STARTFW";
    server.sendDataToClient(messageToSendToClient);
    this->cubeSatDisplay->showDownloadingFirmware(0);
  }

  return newState;
}

CUBESAT_STATE CubeSat::downloadingFirmware() {
  uint8_t progress = 10;
  while(progress <= 100) {
    String messageToSendToClient = "FW:" + String(progress);
    server.sendDataToClient(messageToSendToClient);
    this->cubeSatDisplay->showDownloadingFirmware(progress);
    progress += 10;
    delay(500);
  }

  return REBOOT_NEW_FIRMWARE;
}

CUBESAT_STATE CubeSat::rebootWithNewFirmware() {
  CUBESAT_STATE newState = REBOOT_NEW_FIRMWARE; // Stay the same by default

  String messageToSendToClient = "REBOOTING";
  server.sendDataToClient(messageToSendToClient);
  this->cubeSatDisplay->showRebooting();
  delay(5000);

  return DISPLAY_METRICS;
}

CUBESAT_STATE CubeSat::displayMetrics() {
  CUBESAT_STATE newState = DISPLAY_METRICS; // Stay the same by default
  if (readBuffer.startsWith("TOO HOT")) {
    newState = NEEDS_FAN_ADJUST;
  } else {
    readSensors();
  }

  return newState;
}

CUBESAT_STATE CubeSat::needsFanAdjustment() {
  CUBESAT_STATE newState = NEEDS_FAN_ADJUST; // Stay the same by default

  if (this->currentValueSwitch0 == POSITION_TWO &&
      this->currentValueSwitch0 == POSITION_ONE) {
    newState = DISPLAY_METRICS;
    readSensors();
  } else {
    this->cubeSatDisplay->showNeedsFanAdjustment();
    String messageToSendToClient = "TOO HOT";
    server.sendDataToClient(messageToSendToClient);
  }

  return newState;
}

CUBESAT_STATE CubeSat::stateSearchingForClient() {
  CUBESAT_STATE newState = SEARCHING_FOR_CLIENT; // Stay the same by default

  if (this->currentValueSwitch0 == POSITION_ONE &&
      this->currentValueKnob0 > 700 &&
      this->currentValueKnob0 < 750) {
    newState = WAITING_FOR_FIRMWARE;
    this->cubeSatDisplay->showWaitingForFirmware();
    server.sendDataToClient(messages[newState]);
  } else {
    this->cubeSatDisplay->showLookingForSignal(this->currentValueKnob0 / 10);
  }

  return newState;
}


boolean CubeSat::isInInitialState() {
  return (this->currentValueSwitch0 == OFF &&
      this->currentValueSwitch1 == OFF &&
      this->currentValueKnob0 < PRIMARY_KNOB_OFF_THRESHOLD );
}
