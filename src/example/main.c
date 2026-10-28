#define _DEFAULT_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "logging.h"
#include <x360con/x360con.h>

static volatile sig_atomic_t g_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    g_running = 0;
}

void controller_event_cb(void *cb_ctx, enum x360con_controller_input input, int32_t value) {
    struct x360con_ctx *dev = (struct x360con_ctx *)cb_ctx;

    switch (input) {
        case INPUT_BUTTON_GUIDE:
            x360con_set_led(dev, LED_ALL_BLINK);

            break;

        case INPUT_BUTTON_B:
            if (dev) {
                if (value) { // non zero when pressed. 0 when released
                    x360con_set_rumble(dev, 255, 255);
                } else {
                    x360con_set_rumble(dev, 0, 0);
                }
            }

            break;

        case INPUT_TRIGGER_RT:
            if (value > 0) {
                printf("Right Trigger: %d\n", value);
            }
            break;

        case INPUT_STICK_LEFT_X:
        case INPUT_STICK_LEFT_Y:
            printf("Left stick axis (%d): %d\n", input, value);
            break;

        default:
            break;
    }
}

int main(void) {
    signal(SIGINT, handle_sigint);

    printf("initializing x360con library\n");

    struct x360con_ctx dev = x360con_init(
        controller_event_cb, 
        (1U << LOG_STANDARD) | (1U << LOG_INFO) | (1U << LOG_WARN) | (1U << LOG_ERROR)
    );

    if (dev.init_err != 0) {
        fprintf(stderr, "init failed\n");
        return 1;
    }

    dev.cb_ctx = &dev;

    printf("\n\nExample:\n^C to exit\nHold B to vibrate\nPress Guide to blink LED's\nCheck out the left joystick\nCheck out the right trigger\n\n");

    while (g_running) {
        sleep(999);
    }

    x360con_deinit(&dev);

    return 0;
}
