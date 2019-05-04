// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SimpleTimer.h>
#include "CubeSat.h"

void timerEvent();
void pcf8574Interrupt();

CubeSat cubeSat;
SimpleTimer timer;
PCF8574 pcf8574(PCF8574_ADDRESS, PCF8574_INT_PIN, pcf8574Interrupt);

void setup() {
  Serial.begin(COMM_BAUD_RATE);
  delay(3000); // Allow for the USB to connect

  CubeSatDebug::init();
  cubeSat.begin(&pcf8574);
  timer.setInterval(SENSOR_SAMPLE_RATE, timerEvent);

}

// Main Application Loop
void loop() {
  cubeSat.applicationLoop();
  timer.run();
}

void timerEvent() {
  cubeSat.timerEvent();
}

void pcf8574Interrupt() {
  cubeSat.pcf8574Interrupt();
}
