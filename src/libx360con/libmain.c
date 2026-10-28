#include <pthread.h>
#include <libusb-1.0/libusb.h>
#include "logging.h"
#include "libhelpers.h"
#include <string.h>

#include "protocol/message.h"
#include "x360con/x360con.h"

#define VENDOR_ID 0x045e  // microsoft
#define PRODUCT_ID 0x028e  // xbox 360 controller
#define INTERFACE_NUM 0

static inline void dispatch_if_changed(struct x360con_ctx *ctx, enum x360con_controller_input enum_val, int32_t current, int32_t prev) {
    if (current != prev) {
        ctx->event_callback(ctx->cb_ctx, enum_val, current);
    }
}

static void* x360con_worker_loop(void *arg) {
    struct x360con_ctx *ctx = (struct x360con_ctx *)arg;

    // poll for packets from controller
    unsigned char data[64];
    int transferred = 0;
    int r = 0;
    struct xbox360_msg prev_msg = {0};

    while (ctx->running) {
        r = libusb_interrupt_transfer(ctx->controller_handle, 0x81, data, sizeof(data), &transferred, 100);

        if (r == 0 && transferred > 0) {
            if (is_xbox360_msg(data, transferred)) {
                struct xbox360_msg msg;
                memcpy(&msg, data, sizeof(msg));

                if (ctx->event_callback) {
                    dispatch_if_changed(ctx, INPUT_BUTTON_A, (int32_t)msg.a, (int32_t)prev_msg.a);
                    dispatch_if_changed(ctx, INPUT_BUTTON_B, (int32_t)msg.b, (int32_t)prev_msg.b);
                    dispatch_if_changed(ctx, INPUT_BUTTON_X, (int32_t)msg.x, (int32_t)prev_msg.x);
                    dispatch_if_changed(ctx, INPUT_BUTTON_Y, (int32_t)msg.y, (int32_t)prev_msg.y);
                    dispatch_if_changed(ctx, INPUT_BUTTON_LB, (int32_t)msg.lb, (int32_t)prev_msg.lb);
                    dispatch_if_changed(ctx, INPUT_BUTTON_RB, (int32_t)msg.rb, (int32_t)prev_msg.rb);
                    dispatch_if_changed(ctx, INPUT_BUTTON_START, (int32_t)msg.start, (int32_t)prev_msg.start);
                    dispatch_if_changed(ctx, INPUT_BUTTON_BACK, (int32_t)msg.back, (int32_t)prev_msg.back);
                    dispatch_if_changed(ctx, INPUT_BUTTON_GUIDE, (int32_t)msg.guide, (int32_t)prev_msg.guide);
                    dispatch_if_changed(ctx, INPUT_BUTTON_THUMB_L, (int32_t)msg.thumb_l, (int32_t)prev_msg.thumb_l);
                    dispatch_if_changed(ctx, INPUT_BUTTON_THUMB_R, (int32_t)msg.thumb_r, (int32_t)prev_msg.thumb_r);

                    dispatch_if_changed(ctx, INPUT_DPAD_UP, (int32_t)msg.dpad_up, (int32_t)prev_msg.dpad_up);
                    dispatch_if_changed(ctx, INPUT_DPAD_DOWN, (int32_t)msg.dpad_down, (int32_t)prev_msg.dpad_down);
                    dispatch_if_changed(ctx, INPUT_DPAD_LEFT, (int32_t)msg.dpad_left, (int32_t)prev_msg.dpad_left);
                    dispatch_if_changed(ctx, INPUT_DPAD_RIGHT, (int32_t)msg.dpad_right, (int32_t)prev_msg.dpad_right);

                    dispatch_if_changed(ctx, INPUT_TRIGGER_LT, (int32_t)msg.lt, (int32_t)prev_msg.lt);
                    dispatch_if_changed(ctx, INPUT_TRIGGER_RT, (int32_t)msg.rt, (int32_t)prev_msg.rt);

                    dispatch_if_changed(ctx, INPUT_STICK_LEFT_X, (int32_t)msg.x1, (int32_t)prev_msg.x1);
                    dispatch_if_changed(ctx, INPUT_STICK_LEFT_Y, (int32_t)msg.y1, (int32_t)prev_msg.y1);
                    dispatch_if_changed(ctx, INPUT_STICK_RIGHT_X, (int32_t)msg.x2, (int32_t)prev_msg.x2);
                    dispatch_if_changed(ctx, INPUT_STICK_RIGHT_Y, (int32_t)msg.y2, (int32_t)prev_msg.y2);
                }

                // save current state for next comparison
                prev_msg = msg;

            } else {
                x360con_log_printf(LOG_DEBUG, stdout, "??? packet (%d bytes)\n", transferred);
            }
        } else if (r == LIBUSB_ERROR_TIMEOUT) {
            continue;
        } else if (r < 0) {
            x360con_log_printf(LOG_ERROR, stderr, "transfer error: %s\n", libusb_error_name(r));
            break;
        }
    }

    return NULL;
}

