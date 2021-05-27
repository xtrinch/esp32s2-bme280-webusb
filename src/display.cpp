// GxEPD_MinimumExample by Jean-Marc Zingg

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"

void setupDisplay() {
  display.init();
  display.display(false);
}

int randomNumber = 0;

void draw()
{
  display.setPartialWindow(0, 0, display.width(), display.height());

  // comment out next line to have no or minimal Adafruit_GFX code
  // display.setPartialWindow(0, 0, display.width(), display.height());
  display.clearScreen();
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);
  display.setCursor(0,0);
  display.firstPage();
  // do
  // {
    display.fillScreen(GxEPD_WHITE);
    // comment out next line to have no or minimal Adafruit_GFX code
    display.printf("My name is boxxy. %d", randomNumber);
  // }
  // while (display.nextPage());
  display.display(true); // full update

  display.powerOff();
  Serial.println("Update finished");
  delay(5000);
  randomNumber ++;
}
