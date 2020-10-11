#ifndef bme280_H_INCLUDED
#define bme280_H_INCLUDED

#include <Arduino.h>
#include <Adafruit_BME280.h>
#include "common.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define CFG_I2C_ADDRESS I2C_ADDRESS

extern Adafruit_BME280 bme; // I2C

bool setupbme280();
bool makeMeasurement(char * buf);

#endif