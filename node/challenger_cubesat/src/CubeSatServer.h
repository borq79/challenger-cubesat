// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================

#ifndef _CUBESAT_SERVER_H_
#define _CUBESAT_SERVER_H_

#include "CubeSatCommon.h"
#include "CubeSatDebug.h"
#include "CubeSatConfig.h"
#include "CubeSatDisplayI.h"

#define CUBESAT_LISTEN_PORT 80
#define MAX_SESSION_LENGTH 120000


class CubeSatServer {

private:
    WiFiServer            server;
    CubeSatDebug          *debugger;
    CubeSatConfig         *config;
    CubeSatDisplayI       *cubeSatDisplay;
    WiFiClient            client;

public:
    CubeSatServer();
    void init(CubeSatConfig *config, CubeSatDisplayI *cubeSatDisplay);
    void sendDataToClient(String &dataToSend);
    boolean dataFromClientAvailable(String *readBuffer);
};


#endif
