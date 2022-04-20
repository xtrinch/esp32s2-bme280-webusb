#ifndef IOT_DISPLAY_H_INCLUDED
#define IOT_DISPLAY_H_INCLUDED

#include "bme280.h"

void setupDisplay();
void draw(bme280record * record);
void showPcConn();

#endif