#ifndef TERM_H
#define TERM_H

#include <stdbool.h>
#include <stddef.h>
#include "key.h"

enum {
    TERMCAP_CLEAR_TO_EOL,
    TERMCAP_KEYPAD_OFF,
    TERMCAP_KEYPAD_ON,
    TERMCAP_CUP_MODE_OFF,
    TERMCAP_CUP_MODE_ON,
    TERMCAP_SHOW_CURSOR,
    TERMCAP_HIDE_CURSOR,

    NR_STR_CAPS
};

enum {
    COLOR_DEFAULT = -1,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_GREY
};

enum {
    ATTR_BOLD = 0x01,
    ATTR_LOW_INTENSITY = 0x02,
    ATTR_ITALIC = 0x04,
    ATTR_UNDERLINE = 0x08,
    ATTR_BLINKING = 0x10,
    ATTR_REVERSE_VIDEO = 0x20,
    ATTR_INVISIBLE_TEXT = 0x40,
    ATTR_KEEP = 0x80,
};

typedef struct {
    Key key;
    const char *code;
} TermKeyMap;

// See terminfo(5)
typedef struct {
    bool back_color_erase; // Can clear to end of line with bg color set
    int max_colors;
    const char *strings[NR_STR_CAPS];
    TermKeyMap keymap[NR_SPECIAL_KEYS + 4];
} TerminalCapabilities;

typedef struct {
    short fg;
    short bg;
    unsigned short attr;
} TermColor;

extern TerminalCapabilities term_cap;

void term_init(const char *const term);
void term_setup_extra_keys(const char *const term);

void term_raw(void);
void term_cooked(void);

bool term_read_key(Key *key);
char *term_read_paste(size_t *size);
void term_discard_paste(void);

int term_get_size(int *w, int *h);

const char *term_set_color(const TermColor *color);

// Move cursor (x and y are zero based)
const char *term_move_cursor(int x, int y);

#endif
