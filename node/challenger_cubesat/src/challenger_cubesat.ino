// ====================================================================================================================
// ====================================================================================================================
// Copyright (c) 2017 Ryan Brock. All rights reserved.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.
// ====================================================================================================================
// ====================================================================================================================
#include "CubeSatCommon.h"
#include "CubeSatWiFi.h"
#include "CubeSatDebug.h"

#define KNOB_DIAL A0
#define KNOB_READ_DELAY 10
#define KNOB_DELTA_THRESHOLD 20
#define KNOB_MAX 1024
#define KNOB_MIN 0

//SimpleTimer timer;

// void timerEvent() {
//   weatherStation.timerEvent();
// }

Adafruit_SSD1306 *display;

struct CubeSatData {
  unsigned long lastRead;
  float tempF;
  float humidity;
  float pressure;
  int brightness;
  uint8_t switchState;
};

int16_t           lastKnobValue   = 0;
const byte PcfButtonLedPin = 0;
BME280                   bme280;
CubeSatData              dataSample;
CubeSatConfig            config;
CubeSatWiFi       wsWifi;
 CubeSatDebug             *debugger;
#define PRESSURE_CONVERSION      0.000295299830714 /* merc: 0.000295299830714 / psi: 0.000145037738*/

uint16_t interruptsFired = 0;
volatile int      lastChange      = 0;
volatile boolean  buttonPressedInterruptFired  = false;

#define DEBOUNCE_TIME 300
#define WS_LISTEN_PORT 80

WiFiServer            server(WS_LISTEN_PORT);

// Called when the button is pressed
void buttonISR() {
  if ((millis() - lastChange) > DEBOUNCE_TIME) {
    buttonPressedInterruptFired = true;
    lastChange = millis();
    interruptsFired++;
    Serial.println("asdf");
    //detachInterrupt(digitalPinToInterrupt(BUTTON_SINK));
  }
}
PCF8574 pcf8574(0x20, D6, buttonISR);

void initDisplay() {
  delay(200);

  display = new Adafruit_SSD1306(4);

   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display->begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  display->display(); // show splashscreen
  delay(1500);
  display->clearDisplay();   // clears the screen and buffer
}

