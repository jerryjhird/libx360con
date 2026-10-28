#pragma once

#include <stdint.h>
#include <libusb-1.0/libusb.h>

// LED command
struct xbox360_led_cmd {
    uint8_t type; // 0x01 for LED
    uint8_t length;  // always 0x03
    uint8_t pattern; // LED pattern
} __attribute__((__packed__));

int _internal_x360con_set_led(libusb_device_handle *dev_handle, uint8_t pattern);
