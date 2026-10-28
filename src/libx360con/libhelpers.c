#include <libusb-1.0/libusb.h>
#include "logging.h"

void x360con_libusb_log_callback(libusb_context *ctx, enum libusb_log_level level, const char *str) {
    (void)ctx;
    x360con_log_level ll;

    switch (level) {
        case LIBUSB_LOG_LEVEL_ERROR:   ll = LOG_ERROR; break;
        case LIBUSB_LOG_LEVEL_WARNING: ll = LOG_WARN;  break;
        case LIBUSB_LOG_LEVEL_INFO:    ll = LOG_INFO;  break;
        case LIBUSB_LOG_LEVEL_DEBUG:   ll = LOG_DEBUG; break;
        default:                       ll = LOG_STANDARD; break;
    }

    x360con_log_printf(ll, stderr, "%s", str);
}
