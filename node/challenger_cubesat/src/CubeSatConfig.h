// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_CONFIG_H_
#define _CUBESAT_CONFIG_H_

#include "CubeSatCommon.h"
#include "CubeSatDebug.h"

#define CONFIG_FILE_PATH "/cubesat.json"

class CubeSatConfig {
  public:
    CubeSatConfig() {}

    bool loadConfigurationFile();
    bool saveConfigurationFile(String &jsonConfig);

    String getCubeSatName() { return this->cubeSatName; }
    String getAPName() { return this->apName; }
    String getAPPassword() { return this->apPassword; }
    DEBUG_LEVEL getDebugLevel() { return this->debugLevel; }
    String getDebugLevelName() { return CubeSatDebug::getDebugLevelName(this->debugLevel); }

    void setCubeSatName(String cubeSatName){ this->cubeSatName = cubeSatName; }
    void setAPName(String apName){ this->apName = apName; }
    void setAPPassword(String apPassword){ this->apPassword = apPassword; }
    void setDebugLevel(DEBUG_LEVEL debugLevel){ this->debugLevel = debugLevel; }
    void setDebugLevelString(String debugLevel);

  private:
    String cubeSatName;
    String apName;
    String apPassword;
    DEBUG_LEVEL debugLevel;
    CubeSatDebug *debugger;
};


#endif
