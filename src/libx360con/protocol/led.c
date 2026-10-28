#include "led.h"

int _internal_x360con_set_led(libusb_device_handle *dev_handle, uint8_t pattern) {
    struct xbox360_led_cmd cmd = {
        .type = 0x01,
        .length = 0x03,
        .pattern = pattern & 0x0F
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
