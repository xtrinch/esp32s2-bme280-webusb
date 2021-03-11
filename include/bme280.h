#ifndef bme280_H_INCLUDED
#define bme280_H_INCLUDED

#include <Adafruit_BME280.h>
#include "ArduinoJson.h"
#include "main.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define CFG_I2C_ADDRESS I2C_ADDRESS

typedef struct bme280record {
  float temp;
  float pressure;
  float humidity;
} bme280record;

bool setupbme280();
bool makeMeasurement(bme280record *record);
bool getJsonPayload(char * buf, bme280record records[]);
#endif