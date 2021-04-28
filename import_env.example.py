Import("env")

env.Append(CPPDEFINES=[
  ("SLEEP_SECONDS", 600), # how long the board will sleep between measurements in seconds
  ("MAX_RTC_RECORDS", 4), # how many measurements will be taken before they are sent to server
  ("SENSOR_DASHBOARD_URL", "sensor-dashboard.trina.si"),
  ("ONBOARD_WIFI_SSID", "ONBOARD-WIFI-SSID"), # onboard AP SSID during configuration
  ("ONBOARD_WIFI_PASSWORD", "ONBOARD-WIFI-PASSWORD"), # onboard AP PASSWORD during configuration
  ("WIFI_SSID", "ssid"), # define only if preconfiguring
	("WIFI_PASSWORD", "pass"), # define only if preconfiguring
  ("ACCESS_TOKEN", "token"), # define only if preconfiguring
  ("I2C_ADDRESS", 0x76),
  ("SPI_CS", 3), #  chip select - CSB on bme
  ("SPI_MOSI", 2), # SCA on bme
  ("SPI_MISO", 4), # SDO on bme
  ("SPI_SCK", 1), # clock SCL on bme
  ("DEBUG", 1),
  ("PRECONFIGURED"), # turn preconfiguration on / off
])