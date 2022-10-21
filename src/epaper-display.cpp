#include "epaper-display.h"

#include <SPIFFS.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include "bitmaps/Bitmaps200x200.h" // 1.54" b/w

static const uint16_t input_buffer_pixels = 800; // may affect performance

static const uint16_t max_row_width = 1448; // for up to 6" display 1448x1072
static const uint16_t max_palette_pixels = 256; // for depth <= 8

uint8_t input_buffer[3 * input_buffer_pixels]; // up to depth 24
uint8_t output_row_mono_buffer[max_row_width / 8]; // buffer for at least one row of b/w bits
uint8_t output_row_color_buffer[max_row_width / 8]; // buffer for at least one row of color bits
uint8_t mono_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 b/w
uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w
uint16_t rgb_palette_buffer[max_palette_pixels]; // palette buffer for depth <= 8 for buffered graphics, needed for 7-color display


void setupDisplay() {
  display.init();

  display.setRotation(1);
  SPIFFS.begin();
}

uint16_t read16(fs::File& f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File& f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void drawBitmapFromSpiffs(const char *filename, int16_t x, int16_t y, bool with_color = false)
{
  fs::File file;
  bool valid = false; // valid format to be handled
  bool flip = true; // bitmap is stored bottom-to-top
  uint32_t startTime = millis();
  if ((x >= display.epd2.WIDTH) || (y >= display.epd2.HEIGHT)) return;
  ardprintf("\n");
  ardprintf("Loading image '");
  ardprintf(filename);
#if defined(ESP32)
  file = SPIFFS.open(String("/") + filename, "r");
#else
  file = SPIFFS.open(filename, "r");
#endif
  if (!file)
  {
    ardprintf("File not found");
    return;
  }
  // Parse BMP header
  uint16_t signature = read16(file);
  if (signature == 0x4D42) // BMP signature
  {
    ardprintf("Read BMP signature");
    uint32_t fileSize = read32(file);
    uint32_t creatorBytes = read32(file);
    uint32_t imageOffset = read32(file); // Start of image data
    uint32_t headerSize = read32(file);
    uint32_t width  = read32(file);
    uint32_t height = read32(file);
    uint16_t planes = read16(file);
    uint16_t depth = read16(file); // bits per pixel
    uint32_t format = read32(file);
    ardprintf("Format");
    ardprintf("%d \n", format);
    if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
    {
      ardprintf("File size: "); ardprintf("%d \n", fileSize);
      ardprintf("Image Offset: "); ardprintf("%d \n", imageOffset);
      ardprintf("Header size: "); ardprintf("%d \n", headerSize);
      ardprintf("Bit Depth: "); ardprintf("%d \n", depth);
      ardprintf("Image size: ");
      ardprintf("%d", width);
      ardprintf("x");
      ardprintf("%d", height);
      // BMP rows are padded (if needed) to 4-byte boundary
      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
      if (depth < 8) rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
      if (height < 0)
      {
        height = -height;
        flip = false;
      }
      uint16_t w = width;
      uint16_t h = height;
      if ((x + w - 1) >= display.epd2.WIDTH)  w = display.epd2.WIDTH  - x;
      if ((y + h - 1) >= display.epd2.HEIGHT) h = display.epd2.HEIGHT - y;
      if (w <= max_row_width) // handle with direct drawing
      {
        valid = true;
        uint8_t bitmask = 0xFF;
        uint8_t bitshift = 8 - depth;
        uint16_t red, green, blue;
        bool whitish, colored;
        if (depth == 1) with_color = false;
        if (depth <= 8)
        {
          if (depth < 8) bitmask >>= depth;
          //file.seek(54); //palette is always @ 54
          file.seek(imageOffset - (4 << depth)); // 54 for regular, diff for colorsimportant
          for (uint16_t pn = 0; pn < (1 << depth); pn++)
          {
            blue  = file.read();
            green = file.read();
            red   = file.read();
            file.read();
            whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
            colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
            if (0 == pn % 8) mono_palette_buffer[pn / 8] = 0;
            mono_palette_buffer[pn / 8] |= whitish << pn % 8;
            if (0 == pn % 8) color_palette_buffer[pn / 8] = 0;
            color_palette_buffer[pn / 8] |= colored << pn % 8;
          }
        }
        // display.clearScreen();
        uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
        for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
        {
          uint32_t in_remain = rowSize;
          uint32_t in_idx = 0;
          uint32_t in_bytes = 0;
          uint8_t in_byte = 0; // for depth <= 8
          uint8_t in_bits = 0; // for depth <= 8
          uint8_t out_byte = 0xFF; // white (for w%8!=0 border)
          uint8_t out_color_byte = 0xFF; // white (for w%8!=0 border)
          uint32_t out_idx = 0;
          file.seek(rowPosition);
          for (uint16_t col = 0; col < w; col++) // for each pixel
          {
            // Time to read more pixel data?
            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
            {
              in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
              in_remain -= in_bytes;
              in_idx = 0;
            }
            switch (depth)
            {
              case 24:
                blue = input_buffer[in_idx++];
                green = input_buffer[in_idx++];
                red = input_buffer[in_idx++];
                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                break;
              case 16:
                {
                  uint8_t lsb = input_buffer[in_idx++];
                  uint8_t msb = input_buffer[in_idx++];
                  if (format == 0) // 555
                  {
                    blue  = (lsb & 0x1F) << 3;
                    green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                    red   = (msb & 0x7C) << 1;
                  }
                  else // 565
                  {
                    blue  = (lsb & 0x1F) << 3;
                    green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                    red   = (msb & 0xF8);
                  }
                  whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                  colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0)); // reddish or yellowish?
                }
                break;
              case 1:
              case 4:
              case 8:
                {
                  if (0 == in_bits)
                  {
                    in_byte = input_buffer[in_idx++];
                    in_bits = 8;
                  }
                  uint16_t pn = (in_byte >> bitshift) & bitmask;
                  whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                  colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                  in_byte <<= depth;
                  in_bits -= depth;
                }
                break;
            }
            if (whitish)
            {
              // keep white
            }
            else if (colored && with_color)
            {
              out_color_byte &= ~(0x80 >> col % 8); // colored
            }
            else
            {
              out_byte &= ~(0x80 >> col % 8); // black
            }
            if ((7 == col % 8) || (col == w - 1)) // write that last byte! (for w%8!=0 border)
            {
              output_row_color_buffer[out_idx] = out_color_byte;
              output_row_mono_buffer[out_idx++] = out_byte;
              out_byte = 0xFF; // white (for w%8!=0 border)
              out_color_byte = 0xFF; // white (for w%8!=0 border)
            }
          } // end pixel
          uint16_t yrow = y + (flip ? h - row - 1 : row);
          // display.drawBitmap(output_row_mono_buffer, output_row_color_buffer, x, yrow, w, 1);
          display.drawBitmap(x, yrow, output_row_mono_buffer, w, 1, GxEPD_WHITE, GxEPD_BLACK);
        } // end line
        ardprintf("loaded in "); ardprintf("%d", millis() - startTime); ardprintf(" ms");
        // display.refresh();
      }
    }
  } else {
    ardprintf("First line is weird");
    ardprintf("%d \n", signature);
  }
  file.close();
  if (!valid)
  {
    ardprintf("bitmap format not handled.");
  }
}

