#pragma once

#include <libusb-1.0/libusb.h>

void x360con_libusb_log_callback(libusb_context *ctx, enum libusb_log_level level, const char *str);
