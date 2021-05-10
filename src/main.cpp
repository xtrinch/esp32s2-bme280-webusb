#include "main.h"
#include <Preferences.h>

RTC_DATA_ATTR int recordCounter = 0;
RTC_DATA_ATTR bme280record records[100]; // max 100, actual defined by config

#include <Adafruit_NeoPixel.h>  // go to Main Menu --> Sketch --> Include Library --> Manage libraries... search for Adafruit NeoPixel and install it (as of 4th February 2021 latest version is 1.7.0)
#define PIN 18
#define NUMPIXELS 1
#define PWR_SENS_PIN 7
#define BAT_SENS_PIN 8

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

WebUSB WebUSBSerial;
CDCusb USBSerial;
Preferences preferences;

volatile int usbConnected = 0;

// in-sane defaults, which shouldn't be used
// if there's no user configuration anyway
int maxRtcRecords = 1; // always send
int sleepInMinutes = 1; // send once per minute

void ardprintf(const char *fmt, ...) {
  #ifndef DEBUG
  return;
  #endif

  char buf[128]; // resulting string limited to 128 chars
  buf[127] = '\0';
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end(args);
  USBSerial.println(buf);
  // Serial.println(buf);
}

class MyWebUSBCallbacks: public WebUSBCallbacks {
    void onConnect(bool state) {
      ardprintf("webusb is %s\n", state ? "connected":"disconnected");
    }
};

class MyCDCUSBCallbacks: public CDCCallbacks {
  // when a serial monitor connects
  bool onConnect(bool dtr, bool rts) {
    ardprintf("Connected");
    return true;
  }
  void onData() { 
    ardprintf("Received");
  }
  void onCodingChange(cdc_line_coding_t const* p_line_coding) { 
    ardprintf("C:");
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
  
  ardprintf("Saving ssid: %s", ssid);
  ardprintf("Saving password: %s", password);
  ardprintf("Saving access_token: %s", sensorAccessToken);
  ardprintf("Saving time_between: %d", timeBetweenMeasurements);
  ardprintf("Saving max_rtc_records: %d", maxRtcRecords);

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
    ardprintf("Could not save some config data to preferences");
    retVal = false;
  }

  preferences.end();

  return retVal;
}


bool connectToWifi() {
  int wifiRetriesLeft = WIFI_CONNECT_RETRIES;

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
    USBSerial.println("Station: Could not connect to WiFi.");
    return false;
  }
  
  USBSerial.println("Station: Connected to WiFi");

  return true;
}

void setup() {
  int power = analogRead(PWR_SENS_PIN);   // read the input pin, 1024 max
  if (power > 800) {
    // as an alternative, this could be used for usb connected status, but
    // it would also fire when charging via a regular outlet
    // usbConnected = 1;
  }

  double voltagePerNum = 3.3/8192.0;
  double vBatMeasured = analogRead(BAT_SENS_PIN)*voltagePerNum;   // read the input pin, 8192 max
  // double vBatMeasured = analogReadMilliVolts(BAT_SENS_PIN)/1000.0;
  double vBat = (vBatMeasured * (470000+4700000)) / 470000.0;
  double adcCalibrationValue = 0.895;
  vBat = vBat * adcCalibrationValue;

  WiFi.mode(WIFI_MODE_NULL);
  pixels.begin();
  pixels.clear();
  pixels.show();

  WebUSBSerial.landingPageURI("iotfreezer.com", false);
  WebUSBSerial.deviceID(0x2341, 0x0002);
  WebUSBSerial.setCallbacks(new MyWebUSBCallbacks());
  USBSerial.setCallbacks(new MyCDCUSBCallbacks());

  if(!WebUSBSerial.begin()) {
    ardprintf("Failed to start webUSB stack");
  }

  if(!USBSerial.begin()) {
    ardprintf("Failed to start USB stack");
  }

  if (isCfgSaved()) {
    preferences.begin("iotfreezer", true);
    maxRtcRecords = preferences.getInt("max_rtc_records");
    sleepInMinutes = preferences.getInt("time_between");
    ardprintf("Read max_rtc_records: %d", maxRtcRecords);
    ardprintf("Read time_between: %d", sleepInMinutes);
    preferences.end();
  }

  // wait if usb connection appears - below 500 won't work
  delay(500);
  if (usbConnected) {
    if (isCfgSaved()) {
      // green
      pixels.setPixelColor(0, pixels.Color(0, 255, 0)); 
    } else {
      // red
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    }
    pixels.show();
    // TODO: uncomment
    return; // don't do the measurement
  }

  // TODO: comment out
  // delay(5000);

  ardprintf("Measured battery voltage is %f", vBat);
  records[recordCounter].battery = (float)vBat;

  if (!isCfgSaved()) {
    ardprintf("No config saved :(");
    sleep();
  };

  if (!setupbme280()) {
    ardprintf("BME setup is falsch :(");
    sleep();
  };

  ardprintf("Measurement %d starting", recordCounter);

  // make a sensor reading
  if (!makeMeasurement(&records[recordCounter])) {
    ardprintf("Failed to perform reading :(");
    sleep();
    return;
  }

  ardprintf("Measurement %d done", recordCounter);
  recordCounter++;

  if (recordCounter < maxRtcRecords) {
    ardprintf("Going to sleep, next up is: %d", recordCounter);
    sleep();
    return;
  }
  recordCounter = 0;

  if (!connectToWifi()) {
    ardprintf("Wi-fi connection failed");
    sleep();
  }

  char jsonPayload[900];
  getJsonPayload(jsonPayload, records);

  preferences.begin("iotfreezer", true);
  String accessToken = preferences.getString("access_token");
  preferences.end();

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
  int httpCode = makeSecureNetworkRequest("https://iotfreezer.com/api/measurements/multi", accessToken.c_str(), jsonPayload, NULL, "POST", ca_cert);

  ardprintf("API call HTTP code: %d", httpCode);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  sleep();
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
      // blue, so we know we've configured it even if it was already configured
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.show();
      WebUSBSerial.write((uint8_t *)"success", strlen("success"));
    } else {
      WebUSBSerial.write((uint8_t *)"failure", strlen("failure"));
    }

    inputJson[0] = '\0';
    fullWordRead = false;
  }
}