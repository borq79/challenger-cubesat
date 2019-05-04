// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_OLED_DISPLAY_H_
#define _CUBESAT_OLED_DISPLAY_H_

#include "CubeSatCommon.h"
#include "CubeSatConfig.h"
#include "CubeSatDisplayI.h"
#include "CubeSatDebug.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"



#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

class CubeSatSSD1306Display : public CubeSatDisplayI {

private:
  String            cubeSatName;
  CubeSatDebug     *debugger;
  Adafruit_SSD1306 *display;

  void initDisplay();


public:
  CubeSatSSD1306Display();
  void init(CubeSatConfig &config);
  void showInternalSystemStatus(CubeSatData &cubeSatData);
  void showInitialState();
  void showLookingForSignal();
  void showAcceptFirmwareState();
};

#endif
