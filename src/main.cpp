#include "main.h"
#include <Preferences.h>

RTC_DATA_ATTR int recordCounter = 0;
RTC_DATA_ATTR bme280record records[100]; // max 100, actual defined by config

#include <Adafruit_NeoPixel.h>  // go to Main Menu --> Sketch --> Include Library --> Manage libraries... search for Adafruit NeoPixel and install it (as of 4th February 2021 latest version is 1.7.0)
#define PIN 18
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

WebUSB WebUSBSerial;
Preferences preferences;

volatile int usbConnected = 0;
int maxRtcRecords = MAX_RTC_RECORDS;
int sleepInMinutes = CFG_SLEEP_SECONDS / 60;

class MyWebUSBCallbacks: public WebUSBCallbacks {
    void onConnect(bool state) {
      USBSerial.printf("webusb is %s\n", state ? "connected":"disconnected");
    }
};

class MyCDCUSBCallbacks: public CDCCallbacks {
  // when a serial monitor connects
  bool onConnect(bool dtr, bool rts) {
    USBSerial.printf("Connected");
    // usbConnected = 1;
    return true;
  }
  void onData() { 
    USBSerial.printf("Received");
    // usbConnected = 1;
  }
  void onCodingChange(cdc_line_coding_t const* p_line_coding) { 
    USBSerial.printf("C:");
    usbConnected = 1;
  }
};

void sleep() {
  if (!usbConnected) {
    goToSleep(sleepInMinutes*60);
  }
}

bool isCfgSaved() {
  preferences.begin("iotfreezer", true);
  bool cfgSaved = preferences.getBool("cfg_saved");
  preferences.end();
  return cfgSaved;
}

bool saveCfg(
  const char * ssid, 
  const char * password, 
  const char * sensorAccessToken,
  uint8_t timeBetweenMeasurements, // min
  uint8_t maxRtcRecords // how many records before they are sent to server
) {
  if (!ssid || strlen(ssid) == 0 || !password || strlen(password) == 0) {
    ardprintf("Need SSID & password to save config");
    return false;
  }
  
  bool retVal = true;

  preferences.begin("iotfreezer", false);

  if (
    !preferences.putString("ssid", ssid) ||
    !preferences.putString("password", password) ||
    !preferences.putString("access_token", sensorAccessToken) ||
    !preferences.putInt("time_between", timeBetweenMeasurements) ||
    !preferences.putInt("max_rtc_records", maxRtcRecords) ||
    !preferences.putBool("cfg_saved", true)
  ) {
    USBSerial.printf("Could not save some config data to EEPROM");
    retVal = false;
  }

  USBSerial.println("Values are:");

  USBSerial.println(ssid);
  USBSerial.println(preferences.getString("ssid", "Unknown"));
  USBSerial.println(password);
  USBSerial.println(preferences.getString("password", "Unknown"));

  preferences.end();

  return retVal;
}

void setup() {
  Serial.begin(115200);
  udpSerial.begin(115200);
  Serial.setDebugOutput(true);
  udpSerial.setDebugOutput(true);

  WiFi.mode(WIFI_MODE_NULL);
  pixels.begin();
  pixels.clear();
  pixels.show();

  WebUSBSerial.landingPageURI("iotfreezer.com", false);
  WebUSBSerial.deviceID(0x2341, 0x0002);
  WebUSBSerial.setCallbacks(new MyWebUSBCallbacks());
  USBSerial.setCallbacks(new MyCDCUSBCallbacks());

  if(!WebUSBSerial.begin()) {
    USBSerial.println("Failed to start webUSB stack");
  }

  if(!USBSerial.begin()) {
    USBSerial.println("Failed to start USB stack");
  }

  #ifndef PRECONFIGURED
    // setupEEPROM();
    if (isCfgSaved()) {
      preferences.begin("iotfreezer", true);
      maxRtcRecords = preferences.getInt("max_rtc_records");
      sleepInMinutes = preferences.getInt("time_between");
      preferences.end();
    }
  #endif

  // wait if usb connection appears - below 500 won't work
  delay(500);
  if (usbConnected) {
    ardprintf("USB connected!");
    if (isCfgSaved()) {
      // green
      pixels.setPixelColor(0, pixels.Color(0, 255, 0)); 
    } else {
      // red
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    }
    pixels.show();
    // TODO: uncomment
    // return; // don't do the measurement
  }

  // TODO: comment out
  delay(10000);

  if (!isCfgSaved()) {
    USBSerial.println("No config saved :(");
    sleep();
  };

  if (!setupbme280()) {
    USBSerial.println("BME setup is falsch :(");
    sleep();
  };

  USBSerial.printf("Measurement %d starting", recordCounter);

  // make a sensor reading
  if (!makeMeasurement(&records[recordCounter])) {
    USBSerial.printf("Failed to perform reading :(");
    sleep();
    return;
  }

  USBSerial.printf("Measurement %d done", recordCounter);
  recordCounter++;

  if (recordCounter < maxRtcRecords) {
    USBSerial.printf("Going to sleep, next up is: %d", recordCounter);
    sleep();
    return;
  }
  recordCounter = 0;

  if (!setupWiFi()) {
    sleep();
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
  int httpCode = makeSecureNetworkRequest("https://iotfreezer.com/api/measurements/multi", accessToken, jsonPayload, NULL, "POST", ca_cert);

  USBSerial.printf("API call HTTP code: %d", httpCode);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  sleep();
}

void echo_all(char c)
{
    Serial.write(c);
    WebUSBSerial.write(c);
    USBSerial.write(c);
}

char inputJson[300] = "";
int i = 0;
bool fullWordRead = false;

// only ever come here if we didn't go to sleep in setup - always usb connected
void loop() {
  // read char by char, usually comes in two blocks
  while (WebUSBSerial.available()) {
    char c = WebUSBSerial.read();

    // stop character is 0, everything else is the input json
    if (c != 0) {
      inputJson[i] = c;
      i++;
    } else {
      inputJson[i] = '\0';
      i = 0;
      fullWordRead = true;
    }
  }

  if (fullWordRead) {
    USBSerial.write((uint8_t *)inputJson, strlen(inputJson));
    USBSerial.write('\n');

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, inputJson);
    JsonObject obj = doc.as<JsonObject>();
    bool configSaved = saveCfg(
      obj["wifiSSID"], 
      obj["wifiPassword"], 
      obj["accessToken"],
      obj["timeBetweenMeasurements"].as<uint8_t>(),
      obj["maxRtcRecords"].as<uint8_t>()
    );
    if (configSaved) {
      pixels.setPixelColor(0, pixels.Color(255, 165, 0));
      pixels.show();
      WebUSBSerial.write((uint8_t *)"success", strlen("success"));

      if (!isCfgSaved()) {
        USBSerial.println("No config saved :(");
      } else {
        USBSerial.println("Success saving config!");
      }
    } else {
      WebUSBSerial.write((uint8_t *)"failure", strlen("failure"));
    }

    inputJson[0] = '\0';
    fullWordRead = false;
  }
}