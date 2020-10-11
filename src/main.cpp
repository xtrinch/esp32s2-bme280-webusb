#include "main.h"

// #ifdef ESP8266
// ADC_MODE(ADC_VCC);
// #endif

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setupEEPROM();

  // uncomment when you want to programmatically clear config
  // clearConfig();

  setupButton();

  if (!isConfigSaved()) {
    if (!setupAP()) {
      goToSleep();
    }

    listenForConfig();
    cleanupAP();
  }

  // do not do anything if button is pressed
  if (digitalRead(BTN_PIN) == BTN_PRESSED_STATE) {
    ardprintf("Button pressed upon startup, skipping WiFi setup");
    return;
  }

  if (!setupWiFi()) {
    goToSleep();
  }

  if (!setupbme280()) {
    goToSleep();
  };

  char jsonPayload[500];

  // make a sensor reading
  if (!makeMeasurement(jsonPayload)) {
    ardprintf("Failed to perform reading :(");
    goToSleep();
    return;
  }

  // #ifdef ESP8266
  // float vccReading = ESP.getVcc();
  // float voltage = vccReading / 1024.0;
  // ardprintf("Voltage: %.2f", voltage);
  // #endif

  char url[150];
  snprintf(url, 500, "https://%s/api/measurements/multi", CFG_SENSOR_DASHBOARD_URL);
  char accessToken[60] = CFG_ACCESS_TOKEN;
  #ifndef PRECONFIGURED
  readFromEEPROM(accessToken, "access_token");
  #endif

  makeSecureNetworkRequest(url, accessToken, jsonPayload);
}

void loop() {
  /* do nothing in loop except check button, 
    esp will be in deep sleep in between measurements which will make setup re-run */

  if (checkButtonPressed()) {
      // do not execute anything else when button is pressed
    return;
  }

  goToSleep();
}