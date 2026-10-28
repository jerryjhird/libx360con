#pragma once

#include <stdint.h>
#include <libusb-1.0/libusb.h>

// rumble command (vibration)
struct xbox360_rumble_cmd {
    uint8_t type; // 0x00 for rumble
    uint8_t length; // always 0x08
    uint8_t padding; // always 0x00
    uint8_t large_motor; // left side 0x00 to 0xFF
    uint8_t small_motor; // right side 0x00 to 0xFF
    uint8_t reserved[3]; // padding
} __attribute__((__packed__));

int _internal_x360con_set_rumble(libusb_device_handle *dev_handle, uint8_t large_motor, uint8_t small_motor);
