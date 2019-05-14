// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "CubeSatSSD1306Display.h"

CubeSatSSD1306Display::CubeSatSSD1306Display() {
}

void CubeSatSSD1306Display::init(CubeSatConfig &config) {
  this->debugger    = CubeSatDebug::getCubeSatDebugger();
  this->cubeSatName = config.getCubeSatName();
  this->initDisplay();
  this->showInitialState();
}

void CubeSatSSD1306Display::showSingleLineText(const char *text) {
  this->display->clearDisplay();

  this->display->setTextSize(1);
  this->display->setTextColor(WHITE);
  this->display->setCursor(0,0);
  this->display->print(this->cubeSatName.c_str());

  this->display->setTextSize(2);
  this->display->setCursor(0, 25);
  this->display->println(text);

  this->display->display();
}

void CubeSatSSD1306Display::showInternalSystemStatus(CubeSatData &cubeSatData) {
  this->display->clearDisplay();

  this->display->setTextSize(1);
  this->display->setTextColor(WHITE);
  this->display->setCursor(0,0);
  this->display->print(this->cubeSatName.c_str());

  this->display->setCursor(0, 25);
  this->display->print("Temp       : "); this->display->println(cubeSatData.tempF);
  this->display->print("Humidity   : "); this->display->print(cubeSatData.humidity); this->display->println("%");
  this->display->print("Pressure   : "); this->display->println(cubeSatData.pressure);

  this->display->display();
}

void CubeSatSSD1306Display::showInitialState(){
  this->showSingleLineText("No Signal");
}

void CubeSatSSD1306Display::showWaitingForFirmware() {
  this->showSingleLineText("Signal\nAcquired");
  delay(3000);
  this->showSingleLineText("Waiting\nfor FW");
}

void CubeSatSSD1306Display::showNeedsInitialization() {
  this->showSingleLineText("Init");
}

void CubeSatSSD1306Display::showLookingForSignal(int strength) {
  String signalMsg = String("Sig: ") + String(strength) + String(" %");
  Serial.println(signalMsg);
  this->showSingleLineText(signalMsg.c_str());
}

void CubeSatSSD1306Display::showDownloadingFirmware(uint8_t progress) {
  String msg = String(progress) + String(" %");
  Serial.println(msg);
  this->showSingleLineText(msg.c_str());
}

void CubeSatSSD1306Display::showRebooting() {
  this->showSingleLineText("Rebooting");
}

void CubeSatSSD1306Display::showNeedsFanAdjustment() {
  this->showSingleLineText("TOO HOT!");
}

void CubeSatSSD1306Display::initDisplay() {
  delay(200);

  this->display = new Adafruit_SSD1306(4);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  this->display->begin(SSD1306_SWITCHCAPVCC, ADAFRUIT_DISPLAY_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  //this->display->display(); // show splashscreen
  //delay(1500);
  this->display->clearDisplay();   // clears the screen and buffer
}
