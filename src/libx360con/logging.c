#include "logging.h"
#include <stdarg.h>
#include <stdio.h>
#include "x360con/x360con.h"

static uint32_t *g_log_level = NULL;

void x360con_log_init(struct x360con_ctx *ctx) {
    g_log_level = ctx ? &ctx->log_level : NULL;
}

void x360con_vlog_printf(x360con_log_level ll, FILE *stream, const char *format, va_list args) {
    if (g_log_level && !(*g_log_level & (1U << ll))) {
        return;
    }

    const char *prefix = "";

    switch (ll) {
        case LOG_STANDARD:  prefix = "[ X360CON ] "; break;
        case LOG_INFO:      prefix = "[ X360CON ] INFO:  "; break;
        case LOG_WARN:      prefix = "[ X360CON ] WARN:  "; break;
        case LOG_ERROR:     prefix = "[ X360CON ] ERROR: "; break;
        case LOG_DEBUG:     prefix = "[ X360CON ] DEBUG: "; break;
    }

    fprintf(stream, "%s", prefix);
    vfprintf(stream, format, args);
}

void x360con_log_printf(x360con_log_level ll, FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    x360con_vlog_printf(ll, stream, format, args);
    va_end(args);
}
