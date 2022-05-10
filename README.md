# esp32s2-bme280-webusb

Sends periodic `bme280` humidity, temperature, pressure and battery voltage data from `esp32s2 development board` to [sensor dashboard](http://iotfreezer.com) backend via Wi-Fi. 

Is configurable via a [web interface](http://iotfreezer.com/) using webusb. Since the board uses native usb communication, the serial output will also be emulated via USB so it may not work as you would expect a regular serial connection - if it's configured wrongly, there will be no output in your serial monitor. If the board sleeps, when you connect it to USB, you will not be able to flash it. Board needs to be wide awake and listening to USB commands for flashing to work. When the board is connected to the computer - the neopixel of the esp32s2 will light up and the display should display `PC` instead of the usual measurements.

The board I used is: [ESP32-S2-DevKit-Lipo-USB from olimex](https://www.olimex.com/Products/IoT/ESP32-S2/ESP32-S2-DevKit-Lipo-USB/open-source-hardware).
There's a couple of things to do to make this work for this specific board:
1. remove the solder from LED1_ENABLE SMD jumper (i.e. open it), because it consumes a large current 
2. solder the BAT_SENS and PWR_SENS jumpers, to be able to detect battery voltage levels

If you botch the code and manage to put it to sleep when on USB, boot the board into bootloader mode by holding the button `BUT1` on the board when connecting to USB, then you will be able to flash via USB again.

Currently, this firmware can make it around 7 months on a 1200mA battery connected to the dev board. The board can be configured to send the measurements to the server once every N times via the parameter `maxRtcRecords` to optimize battery time.

Built with PlatformIO.

See also [backend](https://github.com/xtrinch/sensor-dashboard-nestjs-backend) and [frontend](https://github.com/xtrinch/sensor-dashboard-react-frontend) repo, or [see it live](http://iotfreezer.com/) with some sensor data from my living room.

## Setup

1. Have a sensor dashboard backend running (see https://github.com/xtrinch/sensor-dashboard-nestjs-backend) or use [iotfreezer](http://iotfreezer.com/).
2. Copy `import_env.example.py` to `import_env.py` and define your sensor dashboard url.
3. Run `pio run -t buildfs && pio run -t uploadfs` to upload the images
4. While having your board plugged into the computer, reboot the board and configure it via the web interface (via webusb) of the sensor dashboard (iotfreezer).

## Components
- ESP32S2 dev board with native usb support, meaning there should not be a usb to serial converter on the board, e.g. [ESP32-S2-DevKit-Lipo-USB from Olimex](https://www.olimex.com/Products/IoT/ESP32-S2/ESP32-S2-DevKit-Lipo-USB/open-source-hardware)
- Waveshare e-paper 1.54inch display - black and white version
- Bosch BME280 sensor module
- 3.7V LiPo battery (or other), e.g. [BATTERY-LIPO1400mAh from Olimex](https://www.olimex.com/Products/Power/BATTERY-LIPO1400mAh/)

## Additional things needed to assemble the PCB
- PH2.0 8 pin connectors for the display (bent needle for the PCB)
- min 2, max 4 1.8mm high spacers to mount the display above the ESP32S2, depending on how many sides of the display you want to fixate
- PH2.0 2pin connector for the battery (if the battery doesn't already come with one)
- bunch of male / female headers
- 1x push button - right angled for RESET button
- 1x push button - regular for BOOT button


## Troubleshooting

If you cannot upload to your board on linux, input `sudo chmod a+rw /dev/ttyUSB0`.

To monitor across reboots, as uploading and monitoring does not work, use the following command on Linux:
`while [ true ]; do cat /dev/ttyACM0; done`
 
To check out the usb descriptors, run `lsusb -v`. To check out the kernel logs for errors, run `dmesg`.
##

 `convert temperature-2-48.png -background white -alpha remove -alpha off -compress none -depth 24 -resize 48x48 temp-final.bmp`


## Production deploy

Make sure to turn off `DEBUG` to save power.

## PCB

See `schematics-w-display` folder.

![Image of Yaktocat](https://github.com/xtrinch/esp32s2-bme280-webusb/blob/master/images/pcb.jpg)

## End product with casing

Casing is parametric and drawn in FreeCad 0.19. It is made to fit the above PCB. For FreeCad project, see `casing-w-display` folder.

![Image of Yaktocat](https://github.com/xtrinch/esp32s2-bme280-webusb/blob/master/images/casing1.jpg)
![Image of Yaktocat](https://github.com/xtrinch/esp32s2-bme280-webusb/blob/master/images/casing2.jpg)

# TODOs
- PCB cable hole - either add one or move up
- BOOT button should be as up as possible
- additional USB chip for serial comm (top left)