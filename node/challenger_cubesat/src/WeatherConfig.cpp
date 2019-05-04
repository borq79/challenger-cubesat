// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "WeatherConfig.h"
#include <ArduinoJson.h>

bool WeatherConfig::loadConfigurationFile() {
  bool status = true;

  this->debugger = WeatherDebug::getWeatherDebugger();

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
     this->setOwnerName(root["o"]);
     // this->setSSID(root["ssid"]);
     // this->setWifiPassword(root["pwd"]);
     this->setAPName(root["an"]);
     this->setAPPassword(root["apwd"]);
     // this->setThingSpeakAPIKey(root["tk"]);
     // this->setThingSpeakChannelID(root["tc"]);
     // this->setBlynkAPIKey(root["bk"]);
     this->setDebugLevelString(root["dl"]);

   }

   // close the file
   configurationFile.close();
   this->debugger->logln(DEBUG_LEVEL_INFO, "Done parsing Configuration File");

   this->debugger->logln(DEBUG_LEVEL_INFO, "o:" + this->getOwnerName());
   this->debugger->logln(DEBUG_LEVEL_INFO, "an:" + this->getAPName());
   this->debugger->logln(DEBUG_LEVEL_INFO, "apwd:" + this->getAPPassword());
   // this->debugger->logln(DEBUG_LEVEL_INFO, "ssid:" + this->getSSID());
   // this->debugger->logln(DEBUG_LEVEL_INFO, "pwd:" + this->getWifiPassword());
   // this->debugger->logln(DEBUG_LEVEL_INFO, "tk:" + this->getThingSpeakAPIKey());
   // this->debugger->logln(DEBUG_LEVEL_INFO, "tc:" + this->getThingSpeakChannelID());
   // this->debugger->logln(DEBUG_LEVEL_INFO, "bk:" + this->getBlynkAPIKey());
   this->debugger->logln(DEBUG_LEVEL_INFO, "dl:" + this->getDebugLevelName());
  }

  SPIFFS.end();

  return status;
}

bool WeatherConfig::saveConfigurationFile(String &jsonConfig) {
  bool status = true;

  this->debugger = WeatherDebug::getWeatherDebugger();

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

void WeatherConfig::setDebugLevelString(String debugLevel)
{
  if (debugLevel.equalsIgnoreCase("trace")) { this->debugLevel = DEBUG_LEVEL_TRACE; }
  else if (debugLevel.equalsIgnoreCase("info")) { this->debugLevel = DEBUG_LEVEL_INFO; }
  else if (debugLevel.equalsIgnoreCase("error")) { this->debugLevel = DEBUG_LEVEL_ERROR; }
}
