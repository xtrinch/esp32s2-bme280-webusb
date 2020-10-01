#include "bme280.h"

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

bool setupbme280() {
  if (!bme.begin(0x76)) {
    ardprintf("bme280: Could not find a valid bme280 sensor, check wiring!");
    return false;
  }

  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();

  return true;
}

bool makeMeasurement(char * buf) {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  
  Serial.print(F("Temperature = "));
  Serial.print(temp_event.temperature);
  Serial.println(" *C");

  Serial.print(F("Humidity = "));
  Serial.print(humidity_event.relative_humidity);
  Serial.println(" %");

  Serial.print(F("Pressure = "));
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");
  
  snprintf(buf, 500, "{" 
      "\"measurements\":["
        "{"
            "\"measurement\": %f,"
            "\"measurementType\": \"humidity\""
        "},"
        "{"
            "\"measurement\": %f,"
            "\"measurementType\": \"temperature\""
        "},"
        "{"
            "\"measurement\": %f,"
            "\"measurementType\": \"pressure\""
        "}"
      "]"
  "}", humidity_event.relative_humidity, temp_event.temperature, pressure_event.pressure);
}