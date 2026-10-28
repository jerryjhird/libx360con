#include "protocol/led.h"
#include "protocol/rumble.h"
#include "devel/x360con/x360con.h"

int x360con_set_led(struct x360con_ctx *ctx, enum x360con_led_pattern pattern) {
    if (!ctx || !ctx->controller_handle) { return -1; }

    return _internal_x360con_set_led(ctx->controller_handle, (uint8_t)pattern);
}

int x360con_set_rumble(struct x360con_ctx *ctx, uint8_t left_motor, uint8_t right_motor) {
    if (!ctx || !ctx->controller_handle) { return -1; }

    return _internal_x360con_set_rumble(ctx->controller_handle, left_motor, right_motor);
}
