# sensor-dashboard-ESP-bme280-reader

Sends periodic `bme280` humidity, temperature, gas, pressure and altitude data from `esp32/esp8266 development board` to [sensor dashboard](http://iotfreezer.com) backend.

Built with PlatformIO.

See also [backend](https://github.com/xtrinch/sensor-dashboard-nestjs-backend) and [frontend](https://github.com/xtrinch/sensor-dashboard-react-frontend), or [see it live](http://iotfreezer.com/) with some sensor data from my living room.

## Setup

0. Make sure to clone with `git clone --recursive REPO-URL` to make sure ESP-make-easy submodule is cloned along (if you forget the recursive flag, you can always run `git submodule update --init --recursive` in the root directory later)
1. Have a sensor dashboard backend running (see https://github.com/xtrinch/sensor-dashboard-nestjs-backend)
2. Copy `import_env.example.py` to `import_env.py` and define your sensor dashboard url.
3. When board boots connect to its WiFi called `beetleAP` with password `beetleAP`
4. Input your WiFi's SSID, password and sensor access token with visiting this URL `192.168.4.1/?ssid=mySSID&password=myPassword&access_token=mysensoraccesstoken`

## Components
- Firebeetle ESP8266, devkit v1 ESP32 or devkit v1 ESP32S2 (with minimal changes would work for other esp boards also)
- Bosch BME280 sensor module
- pushbutton (for resetting credentials & token)
- capacitor
- resistor
- 3.7V LiPo battery (or other)

## Schematic

Schematic below is made for Firebeetle ESP8266.

![Image of Yaktocat](https://github.com/sensor-dashboard/ESP-BME280-reader/blob/master/images/schematic.png)


## Troubleshooting

If you cannot upload to your board on linux, input `sudo chmod a+rw /dev/ttyUSB0`.