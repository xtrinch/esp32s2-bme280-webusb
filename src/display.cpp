// GxEPD_MinimumExample by Jean-Marc Zingg

#include <GxEPD.h>

// select the display class to use, only one, copy from GxEPD_Example
#include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// constructor for AVR Arduino, copy from GxEPD_Example else
GxIO_Class io(SPI, /*CS=*/ CS_PIN, /*DC=*/ DC_PIN, /*RST=*/ RST_PIN); // arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io, /*RST=*/ RST_PIN, /*BUSY=*/ BUSY_PIN); // default selection of (9), 7

void drawHelloWorld();

void draw()
{
  display.init();
  display.setRotation(0);
  display.setCursor(0,0);
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);
  display.print("Moitsa Roikho");
  // display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  delay(600);
  display.powerDown();
}
