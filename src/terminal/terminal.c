#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "ecma48.h"
#include "terminfo.h"
#include "xterm.h"
#include "../common.h"
#include "../util/macros.h"

enum {
    TERM_OTHER,
    TERM_LINUX,
    TERM_SCREEN,
    TERM_ST,
    TERM_TMUX,
    TERM_URXVT,
    TERM_XTERM,
};

static uint8_t get_term_type(const char *term)
{
    static const struct {
        const char name[14];
        uint8_t name_len;
        uint8_t type;
    } builtin_terminals[] = {
        {"xterm", 5, TERM_XTERM},
        {"st", 2, TERM_ST},
        {"stterm", 6, TERM_ST},
        {"tmux", 4, TERM_TMUX},
        {"screen", 6, TERM_SCREEN},
        {"linux", 5, TERM_LINUX},
        {"rxvt-unicode", 12, TERM_URXVT},
    };
    for (size_t i = 0; i < ARRAY_COUNT(builtin_terminals); i++) {
        const size_t n = builtin_terminals[i].name_len;
        if (memcmp(term, builtin_terminals[i].name, n) == 0) {
            if (term[n] == '-' || term[n] == '\0') {
                return builtin_terminals[i].type;
            }
        }
    }
    return TERM_OTHER;
}

NORETURN
void term_init_fail(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    putc('\n', stderr);
    fflush(stderr);
    exit(1);
}

void term_init(void)
{
    const char *const term = getenv("TERM");
    if (term == NULL || term[0] == '\0') {
        term_init_fail("'TERM' not set");
    }

    if (getenv("DTE_FORCE_TERMINFO")) {
        if (term_init_terminfo(term)) {
            return;
        } else {
            term_init_fail("'DTE_FORCE_TERMINFO' set but terminfo not linked");
        }
    }

    switch (get_term_type(term)) {
    case TERM_XTERM:
        terminal = xterm;
        terminal.repeat_byte = &ecma48_repeat_byte;
        break;
    case TERM_ST:
    case TERM_URXVT:
        terminal = xterm;
        break;
    case TERM_TMUX:
    case TERM_SCREEN:
        terminal = xterm;
        terminal.back_color_erase = false;
        break;
    case TERM_LINUX:
        // Use the default Terminal and just change the control codes
        terminal.control_codes.hide_cursor = xterm.control_codes.hide_cursor;
        terminal.control_codes.show_cursor = xterm.control_codes.show_cursor;
        break;
    case TERM_OTHER:
        if (term_init_terminfo(term)) {
            return;
        }
        break;
    }

    const char *colorterm = getenv("COLORTERM");
    if (streq(colorterm, "truecolor")) {
        terminal.color_type = TERM_TRUE_COLOR;
        return;
    }

    if (
        terminal.color_type < TERM_256_COLOR
        && str_has_suffix(term, "256color")
    ) {
        terminal.color_type = TERM_256_COLOR;
    } else if (str_has_suffix(term, "-direct")) {
        terminal.color_type = TERM_TRUE_COLOR;
    }
}