void setup() {
  Serial.begin(COMM_BAUD_RATE);
  delay(3000); // Allow for the USB to connect
  initDisplay();

  CubeSatDebug::init();
  debugger = CubeSatDebug::getCubeSatDebugger();

  debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  debugger->logln(DEBUG_LEVEL_INFO, "                                CubeSat Startup ");
  debugger->logln(DEBUG_LEVEL_INFO, "=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
  debugger->logln(DEBUG_LEVEL_INFO, "Starting ...");


  config.loadConfigurationFile();
  debugger->setDebugLevel(config.getDebugLevel());

  wsWifi.enableAP(config, true);
  server.begin();
// pinMode(D7, INPUT);
// pinMode(D6, INPUT);
// pinMode(D5, INPUT);
// pinMode(D4, INPUT);
// pinMode(D3, INPUT);
// attachInterrupt(digitalPinToInterrupt(BUTTON_SINK), buttonISR, FALLING);

  pinMode(KNOB_DIAL, INPUT);
// pcf20.begin();
pcf8574.pinMode(P0, INPUT);
pcf8574.pinMode(P1, INPUT);
pcf8574.pinMode(P4, OUTPUT);
pcf8574.begin();
initializeSensors();

  // CubeSatDebug::init();
  // weatherStation.begin();
  // timer.setInterval(SENSOR_SAMPLE_RATE, timerEvent);
}


// Main Application Loop
void loop() {
  boolean buttonPushed = buttonPressedInterruptFired;

  if (buttonPressedInterruptFired) {
    Serial.println("Button State Change");
    buttonPressedInterruptFired = false;
    boolean SW1_S1 = pcf8574.digitalRead(P0);
    boolean SW1_S2 = pcf8574.digitalRead(P1);

    Serial.print("Switch 1, State I: "); Serial.println(SW1_S1);
    Serial.print("Switch 1, State II: "); Serial.println(SW1_S2);
    //attachInterrupt(digitalPinToInterrupt(BUTTON_SINK), buttonISR, LOW);
  }


  WiFiClient client = server.available();
  if (client) {
    debugger->logln(DEBUG_LEVEL_INFO, "Client Connected to Web Server");
  }
  //weatherStation.applicationLoop();
  //timer.run();

  // Serial.print("D3: "); Serial.println(digitalRead(D3));
  // Serial.print("D4: "); Serial.println(digitalRead(D4));
  // Serial.print("D5: "); Serial.println(digitalRead(D5));
  // Serial.print("D6: "); Serial.println(digitalRead(D6));
  // Serial.print("D7: "); Serial.println(digitalRead(D7));


  boolean knobTurned = didTheKnobGetTurned();
  Serial.print("PIN 0: "); Serial.println(pcf8574.digitalRead(P4));

  if (knobTurned) {
    lastKnobValue = getLatestKnobValue();
    Serial.print("Knob Value:  "); Serial.println(lastKnobValue);
    // speedValue = (lastKnobValue / KNOB_CONVERSION);
    // brightnessValue = (((KNOB_MAX - lastKnobValue) / KNOB_CONVERSION));
    //
    // if (speedValue > 255) { speedValue = 255; }
    // if (speedValue < 1) { speedValue = 1; }
    // if (brightnessValue > 255) { brightnessValue = 255; }
    // if (brightnessValue < 1) { brightnessValue = 1; }
  }

  readSensors();
  pcf8574.digitalWrite(P4, LOW);
  //Serial.print("Last Error: "); Serial.println(pcf20.lastError());
  delay(2000);
  pcf8574.digitalWrite(P4, HIGH);
  //Serial.print("Last Error: "); Serial.println(pcf20.lastError());
  delay(2000);


}

void initializeSensors() {
  // Setup the A0 pin (GPIO) to Input
  pinMode(A0, INPUT);

  // Communication Mode Intialization
  bme280.settings.commInterface = I2C_MODE;
  bme280.settings.I2CAddress = 0x76;


  bme280.settings.runMode         = 3;
  bme280.settings.tStandby        = 0;
  bme280.settings.filter          = 0;
  bme280.settings.tempOverSample  = 1;
  bme280.settings.pressOverSample = 1;
  bme280.settings.humidOverSample = 1;

  // Give the BME280 enough time to start up (takes 2ms)
  delay(10);
  bme280.begin();
}

uint16_t getLatestKnobValue() {
  // Read the dial 3 times and find out what the average is to get rid of any outliers
  uint16_t rawValue1 = analogRead(KNOB_DIAL);
  //Serial.print(rawValue1); Serial.print("\t");
  delay(KNOB_READ_DELAY);
  uint16_t rawValue2 = analogRead(KNOB_DIAL);
  //Serial.print(rawValue2); Serial.print("\t");
  delay(KNOB_READ_DELAY);
  uint16_t rawValue3 = analogRead(KNOB_DIAL);
  //Serial.print(rawValue3); Serial.print("\t");

  uint16_t average = (rawValue1 + rawValue2 + rawValue3) / 3;
  //Serial.println(average);

  return average;
}

void readSensors() {
  boolean SW1_S1 = pcf8574.digitalRead(P0);
  boolean SW1_S2 = pcf8574.digitalRead(P1);
  uint8_t switchState = SW1_S1 ? 2 : (SW1_S2 ? 1 : 0);

  dataSample.lastRead   = millis();
  dataSample.tempF      = bme280.readTempF();
  dataSample.humidity   = bme280.readFloatHumidity();
  dataSample.pressure   = bme280.readFloatPressure() * PRESSURE_CONVERSION;
  dataSample.brightness = getLatestKnobValue();
  dataSample.switchState = switchState;


  displayCubeSat(dataSample);
  // dataSample.brightness = 1024 - this->getLightIntensity();

  Serial.print("Sensor Values\nT: "); Serial.println(String(dataSample.tempF, 2));
  Serial.print("\tH: "); Serial.println(String(dataSample.humidity, 2));
}


boolean didTheKnobGetTurned() {
    boolean wasAChange = false;

    int16_t knobValue = getLatestKnobValue();

    if (abs(lastKnobValue - knobValue) > KNOB_DELTA_THRESHOLD) {
        wasAChange = true;
    }

    return wasAChange;
}

void displayCubeSat(CubeSatData &weatherData) {
  display->clearDisplay();

  display->setTextSize(1);
  display->setTextColor(WHITE);
  display->setCursor(0,0);
  display->println(config.getCubeSatName());

  display->setCursor(0, 25);
  display->print("Temp       : "); display->println(weatherData.tempF);
  display->print("Humidity   : "); display->print(weatherData.humidity); display->println("%");
  display->print("Pressure   : "); display->println(weatherData.pressure);
  display->print("Brightness : "); display->println(weatherData.brightness);
  display->print("Switch     : "); display->println(weatherData.switchState);

  display->display();

}
