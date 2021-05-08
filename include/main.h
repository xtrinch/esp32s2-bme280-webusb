#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "common.h"
#include "bme280.h"
#include "station.h"
#include "sleep.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <webusb.h>
#include <cdcusb.h>

#define MAX_RTC_RECORDS 4

extern CDCusb USBSerial;
extern int maxRtcRecords;


#endif