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

  // forced mode - need to force measurement, so that we hopefully save some power
  bme.setSampling(Adafruit_BME280::MODE_FORCED);

  // #ifdef DEBUG
  // bme_temp->printSensorDetails();
  // bme_pressure->printSensorDetails();
  // bme_humidity->printSensorDetails();
  // #endif
  
  return true;
}

bool makeMeasurement(bme280record * record) {
  // in forced mode, tell sensor explicitly to refresh measurements
  bme.takeForcedMeasurement();

  record->humidity = bme.readHumidity();
  record->temp = bme.readTemperature();
  record->pressure = bme.readPressure();

  ardprintf("Temp:%.2fC,hum:%.2f%%,pres:%.2fhPa", record->temp, record->humidity, record->pressure);
  return true;
}

bool getJsonPayload(char *buf, bme280record records[]) {
  StaticJsonDocument<1000> doc; // <- 1000 bytes in the heap

  JsonArray measurements = doc.createNestedArray("measurements");

  for (int i=0; i<maxRtcRecords; i++) {
    JsonObject measurement = measurements.createNestedObject();
    measurement["measurement"] = records[i].temp;
    measurement["measurementType"] = "temperature";
    measurement["timeAgo"] = (maxRtcRecords - i - 1) * SLEEP_SECONDS;
    JsonObject measurement1 = measurements.createNestedObject();
    measurement1["measurement"] = records[i].humidity;
    measurement1["measurementType"] = "humidity";
    measurement1["timeAgo"] = (maxRtcRecords - i - 1) * SLEEP_SECONDS;
    JsonObject measurement2 = measurements.createNestedObject();
    measurement2["measurement"] = records[i].pressure;
    measurement2["measurementType"] = "pressure";
    measurement2["timeAgo"] = (maxRtcRecords - i - 1) * SLEEP_SECONDS;
  }

  serializeJson(doc, (void *)buf, 900);
  Serial.print(buf);
  return true;
}