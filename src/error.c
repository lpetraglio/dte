#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "command/run.h"
#include "config.h"
#include "editor.h"

static char error_buf[256];
static unsigned int nr_errors;
static bool msg_is_error;

void clear_error(void)
{
    error_buf[0] = '\0';
}

void error_msg(const char *format, ...)
{
    int pos = 0;
    if (config_file) {
        if (current_command) {
            pos = snprintf (
                error_buf,
                sizeof(error_buf),
                "%s:%d: %s: ",
                config_file,
                config_line,
                current_command->name
            );
        } else {
            pos = snprintf (
                error_buf,
                sizeof(error_buf),
                "%s:%d: ",
                config_file,
                config_line
            );
        }
    }

    if (pos >= 0 && pos < (sizeof(error_buf) - 3)) {
        va_list ap;
        va_start(ap, format);
        vsnprintf(error_buf + pos, sizeof(error_buf) - pos, format, ap);
        va_end(ap);
    }

    msg_is_error = true;
    nr_errors++;

    if (editor.status != EDITOR_RUNNING) {
        fputs(error_buf, stderr);
        fputc('\n', stderr);
    }
}

void perror_msg(const char *prefix)
{
    error_msg("%s: %s", prefix, strerror(errno));
}

void info_msg(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsnprintf(error_buf, sizeof(error_buf), format, ap);
    va_end(ap);
    msg_is_error = false;
}

const char *get_msg(bool *is_error)
{
    *is_error = msg_is_error;
    return error_buf;
}

unsigned int get_nr_errors(void)
{
    return nr_errors;
}
