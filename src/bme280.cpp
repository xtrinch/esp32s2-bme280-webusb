#include "bme280.h"

//int8_t cspin (CS), int8_t mosipin(SDI), int8_t misopin (SDO), int8_t sckpin (SCK)
Adafruit_BME280 bme(SPI_CS, SPI_MOSI, SPI_MISO, SPI_SCK); // use SPI interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

bool setupbme280() {
  if (!bme.begin(CFG_I2C_ADDRESS)) {
    ardprintf("bme280: Could not find a valid bme280 sensor, check wiring!");
    return false;
  }

  // bme_temp->printSensorDetails();
  // bme_pressure->printSensorDetails();
  // bme_humidity->printSensorDetails();

  return true;
}

bool makeMeasurement(char * buf) {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  
  ardprintf("Temp:%.2fC,hum:%.2f%%,pres:%.2fhPa", temp_event.temperature, humidity_event.relative_humidity, pressure_event.pressure);

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

  return true;
}