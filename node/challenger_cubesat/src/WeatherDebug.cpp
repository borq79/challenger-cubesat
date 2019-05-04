// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#include "WeatherDebug.h"

WeatherDebug *WeatherDebug::debugger    = NULL;
bool          WeatherDebug::initialized = false;

WeatherDebug * WeatherDebug::getWeatherDebugger() {
  return debugger;
}

void WeatherDebug::init() {
  if (initialized == false) {
    Serial.println("\n\n\nPress Enter to enter Debug Mode (Will boot in regular mode in " + String(DEBUG_ENABLED_WAIT_TIME / 1000) + " seconds) ...");
    Serial.setTimeout(DEBUG_ENABLED_WAIT_TIME);
    String userInput = Serial.readStringUntil('\n');

    // this->debugEnabled = !timedOutWaitingForDebug;
    bool debugEnabled = false;
    DEBUG_LEVEL debugLevel = DEFAULT_DEBUG_LEVEL;

    if (userInput.length() > 0) {
      debugEnabled = true;
    }

    debugger = new WeatherDebug(debugEnabled, debugLevel);
    initialized = true;

    if (debugEnabled) {
      Serial.println("  *** DEBUG ENABLED - LEVEL " + WeatherDebug::getDebugLevelName(debugLevel) + " ***\n");
    } else {
      Serial.end();
    }
  }
}

WeatherDebug::WeatherDebug() {
  this->debugLevel = DEFAULT_DEBUG_LEVEL;
  this->debugEnabled = false;
}

WeatherDebug::WeatherDebug(bool debugEnabled, DEBUG_LEVEL debugLevel) {
  this->debugEnabled = debugEnabled;
  this->debugLevel = debugLevel;
}

void WeatherDebug::log(DEBUG_LEVEL level, String msg) {
  if (this->debugEnabled && level >= this->debugLevel) {
    Serial.print(msg);
  }
}

void WeatherDebug::logln(DEBUG_LEVEL level, String msg) {
  if (this->debugEnabled && level >= this->debugLevel) {
    Serial.println(msg);
  }
}

void WeatherDebug::setDebugLevel(String &levelName) {
  if (levelName.equalsIgnoreCase("TRACE")) { this->setDebugLevel(DEBUG_LEVEL_TRACE); }
  else if (levelName.equalsIgnoreCase("INFO")) { this->setDebugLevel(DEBUG_LEVEL_INFO); }
  else if (levelName.equalsIgnoreCase("ERROR")) { this->setDebugLevel(DEBUG_LEVEL_ERROR); }
}

String WeatherDebug::getDebugLevelName(DEBUG_LEVEL level) {
  String levelString = "UNKNOWN";

  if (level == DEBUG_LEVEL_TRACE) { levelString = "TRACE"; }
  else if (level == DEBUG_LEVEL_INFO) { levelString = "INFO"; }
  else if (level == DEBUG_LEVEL_ERROR) { levelString = "ERROR"; }

  return levelString;
}
