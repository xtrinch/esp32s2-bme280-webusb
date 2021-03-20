#include "main.h"

RTC_DATA_ATTR int recordCounter = 0;
RTC_DATA_ATTR bme280record records[MAX_RTC_RECORDS];

#include <Adafruit_NeoPixel.h>  // go to Main Menu --> Sketch --> Include Library --> Manage libraries... search for Adafruit NeoPixel and install it (as of 4th February 2021 latest version is 1.7.0)
#define PIN 18
#define NUMPIXELS 1
#define PERIOD  10  //ms

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int R=255, G=0, B=0;

WebUSB WebUSBSerial;
CDCusb USBSerial;

bool cdcConnected = false;
int usbConnected = 0;

class MyWebUSBCallbacks : public WebUSBCallbacks {
    void onConnect(bool state) {
      USBSerial.printf("webusb is %s\n", state ? "connected":"disconnected");
    }
};

class MyCDCUSBCallbacks : public CDCCallbacks {
  // void onConnect(bool state) {
  //   USBSerial.printf("cdc is %s\n", state ? "connected":"disconnected");
  // }
  // void onData() { 
  //   USBSerial.printf("Received");
  // }
  void onCodingChange(cdc_line_coding_t const* p_line_coding) { 
    USBSerial.printf("Coding change");
    usbConnected = 1;
  }
};

void waitForConfig() {
  while (!configured);
}

void setup() {
  WiFi.mode(WIFI_MODE_NULL);
  pixels.begin();

  WebUSBSerial.landingPageURI("localhost:3001", false);
  WebUSBSerial.deviceID(0x2341, 0x0002);
  WebUSBSerial.setCallbacks(new MyWebUSBCallbacks());
  USBSerial.setCallbacks(new MyCDCUSBCallbacks());

  if(!WebUSBSerial.begin()) {
    USBSerial.println("Failed to start webUSB stack");
  }

  if(!USBSerial.begin()) {
    USBSerial.println("Failed to start USB stack");
  }

  return;

  #ifndef PRECONFIGURED
    setupEEPROM();
    // clearConfig(); // uncomment when you want to programmatically clear config
  #endif
  while (!isConfigSaved());

  if (!setupbme280()) {
    goToSleep();
  };

  ardprintf("Measurement %d starting", recordCounter);

  // make a sensor reading
  if (!makeMeasurement(&records[recordCounter])) {
    ardprintf("Failed to perform reading :(");
    goToSleep();
    return;
  }

  ardprintf("Measurement %d done", recordCounter);
  recordCounter++;

  if (recordCounter < MAX_RTC_RECORDS) {
    ardprintf("Going to sleep, next up is: %d", recordCounter);
    goToSleep();
    return;
  }
  recordCounter = 0;

  if (!setupWiFi()) {
    goToSleep();
  }

  char jsonPayload[900];
  getJsonPayload(jsonPayload, records);

  char accessToken[60] = CFG_ACCESS_TOKEN;
  #ifndef PRECONFIGURED
  readFromEEPROM(accessToken, "access_token");
  #endif

  const char* ca_cert = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
  "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
  "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
  "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
  "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
  "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
  "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
  "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
  "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
  "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
  "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
  "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
  "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
  "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
  "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
  "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
  "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
  "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
  "-----END CERTIFICATE-----\n";
  makeSecureNetworkRequest("https://iotfreezer.com/api/measurements/multi", accessToken, jsonPayload, NULL, "POST", ca_cert);
}

void echo_all(char c)
{
    Serial.write(c);
    WebUSBSerial.write(c);
    USBSerial.write(c);
}

void loop() {
  
  if (usbConnected) {
    while (WebUSBSerial.available()) {
        echo_all(WebUSBSerial.read());
    }

    while (Serial.available()) {
        echo_all(Serial.read());
    }

    while (USBSerial.available()) {
        echo_all(USBSerial.read());
    }
  }

  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
  if (!usbConnected) {
    return;
  }

  for (G=0; G<255; G++)
  {
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    // delay (PERIOD);
  }
  
  for (R=255; R>0; R--)
  {
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    // delay (PERIOD);
  }
  
  for (B=0; B<255; B++)
  {
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    // delay (PERIOD);
  }
  
  for (G=255; G>0; G--)
  {
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    // delay (PERIOD);
  }
  
  for (R=0; R<255; R++)
  {
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    // delay (PERIOD);
  }
  
  for (B=255; B>0; B--)
  {
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    // delay (PERIOD);
  }

  return;
  /* do nothing in loop except check button, 
    esp will be in deep sleep in between measurements which will make setup re-run */

  #ifndef PRECONFIGURED
  if (checkButtonPressed()) {
      // do not execute anything else when button is pressed
    return;
  }
  #endif

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  goToSleep();
}