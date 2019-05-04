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

void CubeSatServer::applicationLoop() {

  WiFiClient client = server.available();
  if (client) {
    this->debugger->logln(DEBUG_LEVEL_INFO, "Client Connected to Web Server");

    long maxWaitTime = MAX_SESSION_LENGTH;
    long startTime = millis();

    while (client.connected()) {
      this->debugger->logln(DEBUG_LEVEL_TRACE, "Waiting for client to be available ...");
      while(!client.available() && ((millis() - startTime) < maxWaitTime)){
        delay(1);
      }
      this->debugger->logln(DEBUG_LEVEL_TRACE, "Client Ready.");

      String readBuffer; readBuffer.reserve(350);
      String requestBody;
      int contentLength = 0;
      String requestPath;

      while (client.connected() && ((millis() - startTime) < maxWaitTime)) {

        if (requestBody.length() < contentLength && client.available() <= 0) {
          delay(1);
          continue;
        }

        this->debugger->logln(DEBUG_LEVEL_TRACE, "Server State: ");
        readBuffer = client.readStringUntil('\n'); readBuffer.trim();

        this->debugger->logln(DEBUG_LEVEL_TRACE, " SENT FROM CLIENT: [" + readBuffer + "]");
        client.print(this->config->getCubeSatName()); client.print(" Bootloader\r\n");
      }

      break; // End connection
    }

    // give the web browser time to receive the data
    delay(10);

    client.stop();
    this->debugger->logln(DEBUG_LEVEL_INFO, "Client Disonnected");
  }
}

// void CubeSatServer::sendAPIJsonResponse(WiFiClient &client, String &response) {
//   client.print("HTTP/1.1 200 OK\r\n");
//   client.print("Content-Type: application/json\r\n");
//   client.print("Connection: close\r\n");  // the connection will be closed after completion of the response
//   client.print("Content-Length: " + String(response.length()) + "\r\n");
//   client.print("\r\n");
//   client.print(response);
//   client.print("\r\n\r\n");
// }

// void CubeSatServer::sendAPIAPStatus(WiFiClient &client) {
//   StaticJsonBuffer<128> jsonBuffer;
//   String response;
//
//   WeatherConfig c = weatherStation->getWeatherConfig();
//   JsonObject& root = jsonBuffer.createObject();
//   root["oname"] = c.getOwnerName();
//   root["mode"] = String(this->isAPModeEnabled ? "ap" : "station");
//   root.printTo(response);
//
//   sendAPIJsonResponse(client, response);
// }

// void CubeSatServer::sendAPIWeatherData(WiFiClient &client) {
//   StaticJsonBuffer<256> jsonBuffer;
//   String response;
//   WeatherData d = weatherStation->getWeatherData();
//   JsonObject& root = jsonBuffer.createObject();
//   root["u"] = d.lastRead;
//   root["t"] = d.tempF;
//   root["p"] = d.pressure;
//   root["h"] = d.humidity;
//   root["b"] = d.brightness;
//   root.printTo(response);
//
//   sendAPIJsonResponse(client, response);
// }

// void CubeSatServer::sendAPIConfiguration(WiFiClient &client) {
//   StaticJsonBuffer<350> jsonBuffer;
//   String response;
//
//   WeatherConfig c = weatherStation->getWeatherConfig();
//   JsonObject& root = jsonBuffer.createObject();
//   root["o"] = c.getOwnerName();
//   root["an"] = c.getAPName();
//   root["apwd"] = c.getAPPassword();
//   root["ssid"] = c.getSSID();
//   root["pwd"] = c.getWifiPassword();
//   root["tk"] = c.getThingSpeakAPIKey();
//   root["tc"] = c.getThingSpeakChannelID();
//   root["bk"] = c.getBlynkAPIKey();
//   root["dl"] = c.getDebugLevelName();
//   root.printTo(response);
//
//   sendAPIJsonResponse(client, response);
// }

// void CubeSatServer::sendErrorResponse(WiFiClient &client, int status, String statusPhrase) {
//   client.print("HTTP/1.1 " + String(status) + " " + statusPhrase + "\r\n");
//   client.print("Connection: close\r\n");  // the connection will be closed after completion of the response
//   client.print("Content-Length: 0\r\n\r\n\r\n");
// }

// void CubeSatServer::sendFile(WiFiClient &client, String requestPath) {
//   String contentType = "text/html";
//   if (requestPath.endsWith(".css")) { contentType = "text/css"; }
//   else if (requestPath.endsWith(".html")) { contentType = "text/html"; }
//   else if (requestPath.endsWith(".js")) { contentType = "application/javascript"; }
//
//   SPIFFS.begin();
//   File serverSideFile = SPIFFS.open(requestPath, "r");
//   if (!serverSideFile) {
//     this->debugger->logln(DEBUG_LEVEL_ERROR, String("Failed to open file " + serverSideFile));
//     sendErrorResponse(client, 404, "Not Found");
//   } else {
//     int fileSize = serverSideFile.size();
//     client.print("HTTP/1.1 200 OK\r\n");
//     client.print("Content-Type: " + contentType + "\r\n");
//     client.print("Connection: close\r\n");  // the connection will be closed after completion of the response
//     client.print("Content-Length: " + String(fileSize) + "\r\n");
//     client.print("\r\n");
//
//     const int TXFR_BUFFER_SIZE = 128;
//     char *buffer = new char[TXFR_BUFFER_SIZE];
//     while (serverSideFile.available()) {
//       size_t bytesRead = serverSideFile.readBytes(buffer, TXFR_BUFFER_SIZE);
//       if (bytesRead > 0) {
//         client.write((const char*)buffer, bytesRead);
//         // give the web browser time to receive the data
//         delay(5);
//       }
//     }
//     client.print("\r\n\r\n");
//
//     // close the file
//     serverSideFile.close();
//   }
//   SPIFFS.end();
// }
//
// bool CubeSatServer::updateConfiguration(String &jsonConfig) {
//   WeatherConfig c = weatherStation->getWeatherConfig();
//   return c.saveConfigurationFile(jsonConfig);
// }