void draw(bme280record * record, bool connectedToPower, int sleepInMinutes) {
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.fillScreen(GxEPD_WHITE);

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextSize(1);

  drawBitmapFromSpiffs("thermometer.bmp", 4, 5);
  display.setCursor(75,49);
  display.printf("%.1f", record->temp);

  drawBitmapFromSpiffs("humidity.bmp", 0, 79);
  display.setCursor(75,128);
  display.printf("%.0f", record->humidity);

  drawBitmapFromSpiffs("battery.bmp", 155, 160);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextSize(1);
  // usable lipo battery cell voltage is from 3.2 to 4.2V,
  // the input will come in as e.g. 4.2 double
  if (connectedToPower) {
    display.setCursor(30,185);
    display.printf("charging");
  } else {
    display.setCursor(105,185);
    // 4.2 - 3.2 === 1, multiply with 100
    display.printf("%.0f%%", (record->battery - 3.2) * 100); // may go into -
  }

  // display.setCursor(5,165);
  // display.printf("wh: %d/%d", display.width(), display.height());

  display.display(false); // full update, some displays dont show anything with a partial one

  display.powerOff();
  display.hibernate();
  
  ardprintf("Update finished");
}

void showPcConn() {
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextSize(1);

  display.setCursor(70,110);
  display.printf("PC");

  display.display(false); // full update
  
  display.powerOff();
  display.hibernate();

  ardprintf("Update finished");
}
