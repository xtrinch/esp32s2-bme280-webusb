#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "common.h"
#include "bme280.h"
#include "access-point.h"
#include "station.h"
#include "pushbutton.h"
#include "sleep.h"
#include "esp32-hal-cpu.h"

#include "usb-serial.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <webusb.h>
#include <cdcusb.h>

#define MAX_RTC_RECORDS 4

extern CDCusb USBSerial;
extern int maxRtcRecords;


#endif