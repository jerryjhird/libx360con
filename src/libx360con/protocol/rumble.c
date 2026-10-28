#include "rumble.h"

int _internal_x360con_set_rumble(libusb_device_handle *dev_handle, uint8_t large_motor, uint8_t small_motor) {
    struct xbox360_rumble_cmd cmd = {
        .type = 0x00,
        .length = 0x08,
        .padding = 0x00,
        .large_motor = large_motor,
        .small_motor = small_motor,
        .reserved = {0, 0, 0}
    };

    int transferred = 0;
    int r = libusb_interrupt_transfer(
        dev_handle, 
        0x02,
        (unsigned char*)&cmd,
        sizeof(cmd),
        &transferred, 
        1000
    );
    
    return (r < 0) ? -1 : 0;
}