struct x360con_ctx x360con_init(x360con_controller_event_cb cb, uint32_t log_level) {
    struct x360con_ctx ctx = {0};
    ctx.event_callback = cb;

    ctx.log_level = log_level;
    x360con_log_init(&ctx);

    int r = libusb_init(NULL);
    if (r < 0) {
        x360con_log_printf(LOG_ERROR, stderr, "init failed: %s\n", libusb_error_name(r));
        ctx.init_err = r;
        return ctx;
    }

    libusb_set_log_cb(NULL, x360con_libusb_log_callback, LIBUSB_LOG_CB_GLOBAL);
    libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

    ctx.controller_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);

    if (ctx.controller_handle == NULL) {
        x360con_log_printf(LOG_ERROR, stderr, "could not find or open xbox 360 controller.\n");
        libusb_exit(NULL);
        ctx.init_err = LIBUSB_ERROR_NO_DEVICE;
        return ctx;
    }

    if (libusb_kernel_driver_active(ctx.controller_handle, INTERFACE_NUM) == 1) {
        r = libusb_detach_kernel_driver(ctx.controller_handle, INTERFACE_NUM);
        if (r < 0) {
            x360con_log_printf(LOG_ERROR, stderr, "failed to detach kernel driver: %s\n", libusb_error_name(r));
            libusb_close(ctx.controller_handle);
            ctx.controller_handle = NULL;
            libusb_exit(NULL);
            ctx.init_err = r;
            return ctx;
        }
    
        x360con_log_printf(LOG_INFO, stdout, "successfully detached kernel driver owning target\n");
    }

    r = libusb_claim_interface(ctx.controller_handle, INTERFACE_NUM);
    if (r < 0) {
        x360con_log_printf(LOG_ERROR, stderr, "failed to claim interface: %s\n", libusb_error_name(r));
        libusb_attach_kernel_driver(ctx.controller_handle, INTERFACE_NUM);
        libusb_close(ctx.controller_handle);
        ctx.controller_handle = NULL;
        libusb_exit(NULL);
        ctx.init_err = r;
        return ctx;
    }

    x360con_log_printf(LOG_INFO, stdout, "successfully claimed interface\n");

    ctx.running = 1;
    if (pthread_create(&ctx.worker_thread, NULL, x360con_worker_loop, &ctx) != 0) {
        x360con_log_printf(LOG_ERROR, stderr, "failed to create worker thread\n");
        libusb_release_interface(ctx.controller_handle, INTERFACE_NUM);
        libusb_attach_kernel_driver(ctx.controller_handle, INTERFACE_NUM);
        libusb_close(ctx.controller_handle);
        ctx.controller_handle = NULL;
        libusb_exit(NULL);
        ctx.running = 0;
        ctx.init_err = -1;
        return ctx;
    }

    ctx.init_err = 0;
    return ctx;
}

void x360con_deinit(struct x360con_ctx *ctx) {
    if (!ctx || ctx->cleaned_up) return;
    ctx->cleaned_up = 1;

    if (ctx->running) {
        ctx->running = 0;
        pthread_join(ctx->worker_thread, NULL);
    }

    if (ctx->controller_handle) {
        libusb_release_interface(ctx->controller_handle, INTERFACE_NUM);
        libusb_attach_kernel_driver(ctx->controller_handle, INTERFACE_NUM);
        
        libusb_close(ctx->controller_handle);
        ctx->controller_handle = NULL;
        libusb_exit(NULL);
        
        x360con_log_printf(LOG_STANDARD, stdout, "kernel driver re attached\n");
    }
}
