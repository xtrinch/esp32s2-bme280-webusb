#include "common.h"
#include "driver/rtc_io.h"

void ardprintf(const char *fmt, ...) {
  #ifndef DEBUG
  return;
  #endif

  int maxLen = 256;

  char buf[maxLen]; // resulting string limited to 128 chars
  buf[maxLen-1] = '\0';
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end(args);
  USBSerial.println(buf);
  Serial.println(buf);
}


void goToSleep(int secondsToSleep) {
  ardprintf("Sleep: Every %d sec", secondsToSleep);

  Serial.flush();

  esp_sleep_enable_timer_wakeup(secondsToSleep * uS_TO_S_FACTOR);
  // does not work, prolly needs to be some other button cuz it may be going into bootloader mode
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)GPIO_NUM_0, 0); // TODO: parametrize as wakeup pin
  // rtc_gpio_pullup_en((gpio_num_t)GPIO_NUM_0);
  // rtc_gpio_pulldown_dis((gpio_num_t)GPIO_NUM_0);

  // does not work, input does not go high enough
  // rtc_gpio_pulldown_en((gpio_num_t)PWR_SENS_PIN);
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)PWR_SENS_PIN, 1);

  // disables the brownout, apparently :)
  CLEAR_PERI_REG_MASK(RTC_CNTL_BROWN_OUT_REG, RTC_CNTL_BROWN_OUT_RST_ENA);
  esp_wifi_stop();
  esp_deep_sleep_start();
  ardprintf("This will never be printed");
}

int makeSecureNetworkRequest(const char * url, const char * authorization, const char * content, const char * response, const char * method, const char * certificate) {
  WiFiClientSecure client;
  if (certificate) {
    client.setCACert((const char *)certificate);
  }

  client.connect(url, 443);
  HTTPClient http;

  http.begin(client, url);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("Authorization", authorization);
  int httpResponseCode = -1;
  if (strcmp(method, "POST") == 0) {
    httpResponseCode = http.POST(content);
  } else {
    httpResponseCode = http.GET();
  }

  if (httpResponseCode > 0) {
    ardprintf("Station: HTTPS %d", httpResponseCode);
    const char * payload = http.getString().c_str();
    // ardprintf("%s", payload);
    if (response != NULL) {
      strcpy((char *)response, payload);
    }
    http.end();
  }
  else {
    http.end();
    ardprintf("Station: Error code: %d", httpResponseCode);
  }

  return httpResponseCode;
}

bool connectToWifi() {
  int wifiRetriesLeft = 100;

  preferences.begin("iotfreezer", true);
  String ssid = preferences.getString("ssid");
  String password = preferences.getString("password");
  preferences.end();

  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED && wifiRetriesLeft > 0) {
    delay(100);
    wifiRetriesLeft -= 1;
  }

  if (wifiRetriesLeft <= 0 || WiFi.status() != WL_CONNECTED) {
    ardprintf("Station: Could not connect to WiFi.");
    return false;
  }
  
  ardprintf("Station: Connected to WiFi");

  return true;
}