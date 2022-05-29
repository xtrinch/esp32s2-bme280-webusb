#ifndef IOT_COMMON_H_INCLUDED
#define IOT_COMMON_H_INCLUDED

#include <Arduino.h>
#include <Wire.h>
#include <cdcusb.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_wifi.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <driver/adc.h>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

extern int maxRtcRecords;
extern int sleepInMinutes;

extern CDCusb USBSerial;
extern Preferences preferences;

void ardprintf(const char *fmt, ...);
void goToSleep(int secondsToSleep);
int makeSecureNetworkRequest(
  const char * url, 
  const char * authorization, 
  const char * content, 
  const char * response, 
  const char * method, 
  const char * certificate
);
bool connectToWifi();
bool saveCfg(
  const char * ssid, 
  const char * password, 
  const char * sensorAccessToken,
  uint8_t timeBetweenMeasurements, // min
  uint8_t maxRtcRecords // how many records before they are sent to server
);
bool isCfgSaved();
void check_efuse(void);
void routeVRefToGPIO(adc_unit_t adc_unit, gpio_num_t gpio);

#endif