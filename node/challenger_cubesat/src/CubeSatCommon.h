// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESATSTATION_COMMON_H_
#define _CUBESATSTATION_COMMON_H_

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SparkFunBME280.h>
#include "PCF8574.h"

#define COMM_BAUD_RATE                115200
#define BME_ADDRESS                   0x76
#define PCF8574_ADDRESS               0x20
#define ADAFRUIT_DISPLAY_ADDRESS      0x3C

#endif
