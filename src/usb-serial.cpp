#include "usb-serial.h"

CDCusb USBSerial;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
UdpSerial udpSerial(0);
#endif

size_t UdpSerial::write(const uint8_t *buffer, size_t size){
    USBSerial.print((char *)buffer);
    Print::write(buffer, size);
}