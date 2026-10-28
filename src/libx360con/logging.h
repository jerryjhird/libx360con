#pragma once

#include <stdio.h>
#include <stdarg.h>

#include "x360con/x360con.h"

typedef enum {
    LOG_STANDARD,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DEBUG
} x360con_log_level;

void x360con_log_init(struct x360con_ctx *ctx);

void x360con_vlog_printf(x360con_log_level ll, FILE *stream, const char *format, va_list args);
void x360con_log_printf(x360con_log_level ll, FILE *stream, const char *format, ...);
