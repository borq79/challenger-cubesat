// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _WEATHER_STATION_WIFI_H_
#define _WEATHER_STATION_WIFI_H_

#include "WeatherStationCommon.h"
#include "WeatherDebug.h"
#include "WeatherConfig.h"

#define WIFI_CLIENT_CONNECT_TIMEOUT 60000
#define AP_MODE_TOGGLE_PIN          D7
#define AP_ENABLED_LED_PIN          D4
#define MIN_PASSWORD_LENGTH         11

class WeatherStationWiFi {

private:
  String wifiPassword;
  String wifiSSID;
  String apName;
  String apPassword;
  bool apModeEnabled;
  WeatherDebug *debugger;

public:
  WeatherStationWiFi();
  bool connect(WeatherConfig &config, bool apEnabled);
  String getWifiStatus(wl_status_t status);
  void setSSID(String ssid);
  void setPassword(String password);
  void setAPName(String apName);
  void setAPPassword(String apPassword);
  void enableAPMode();
  bool isAPModeEnabled();
};

#endif
