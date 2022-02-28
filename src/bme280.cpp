#include "bme280.h"

Adafruit_BME280 bme(SENSOR_SPI_CS, SENSOR_SPI_MOSI, SENSOR_SPI_MISO, SENSOR_SPI_SCK); // use SPI interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

bool setupbme280() {
  if (!bme.begin()) {
    ardprintf("bme280: Could not find a valid bme280 sensor, check wiring!");
    return false;
  }

  // forced mode - need to force measurement, so that we hopefully save some power
  bme.setSampling(Adafruit_BME280::MODE_FORCED);

  // bme_temp->printSensorDetails();
  // bme_pressure->printSensorDetails();
  // bme_humidity->printSensorDetails();
  
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
  StaticJsonDocument<2500> doc; // <- over 2000 bytes in the heap for 4*4 measurements (maxRtcRecords= 4)

  JsonArray measurements = doc.createNestedArray("measurements");

  for (int i=0; i<maxRtcRecords; i++) {
    int timeAgo = (maxRtcRecords - i - 1) * (sleepInMinutes*60);
    JsonObject measurement = measurements.createNestedObject();
    measurement["measurement"] = records[i].temp;
    measurement["measurementType"] = "temperature";
    measurement["timeAgo"] = timeAgo;
    JsonObject measurement1 = measurements.createNestedObject();
    measurement1["measurement"] = records[i].humidity;
    measurement1["measurementType"] = "humidity";
    measurement1["timeAgo"] = timeAgo;
    JsonObject measurement2 = measurements.createNestedObject();
    measurement2["measurement"] = records[i].pressure;
    measurement2["measurementType"] = "pressure";
    measurement2["timeAgo"] = timeAgo;
    JsonObject measurement3 = measurements.createNestedObject();
    measurement3["measurement"] = records[i].battery;
    measurement3["measurementType"] = "battery-voltage";
    measurement3["timeAgo"] = timeAgo;
    JsonObject measurement4 = measurements.createNestedObject();
    measurement4["measurement"] = records[i].rawBattery;
    measurement4["measurementType"] = "raw-battery-voltage";
    measurement4["timeAgo"] = timeAgo;
  }

  serializeJson(doc, (void *)buf, 2000);
  return true;
}