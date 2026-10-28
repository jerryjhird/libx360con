#pragma once

#include <stdint.h>
#include <libusb-1.0/libusb.h>
#include <pthread.h>

// controller inputs
enum x360con_controller_input {
    // digital Buttons
    INPUT_BUTTON_A,
    INPUT_BUTTON_B,
    INPUT_BUTTON_X,
    INPUT_BUTTON_Y,
    INPUT_BUTTON_LB,
    INPUT_BUTTON_RB,
    INPUT_BUTTON_START,
    INPUT_BUTTON_BACK,
    INPUT_BUTTON_GUIDE,
    INPUT_BUTTON_THUMB_L,
    INPUT_BUTTON_THUMB_R,
    
    // D-Pad
    INPUT_DPAD_UP,
    INPUT_DPAD_DOWN,
    INPUT_DPAD_LEFT,
    INPUT_DPAD_RIGHT,
    
    // analog triggers (value: 0 to 255)
    INPUT_TRIGGER_LT,
    INPUT_TRIGGER_RT,
    
    // joysticks (value: -32768 to 32767)
    INPUT_STICK_LEFT_X,
    INPUT_STICK_LEFT_Y,
    INPUT_STICK_RIGHT_X,
    INPUT_STICK_RIGHT_Y
};

// some dont work maybe?
enum x360con_led_pattern {
    LED_OFF         = 0x00, // all off
    LED_ALL_BLINK   = 0x01, // all blink
    LED_FLASH_1     = 0x02, // quadrant 1 flashes then stays on
    LED_FLASH_2     = 0x03, // quadrant 2 flashes then stays on
    LED_FLASH_3     = 0x04, // quadrant 3 flashes then stays on
    LED_FLASH_4     = 0x05, // quadrant 4 flashes then stays on
    LED_PLAYER_1    = 0x06, // player 1 steady (top left)
    LED_PLAYER_2    = 0x07, // player 2 steady (top right)
    LED_PLAYER_3    = 0x08, // player 3 steady (bottom left)
    LED_PLAYER_4    = 0x09, // player 4 steady (bottom right)
    LED_ROTATING    = 0x0A, // spinning (1 > 2 > 3 > 4)
    LED_BLINK       = 0x0B, // blink
    LED_SLOW_BLINK  = 0x0C, // slow blink
    LED_ALTERNATING = 0x0D  // alternating
};

typedef void (*x360con_controller_event_cb)(void *cb_ctx, enum x360con_controller_input input, int32_t value);

struct x360con_ctx {
    int init_err;
    uint32_t log_level;

    libusb_device_handle *controller_handle;
    x360con_controller_event_cb event_callback;

    pthread_t worker_thread;
    volatile int running;
    int cleaned_up;

    void *cb_ctx;
};

struct x360con_ctx x360con_init(x360con_controller_event_cb cb, uint32_t log_level);
void x360con_deinit(struct x360con_ctx *ctx);

int x360con_set_led(struct x360con_ctx *ctx, enum x360con_led_pattern pattern);
int x360con_set_rumble(struct x360con_ctx *ctx, uint8_t left_motor, uint8_t right_motor);
