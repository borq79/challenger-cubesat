// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "CubeSatConfig.h"
#include <ArduinoJson.h>

bool CubeSatConfig::loadConfigurationFile() {
  bool status = true;

  this->debugger = CubeSatDebug::getCubeSatDebugger();

  String configurationFilePath = CONFIG_FILE_PATH;

  this->debugger->logln(DEBUG_LEVEL_INFO, "Loading Configuration File " + configurationFilePath + " ...");

  SPIFFS.begin();
  File configurationFile = SPIFFS.open(configurationFilePath, "r");
  if (!configurationFile) {
    this->debugger->logln(DEBUG_LEVEL_ERROR, String("Failed to open file " + configurationFilePath));
    status = false;
  } else {
   StaticJsonBuffer<512> jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject(configurationFile.readStringUntil('\n'));

   if (!root.success()) {
     this->debugger->logln(DEBUG_LEVEL_ERROR, String("Failed to parse json file " + configurationFilePath));
     status = false;
   } else {
     this->setCubeSatName(root["n"]);
     this->setAPName(root["an"]);
     this->setAPPassword(root["apwd"]);
     this->setDebugLevelString(root["dl"]);

   }

   // close the file
   configurationFile.close();
   this->debugger->logln(DEBUG_LEVEL_INFO, "Done parsing Configuration File");

   this->debugger->logln(DEBUG_LEVEL_INFO, "n:" + this->getCubeSatName());
   this->debugger->logln(DEBUG_LEVEL_INFO, "an:" + this->getAPName());
   this->debugger->logln(DEBUG_LEVEL_INFO, "apwd:" + this->getAPPassword());
   this->debugger->logln(DEBUG_LEVEL_INFO, "dl:" + this->getDebugLevelName());
  }

  SPIFFS.end();

  return status;
}

bool CubeSatConfig::saveConfigurationFile(String &jsonConfig) {
  bool status = true;

  this->debugger = CubeSatDebug::getCubeSatDebugger();

  String configurationFilePath = CONFIG_FILE_PATH;

  this->debugger->logln(DEBUG_LEVEL_INFO, "Saving Configuration File " + configurationFilePath + " ...");

  SPIFFS.begin();
  File configurationFile = SPIFFS.open(configurationFilePath, "w+");
  if (!configurationFile) {
    this->debugger->logln(DEBUG_LEVEL_ERROR, String("Failed to open/create file " + configurationFilePath));
    status = false;
  } else {
    configurationFile.println(jsonConfig);

    // close the file
    configurationFile.close();
    this->debugger->logln(DEBUG_LEVEL_INFO, "Done saving Configuration File");
  }

  SPIFFS.end();

  return status;
}

void CubeSatConfig::setDebugLevelString(String debugLevel)
{
  if (debugLevel.equalsIgnoreCase("trace")) { this->debugLevel = DEBUG_LEVEL_TRACE; }
  else if (debugLevel.equalsIgnoreCase("info")) { this->debugLevel = DEBUG_LEVEL_INFO; }
  else if (debugLevel.equalsIgnoreCase("error")) { this->debugLevel = DEBUG_LEVEL_ERROR; }
}
