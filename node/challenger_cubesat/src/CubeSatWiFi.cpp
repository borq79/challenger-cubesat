// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "CubeSatWiFi.h"

CubeSatWiFi::CubeSatWiFi() {
}

bool CubeSatWiFi::enableAP(CubeSatConfig &config) {
  this->debugger = CubeSatDebug::getCubeSatDebugger();
  this->debugger->logln(DEBUG_LEVEL_INFO, "Current Status of WiFi Connection: " + getWifiStatus(WiFi.status()));

  this->setAPName(config.getAPName());
  this->setAPPassword(config.getAPPassword());

  this->debugger->logln(DEBUG_LEVEL_INFO, "Disconnecting current WiFi connection ...");
  WiFi.disconnect(false);
  this->debugger->logln(DEBUG_LEVEL_INFO, "Entering AP Mode. AP SSID: " + this->apName + ". Password: " + this->apPassword);
  this->debugger->logln(DEBUG_LEVEL_INFO, "Connect to server via http://192.168.4.1/");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(this->apName.c_str(), this->apPassword.c_str());
}

String CubeSatWiFi::getWifiStatus(wl_status_t status) {
  String statusString = "Disconnected";

  switch (status) {
    case WL_NO_SHIELD: statusString = "No Sheild"; break;
    case WL_IDLE_STATUS: statusString = "Idle"; break;
    case WL_NO_SSID_AVAIL: statusString = "No SSID Available"; break;
    case WL_SCAN_COMPLETED: statusString = "Scan Completed"; break;
    case WL_CONNECT_FAILED: statusString = "Connect Failed"; break;
    case WL_CONNECTED: statusString = "Connected"; break;
    case WL_CONNECTION_LOST: statusString = "Connection Lost"; break;
    case WL_DISCONNECTED: statusString = "Disconnected"; break;
    default: statusString = "Unknown"; break;
  }

  return statusString;
}

void CubeSatWiFi::setAPName(String apName) {
  if (apName.length() == 0) {
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    this->apName = "CubeSat " + macID;
  } else {
    this->apName = apName;
  }
}

void CubeSatWiFi::setAPPassword(String apPassword) {
  if (apPassword.length() >= MIN_PASSWORD_LENGTH) {
    this->apPassword = apPassword;
  } else {
    this->apPassword = "challenger";
  }
}
