# sensor-dashboard-ESP-bme280-reader

Sends periodic `bme280` humidity, temperature, gas, pressure and altitude data from `esp32s2 development board` to [sensor dashboard](http://iotfreezer.com) backend.

Built with PlatformIO.

See also [backend](https://github.com/xtrinch/sensor-dashboard-nestjs-backend) and [frontend](https://github.com/xtrinch/sensor-dashboard-react-frontend), or [see it live](http://iotfreezer.com/) with some sensor data from my living room.

## Setup

0. Make sure to clone with `git clone --recursive REPO-URL` to make sure ESP-make-easy submodule is cloned along (if you forget the recursive flag, you can always run `git submodule update --init --recursive` in the root directory later)
1. Have a sensor dashboard backend running (see https://github.com/xtrinch/sensor-dashboard-nestjs-backend)
2. Copy `import_env.example.py` to `import_env.py` and define your sensor dashboard url.
3. While having your board plugged into the computer, reboot the board and configure it via the web interface (via webusb) of the sensor dashboard (iotfreezer).

## Components
- ESP32S2 dev board
- Bosch BME280 sensor module
- 3.7V LiPo battery (or other)

## Troubleshooting

If you cannot upload to your board on linux, input `sudo chmod a+rw /dev/ttyUSB0`.