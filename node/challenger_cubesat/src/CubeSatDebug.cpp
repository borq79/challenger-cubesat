// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#include "CubeSatDebug.h"

CubeSatDebug *CubeSatDebug::debugger    = NULL;
bool          CubeSatDebug::initialized = false;

CubeSatDebug * CubeSatDebug::getCubeSatDebugger() {
  return debugger;
}

void CubeSatDebug::init() {
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

    debugger = new CubeSatDebug(debugEnabled, debugLevel);
    initialized = true;

    if (debugEnabled) {
      Serial.println("  *** DEBUG ENABLED - LEVEL " + CubeSatDebug::getDebugLevelName(debugLevel) + " ***\n");
    } else {
      Serial.end();
    }
  }
}

CubeSatDebug::CubeSatDebug() {
  this->debugLevel = DEFAULT_DEBUG_LEVEL;
  this->debugEnabled = false;
}

CubeSatDebug::CubeSatDebug(bool debugEnabled, DEBUG_LEVEL debugLevel) {
  this->debugEnabled = debugEnabled;
  this->debugLevel = debugLevel;
}

void CubeSatDebug::log(DEBUG_LEVEL level, String msg) {
  if (this->debugEnabled && level >= this->debugLevel) {
    Serial.print(msg);
  }
}

void CubeSatDebug::logln(DEBUG_LEVEL level, String msg) {
  if (this->debugEnabled && level >= this->debugLevel) {
    Serial.println(msg);
  }
}

void CubeSatDebug::setDebugLevel(String &levelName) {
  if (levelName.equalsIgnoreCase("TRACE")) { this->setDebugLevel(DEBUG_LEVEL_TRACE); }
  else if (levelName.equalsIgnoreCase("INFO")) { this->setDebugLevel(DEBUG_LEVEL_INFO); }
  else if (levelName.equalsIgnoreCase("ERROR")) { this->setDebugLevel(DEBUG_LEVEL_ERROR); }
}

String CubeSatDebug::getDebugLevelName(DEBUG_LEVEL level) {
  String levelString = "UNKNOWN";

  if (level == DEBUG_LEVEL_TRACE) { levelString = "TRACE"; }
  else if (level == DEBUG_LEVEL_INFO) { levelString = "INFO"; }
  else if (level == DEBUG_LEVEL_ERROR) { levelString = "ERROR"; }

  return levelString;
}
