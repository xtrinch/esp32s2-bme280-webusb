Import("env")

env.Append(CPPDEFINES=[
  ("SENSOR_DASHBOARD_URL", "sensor-dashboard.trina.si"),
  ("SPI_CS", 3), #  chip select - CSB on bme
  ("SPI_MOSI", 2), # SCA on bme
  ("SPI_MISO", 4), # SDO on bme
  ("SPI_SCK", 1), # clock SCL on bme
  ("DEBUG", 1),
  ("ENABLE_USB_CONFIGURATOR", 1),
])