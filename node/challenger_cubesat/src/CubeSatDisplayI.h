// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_DISPLAY_I_H_
#define _CUBESAT_DISPLAY_I_H_

#include "CubeSatCommon.h"
#include "CubeSatConfig.h"
#include "CubeSatData.h"


class CubeSatDisplayI {
  public:
    virtual void init(CubeSatConfig &config) = 0;
    virtual void showInitialState() = 0;
    virtual void showNeedsInitialization() = 0;
    virtual void showLookingForSignal(int strength) = 0;
    virtual void showInternalSystemStatus(CubeSatData &cubesatData) = 0;
    virtual void showWaitingForFirmware() = 0;
    virtual void showRebooting() = 0;
    virtual void showNeedsFanAdjustment() = 0;
    virtual void showDownloadingFirmware(uint8_t progress) = 0;
};

#endif
