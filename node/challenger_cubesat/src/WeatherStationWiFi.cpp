// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "WeatherStationWiFi.h"

WeatherStationWiFi::WeatherStationWiFi() {
  this->apModeEnabled = false;
}

bool WeatherStationWiFi::connect(WeatherConfig &config, bool apEnabled) {
  this->debugger = WeatherDebug::getWeatherDebugger();
  this->debugger->logln(DEBUG_LEVEL_INFO, "Current Status of WiFi Connection: " + getWifiStatus(WiFi.status()));

  // this->setSSID(config.getSSID());
  // this->setPassword(config.getWifiPassword());
  this->setAPName(config.getAPName());
  this->setAPPassword(config.getAPPassword());

  // if (apEnabled) {
    enableAPMode();
  // } else {
  //   if (WiFi.status() == WL_CONNECTED) {
  //     if (WiFi.setAutoConnect(false))
  //     if (this->wifiSSID.equals(WiFi.SSID())) {
  //       this->debugger->logln(DEBUG_LEVEL_INFO, "The WiFi is already connected to " + this->wifiSSID);
  //     } else {
  //       this->debugger->logln(DEBUG_LEVEL_INFO, "Disconnecting current WiFi connection ...");
  //       WiFi.disconnect(false);
  //       while (WiFi.status() == WL_CONNECTED) {
  //         delay(500);
  //         this->debugger->log(DEBUG_LEVEL_INFO, ".");
  //       }
  //     }
  //   }
  //
  //   if (WiFi.status() != WL_CONNECTED) {
  //     unsigned long startReadingTime = millis();
  //     this->debugger->logln(DEBUG_LEVEL_INFO, String("Connecting to " + this->wifiSSID + " "));
  //
  //     WiFi.mode(WIFI_STA);
  //     WiFi.setAutoConnect(true);
  //     WiFi.begin(this->wifiSSID.c_str(), this->wifiPassword.c_str());
  //     while (WiFi.status() != WL_CONNECTED) {
  //       delay(500);
  //       this->debugger->log(DEBUG_LEVEL_INFO, ".");
  //
  //       if ((millis() - startReadingTime) >= WIFI_CLIENT_CONNECT_TIMEOUT) {
  //         this->debugger->logln(DEBUG_LEVEL_ERROR, "Failed to connect to " + this->wifiSSID + ". Going into AP mode. Reset to retry.");
  //         break;
  //       }
  //     }
  //   }

  //   if (WiFi.status() != WL_CONNECTED) {
  //     enableAPMode();
  //   } else {
  //     IPAddress ip;
  //     char addr[26];
  //
  //     ip = WiFi.localIP();
  //     sprintf(addr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  //     this->debugger->logln(DEBUG_LEVEL_INFO, "\nWiFi connected. IP: " + String(addr));
  //   }
  // }

  return this->apModeEnabled;
}

void WeatherStationWiFi::enableAPMode() {
  // Headless visual AP is enabled
  if (this->debugger->isDebugEnabled() == false) {
    pinMode(AP_ENABLED_LED_PIN, OUTPUT);
    digitalWrite(AP_ENABLED_LED_PIN, LOW);
  }

  this->apModeEnabled = true;
  this->debugger->logln(DEBUG_LEVEL_INFO, "Disconnecting current WiFi connection ...");
  WiFi.disconnect(false);
  this->debugger->logln(DEBUG_LEVEL_INFO, "Entering AP Mode. AP SSID: " + this->apName + ". Password: " + this->apPassword);
  this->debugger->logln(DEBUG_LEVEL_INFO, "Connect to server via http://192.168.4.1/");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(this->apName.c_str(), this->apPassword.c_str());
}

String WeatherStationWiFi::getWifiStatus(wl_status_t status) {
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

void WeatherStationWiFi::setSSID(String ssid) {
  this->wifiSSID = ssid;
}

void WeatherStationWiFi::setPassword(String password) {
  this->wifiPassword = password;
}

void WeatherStationWiFi::setAPName(String apName) {
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

void WeatherStationWiFi::setAPPassword(String apPassword) {
  if (apPassword.length() >= MIN_PASSWORD_LENGTH) {
    this->apPassword = apPassword;
  } else {
    this->apPassword = "challenger";
  }
}

bool WeatherStationWiFi::isAPModeEnabled() {
  return this->apModeEnabled;
}
