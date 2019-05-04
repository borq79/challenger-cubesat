// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_STATION_WIFI_H_
#define _CUBESAT_STATION_WIFI_H_

#include "CubeSatCommon.h"
#include "CubeSatDebug.h"
#include "CubeSatConfig.h"

#define MIN_PASSWORD_LENGTH         11

class CubeSatWiFi {

private:
  String apName;
  String apPassword;
  CubeSatDebug *debugger;

public:
  CubeSatWiFi();
  bool enableAP(CubeSatConfig &config, bool apEnabled);
  String getWifiStatus(wl_status_t status);
  void setAPName(String apName);
  void setAPPassword(String apPassword);
};

#endif
