#include "bme280.h"

Adafruit_BME280 bme(SPI_CS, SPI_MOSI, SPI_MISO, SPI_SCK); // use SPI interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

bool setupbme280() {
  if (!bme.begin(CFG_I2C_ADDRESS)) {
    ardprintf("bme280: Could not find a valid bme280 sensor, check wiring!");
    return false;
  }

  // #ifdef DEBUG
  // bme_temp->printSensorDetails();
  // bme_pressure->printSensorDetails();
  // bme_humidity->printSensorDetails();
  // #endif
  
  return true;
}

bool makeMeasurement(bme280record * record) {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  
  ardprintf("Temp:%.2fC,hum:%.2f%%,pres:%.2fhPa", temp_event.temperature, humidity_event.relative_humidity, pressure_event.pressure);

  record->humidity = humidity_event.relative_humidity;
  record->temp = temp_event.temperature;
  record->pressure = pressure_event.pressure;

  return true;
}

// bool makeMeasurementAndJson(char * buf) {
//   sensors_event_t temp_event, pressure_event, humidity_event;
//   bme_temp->getEvent(&temp_event);
//   bme_pressure->getEvent(&pressure_event);
//   bme_humidity->getEvent(&humidity_event);
  
//   ardprintf("Temp:%.2fC,hum:%.2f%%,pres:%.2fhPa", temp_event.temperature, humidity_event.relative_humidity, pressure_event.pressure);

//   snprintf(buf, 500, "{" 
//       "\"measurements\":["
//         "{"
//             "\"measurement\": %f,"
//             "\"measurementType\": \"humidity\""
//         "},"
//         "{"
//             "\"measurement\": %f,"
//             "\"measurementType\": \"temperature\""
//         "},"
//         "{"
//             "\"measurement\": %f,"
//             "\"measurementType\": \"pressure\""
//         "}"
//       "]"
//   "}", humidity_event.relative_humidity, temp_event.temperature, pressure_event.pressure);

//   return true;
// }


bool getJsonPayload(char *buf, bme280record records[]) {
  StaticJsonDocument<1000> doc; // <- 1000 bytes in the heap

  JsonArray measurements = doc.createNestedArray("measurements");

  for (int i=0; i<MAX_RTC_RECORDS; i++) {
    JsonObject measurement = measurements.createNestedObject();
    measurement["measurement"] = records[i].temp;
    measurement["measurementType"] = "temperature";
    measurement["timeAgo"] = (MAX_RTC_RECORDS - i - 1) * SLEEP_SECONDS;
    JsonObject measurement1 = measurements.createNestedObject();
    measurement1["measurement"] = records[i].humidity;
    measurement1["measurementType"] = "humidity";
    measurement1["timeAgo"] = (MAX_RTC_RECORDS - i - 1) * SLEEP_SECONDS;
    JsonObject measurement2 = measurements.createNestedObject();
    measurement2["measurement"] = records[i].pressure;
    measurement2["measurementType"] = "pressure";
    measurement2["timeAgo"] = (MAX_RTC_RECORDS - i - 1) * SLEEP_SECONDS;
  }

  serializeJson(doc, (void *)buf, 900);
  Serial.print(buf);
  return true;
}