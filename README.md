# esp32s2-bme280-webusb

Sends periodic `bme280` humidity, temperature, gas, pressure and altitude data from `esp32s2 development board` to [sensor dashboard](http://iotfreezer.com) backend. 

Is configurable via a [web interface](http://iotfreezer.com/) using webusb. Since the board uses native usb communication, the serial output will also be emulated via USB so it may not work as you would expect a regular serial connection - if it's configured wrongly, there will be no output in your serial monitor. If the board sleeps, when you connect it to USB, you will not be able to flash it. Board needs to be wide awake and listening to USB commands for flashing to work. When the board is connected to the computer - the neopixel of the esp32s2 will light up.

The board I used is: [ESP32-S2-DevKit-Lipo-USB from olimex](https://www.olimex.com/Products/IoT/ESP32-S2/ESP32-S2-DevKit-Lipo-USB/open-source-hardware).

If you botch the code and manage to put it to sleep when on USB, boot the board into bootloader mode by holding the button `BUT1` on the board when connecting to USB, then you will be able to flash via USB again.

Currently, this firmware can make it around 6 weeks on a 1600mA battery connected to the dev board. The board can be configured to send the measurements to the server once every N times via the parameter `maxRtcRecords` to optimize battery time.

Built with PlatformIO.

See also [backend](https://github.com/xtrinch/sensor-dashboard-nestjs-backend) and [frontend](https://github.com/xtrinch/sensor-dashboard-react-frontend) repo, or [see it live](http://iotfreezer.com/) with some sensor data from my living room.

## Setup

1. Have a sensor dashboard backend running (see https://github.com/xtrinch/sensor-dashboard-nestjs-backend) or use [iotfreezer](http://iotfreezer.com/).
2. Copy `import_env.example.py` to `import_env.py` and define your sensor dashboard url.
3. While having your board plugged into the computer, reboot the board and configure it via the web interface (via webusb) of the sensor dashboard (iotfreezer).

## Components
- ESP32S2 dev board with native (!) usb support -> there should not be a usb to serial converter on the board
- Bosch BME280 sensor module
- 3.7V LiPo battery (or other)

## Troubleshooting

If you cannot upload to your board on linux, input `sudo chmod a+rw /dev/ttyUSB0`.
