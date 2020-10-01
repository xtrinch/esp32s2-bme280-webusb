# sensor-dashboard-ESP-bme280-reader

Sends periodic `bme280` humidity, temperature, gas, pressure and altitude data from `esp32/esp8266 development board` to [sensor dashboard](http://sensor-dashboard.trina.si/) backend.

Built with PlatformIO.

See also [backend](https://github.com/xtrinch/sensor-dashboard-nestjs-backend) and [frontend](https://github.com/xtrinch/sensor-dashboard-react-frontend), or [see it live](http://sensor-dashboard.trina.si/) with some sensor data from my living room.

## Setup

0. Make sure to clone with `git clone --recursive REPO-URL` to make sure ESP-make-easy submodule is cloned along
1. Have a sensor dashboard backend running (see https://github.com/xtrinch/sensor-dashboard-nestjs-backend)
2. Copy `import_env.example.py` to `import_env.py` and define your sensor dashboard url.
3. When board boots connect to its WiFi called `beetleAP` with password `beetleAP`
4. Input your WiFi's SSID, password and sensor access token with visiting this URL `192.168.4.1/?ssid=mySSID&password=myPassword&access_token=mysensoraccesstoken`

## Components
- Firebeetle ESP8266 or devkit v1 ESP32 (with minimal changes would work for other esp boards also)
- Bosch bme280 sensor module
- pushbutton (for resetting credentials & token)
- capacitor
- resistor
- 3.7V LiPo battery (or other)

## Schematic

Schematic below is made for Firebeetle ESP8266.

![Image of Yaktocat](https://github.com/sensor-dashboard/ESP-BME280-reader/blob/master/images/schematic.png)
