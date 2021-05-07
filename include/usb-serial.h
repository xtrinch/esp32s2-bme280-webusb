#ifndef UdpSerial_h
#define UdpSerial_h

#include <HardwareSerial.h>
#include "Arduino.h"
#include <cdcusb.h>

class UdpSerial: public HardwareSerial {
    using HardwareSerial::HardwareSerial;   // Inheriting constructors
    public: 
    size_t write(const uint8_t *buffer, size_t size) override;   // Overriding base functionality
};

extern CDCusb USBSerial;

extern UdpSerial udpSerial; // Making it available as udpSerial
#endif