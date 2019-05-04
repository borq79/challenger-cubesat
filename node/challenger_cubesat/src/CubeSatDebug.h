// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_DEBUG_H_
#define _CUBESAT_DEBUG_H_

#include "CubeSatCommon.h"

#define DEBUG_ENABLED_WAIT_TIME  3000
#define DEFAULT_DEBUG_LEVEL      DEBUG_LEVEL_TRACE

enum DEBUG_LEVEL {
  DEBUG_LEVEL_TRACE = 0,
  DEBUG_LEVEL_INFO  = 50,
  DEBUG_LEVEL_ERROR = 100
};

class CubeSatDebug
{
  public:
    static void init();
    static CubeSatDebug *getCubeSatDebugger();

    void log(DEBUG_LEVEL level, String msg);
    void logln(DEBUG_LEVEL level, String msg);
    bool isDebugEnabled() { return debugEnabled; }
    void setDebugLevel(DEBUG_LEVEL newLevel) { this->debugLevel = newLevel; }
    void setDebugLevel(String &levelName);
    static String getDebugLevelName(DEBUG_LEVEL level);

  private:
    CubeSatDebug();
    CubeSatDebug(bool debugEnabled, DEBUG_LEVEL debugLevel);

    bool                debugEnabled;
    DEBUG_LEVEL         debugLevel;
    static CubeSatDebug *debugger;
    static bool         initialized;
};


#endif
