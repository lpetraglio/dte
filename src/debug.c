#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "debug.h"
#include "editor.h"
#include "terminal/terminal.h"
#include "util/xreadwrite.h"
#include "util/xsnprintf.h"

void term_cleanup(void)
{
    if (!editor.child_controls_terminal) {
        ui_end();
    }
}

noreturn void fatal_error(const char *msg, int err)
{
    term_cleanup();
    errno = err;
    perror(msg);
    abort();
}

#if DEBUG >= 1
noreturn
void bug(const char *file, int line, const char *func, const char *fmt, ...)
{
    term_cleanup();
    fprintf(stderr, "\n%s:%d: **BUG** in %s() function: '", file, line, func);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputs("'\n", stderr);
    fflush(stderr);
    abort();
}
#endif

#ifdef DEBUG_PRINT
void debug_log(const char *function, const char *fmt, ...)
{
    static int fd = -1;
    if (fd < 0) {
        char *filename = editor_file("debug.log");
        fd = open(filename, O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0666);
        free(filename);
        BUG_ON(fd < 0);
    }

    char buf[4096];
    size_t write_max = ARRAY_COUNT(buf);
    const size_t len1 = xsnprintf(buf, write_max, "%s: ", function);
    write_max -= len1;

    va_list ap;
    va_start(ap, fmt);
    const size_t len2 = xvsnprintf(buf + len1, write_max, fmt, ap);
    va_end(ap);

    xwrite(fd, buf, len1 + len2);
}
#endif
