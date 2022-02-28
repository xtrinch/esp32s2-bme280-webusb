#ifndef bme280_H_INCLUDED
#define bme280_H_INCLUDED

#include <Adafruit_BME280.h>
#include "ArduinoJson.h"
#include "common.h"

#define SEALEVELPRESSURE_HPA (1013.25)

typedef struct bme280record {
  float temp;
  float pressure;
  float humidity;
  float battery;
  float rawBattery;
} bme280record;

bool setupbme280();
bool makeMeasurement(bme280record *record);
bool getJsonPayload(char * buf, bme280record records[]);

#endif