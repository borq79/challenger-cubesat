// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2019 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "CubeSatServer.h"

CubeSatServer::CubeSatServer() : server(CUBESAT_LISTEN_PORT) {
}

void CubeSatServer::init(CubeSatConfig *config, CubeSatDisplayI *cubeSatDisplay) {
  this->debugger       = CubeSatDebug::getCubeSatDebugger();
  this->config         = config;
  this->cubeSatDisplay = cubeSatDisplay;
  server.begin();
}

boolean CubeSatServer::dataFromClientAvailable(String *buf) {
  boolean dataAvailable = false;

  if (client.connected() == false && client.available() == false ) {
    client = server.available();
    if (client && client.connected()) {
      this->debugger->logln(DEBUG_LEVEL_INFO, "Client Connected to Web Server");
    }
  }

  if (client.connected() && client.available()) {
    this->debugger->logln(DEBUG_LEVEL_INFO, "Client Ready.");
    dataAvailable = true;
    *buf = client.readStringUntil('\n'); buf->trim();
  }

  return dataAvailable;
}

void CubeSatServer::sendDataToClient(String &dataToSend){
    client.println(dataToSend);
}
