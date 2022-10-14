#include "main.h"
#include "esp_adc_cal.h"

RTC_DATA_ATTR int recordCounter = 0;
RTC_DATA_ATTR bme280record records[100]; // max 100, actual defined by config

#define NEOPIXEL_PIN 18
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

WebUSB WebUSBSerial;
CDCusb USBSerial;
Preferences preferences;

volatile bool usbConnected = 0;

// in-sane defaults, which shouldn't be used
// if there's no user configuration anyway
int maxRtcRecords = 1; // always send
int sleepInMinutes = 1; // send once per minute

class MyWebUSBCallbacks: public WebUSBCallbacks {
    void onConnect(bool state) {
      usbConnected = true;
      ardprintf("webusb is %s\n", state ? "connected":"disconnected");
    }
};

class MyCDCUSBCallbacks: public CDCCallbacks {
  // when a serial monitor connects
  bool onConnect(int itf, bool dtr, bool rts) {
    ardprintf("Connected, dtr: %d, rts: %d, itf: %d", dtr, rts, itf);
    usbConnected = true;
    return true;
  }
  void onData() { 
    ardprintf("Received");
    usbConnected = true;
  }
  void onCodingChange(cdc_line_coding_t const* p_line_coding) { 
    ardprintf("C:");
    usbConnected = true;
  }
};

class MyUSBCallbacks: public USBCallbacks {
  void onMount() {
    ardprintf("Mounted");
    usbConnected = true;
  }
};

void sleep(bool forever = false) {
  goToSleep(sleepInMinutes*60, forever);
}

esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;

void setup() {
  WiFi.mode(WIFI_MODE_NULL);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  bool isReset = wakeup_reason != ESP_SLEEP_WAKEUP_TIMER;

  // this can stay, even if we're not using serial
  Serial.begin(115200);

  USBSerial.setCallbacks(new MyCDCUSBCallbacks());

  WebUSBSerial.landingPageURI("iotfreezer.com", true);
  WebUSBSerial.deviceID(0x2341, 0x0002);
  WebUSBSerial.setCallbacks(new MyWebUSBCallbacks());

  EspTinyUSB::registerDeviceCallbacks(new MyUSBCallbacks());

  if(!USBSerial.begin()) {
    ardprintf("Failed to start USB stack");
    return;
  }

  if(!WebUSBSerial.begin()) {
    ardprintf("Failed to start webUSB stack");
    return;
  }

  pinMode(BAT_SENS_PIN, INPUT);
  pinMode(PWR_SENS_PIN, INPUT);

  // calibrate the ADC with the measured VREF at 0 attenuation
  adc1_config_width(ADC_WIDTH_BIT_13);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_0); // TODO: channels should be parametrized from pin number
  esp_adc_cal_value_t val_type = 
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_13, REF_VOLTAGE, adc_chars);

  // IMPORTANT:
  // Uncomment, when you want to route VREF to a GPIO to measure it with a multimeter
  // routeVRefToGPIO(ADC_UNIT_1, GPIO_NUM_17);

  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
  double rawAdcPower = adc1_get_raw(ADC1_CHANNEL_6);   // read the input pin, 8129 max
  double vPowerMeasured = esp_adc_cal_raw_to_voltage(rawAdcPower, adc_chars);
  double vPower = ((vPowerMeasured/1000.0) * (1000+10000)) / 1000.0; // voltage divider equation
  
  bool connectedToPower = false;
  if (vPower > 3.0) {
    // as an alternative, this could be used for usb connected status, but
    // it would also fire when charging via a regular outlet
    connectedToPower = true;
  }

  double rawAdcBatteryVal = adc1_get_raw(ADC1_CHANNEL_7);
  double vBatMeasured = esp_adc_cal_raw_to_voltage(rawAdcBatteryVal, adc_chars);
  double vBat = ((vBatMeasured/1000.0) * (470000+4700000)) / 470000.0;

  pixels.begin();
  pixels.clear();
  pixels.show();

  if (isCfgSaved()) {
    preferences.begin("iotfreezer", true);
    maxRtcRecords = preferences.getInt("max_rtc_records");
    sleepInMinutes = preferences.getInt("time_between");
    ardprintf("Read max_rtc_records: %d", maxRtcRecords);
    ardprintf("Read time_between: %d", sleepInMinutes);
    preferences.end();
  }

  // wait if usb connection appears - we're supposed to reset the controller when connecting to pc
  // - below 500 won't work
  if ((isReset && ENABLE_USB_CONFIGURATOR)) {
    // we can afford to wait a little longer since we know that the
    // connected to power works well
    delay(1500);
    ardprintf("Connected!. Power is raw %f, measured %f, calculated %f", rawAdcPower, vPowerMeasured, vPower);
    if (usbConnected) {
      if (isCfgSaved()) {
        // green
        pixels.setPixelColor(0, pixels.Color(0, 255, 0)); 
      } else {
        // red
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      }
      pixels.show();
      if (ENABLE_DISPLAY) {
        setupDisplay();
        showPcConn();
      }
      // TODO: uncomment
      return; // don't do the measurement
    }
  }

  // TODO: comment out
  // delay(5000);

  ardprintf("Measured battery voltage is %f", vBat);
  if (vBat <= 3.2 && !connectedToPower && !usbConnected) {
    // go to sleep indefinetly, battery is empty
    sleep(true);
  }

  records[recordCounter].battery = (float)vBat;
  records[recordCounter].rawBattery = (float)rawAdcBatteryVal;

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
  }

  // save a copy, so we can display the most recent one on the display
  int oldRecordCounter = recordCounter;
  ardprintf("Measurement %d done", recordCounter);
  recordCounter++;

  if (recordCounter < maxRtcRecords) {
    ardprintf("Going to sleep, next up is: %d", recordCounter);
    sleep();
  }
  recordCounter = 0;

  // only refresh display when sending to cloud to save battery
  if (ENABLE_DISPLAY) {
    setupDisplay();
    draw(&records[oldRecordCounter], connectedToPower, sleepInMinutes);
  }

  // everything after this point is just sending the measurement
  // to internet, so it's safe to just return if we're not syncing to
  // the cloud or we're connected to power - because the readings will be
  // far too high to be realistic because charging heats it up
  if (!ENABLE_CLOUD_SYNC || connectedToPower) {
    sleep();
  }

  if (!connectToWifi()) {
    ardprintf("Wi-fi connection failed");
    sleep();
  }

  // around 2000 should be enough for max rtc records = 4
  char jsonPayload[2000];
  getJsonPayload(jsonPayload, records);

  preferences.begin("iotfreezer", true);
  String accessToken = preferences.getString("access_token");
  ardprintf("Read access token: %s", accessToken.c_str());
  preferences.end();

  int httpCode = makeSecureNetworkRequest(
    "https://iotfreezer.com/api/measurements/multi", 
    accessToken.c_str(), 
    jsonPayload, 
    NULL, 
    "POST", 
    IOT_FREEZER_CA_CERT
  );

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

    // recheck, just in case
    if (configSaved && isCfgSaved()) {
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