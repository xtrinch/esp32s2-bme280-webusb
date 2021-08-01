Import("env")

env.Append(CPPDEFINES=[
  ("SENSOR_DASHBOARD_URL", "sensor-dashboard.trina.si"),
  ("SENSOR_SPI_CS", 3), #  sensor chip select - CSB on bme
  ("SENSOR_SPI_MOSI", 2), # sensor SCA on bme
  ("SENSOR_SPI_MISO", 4), # sensor SDO on bme
  ("SENSOR_SPI_SCK", 1), # sensor clock SCL on bme
  ("BUSY_PIN", 16), # display busy pin
  ("RST_PIN", 15), # display RST pin
  ("DC_PIN", 14), # display DC pin
  ("CS_PIN", 13), # display CS pin
  ("MOSI_PIN", 11), # display MOSI pin
  ("CLK_PIN", 12), # display CLK pin
  ("DEBUG", 1),
  ("ENABLE_USB_CONFIGURATOR", 1),
  ("BOARD_HAS_PSRAM", 1), # if your board has PSRam or not
  ("ENABLE_DISPLAY", 1), # if you'll be using it with a display or not
  ("REF_VOLTAGE", 1130), # multimeter measured reference voltage
  ("ENABLE_CLOUD_SYNC", 1), # whether the board should send measurement to the cloud
  ("PWR_SENS_PIN", 7), # with this pin we can sense whether we're connected to external power or not
])