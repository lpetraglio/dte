#include "term-info.h"
#include "term-write.h"
#include "color.h"
#include "common.h"
#include "lookup/xterm-keys.c"

#define ANSI_ATTRS (ATTR_UNDERLINE | ATTR_REVERSE | ATTR_BLINK | ATTR_BOLD)

static void buf_put_clear_to_eol(void);

TerminalInfo terminal = {
    .max_colors = 8,
    .width = 80,
    .height = 24,
    .attributes = ANSI_ATTRS,
    .ncv_attributes = ATTR_UNDERLINE,
    .parse_key_sequence = &parse_xterm_key_sequence,
    .put_clear_to_eol = &buf_put_clear_to_eol,
    .set_color = &buf_set_color,
    .control_codes = &(TermControlCodes) {
        .reset_colors = "\033[39;49m",
        .reset_attrs = "\033[0m",
        .clear_to_eol = "\033[K"
    }
};

static void buf_put_clear_to_eol(void)
{
    const char *seq = terminal.control_codes->clear_to_eol;
    if (seq) {
        buf_escape(seq);
    }
}

#ifndef TERMINFO_DISABLE

#define KEY(c, k) { \
    .code = (c), \
    .code_length = (sizeof(c) - 1), \
    .key = (k) \
}

#define XKEYS(p, key) \
    KEY(p,     key | MOD_SHIFT), \
    KEY(p "3", key | MOD_META), \
    KEY(p "4", key | MOD_SHIFT | MOD_META), \
    KEY(p "5", key | MOD_CTRL), \
    KEY(p "6", key | MOD_SHIFT | MOD_CTRL), \
    KEY(p "7", key | MOD_META | MOD_CTRL), \
    KEY(p "8", key | MOD_SHIFT | MOD_META | MOD_CTRL)

static struct TermKeyMap {
    const char *code;
    uint32_t code_length;
    Key key;
} keymap[] = {
    KEY("kcuu1", KEY_UP),
    KEY("kcud1", KEY_DOWN),
    KEY("kcub1", KEY_LEFT),
    KEY("kcuf1", KEY_RIGHT),
    KEY("kdch1", KEY_DELETE),
    KEY("kpp", KEY_PAGE_UP),
    KEY("knp", KEY_PAGE_DOWN),
    KEY("khome", KEY_HOME),
    KEY("kend", KEY_END),
    KEY("kich1", KEY_INSERT),
    KEY("kcbt", MOD_SHIFT | '\t'),
    KEY("kf1", KEY_F1),
    KEY("kf2", KEY_F2),
    KEY("kf3", KEY_F3),
    KEY("kf4", KEY_F4),
    KEY("kf5", KEY_F5),
    KEY("kf6", KEY_F6),
    KEY("kf7", KEY_F7),
    KEY("kf8", KEY_F8),
    KEY("kf9", KEY_F9),
    KEY("kf10", KEY_F10),
    KEY("kf11", KEY_F11),
    KEY("kf12", KEY_F12),
    XKEYS("kUP", KEY_UP),
    XKEYS("kDN", KEY_DOWN),
    XKEYS("kLFT", KEY_LEFT),
    XKEYS("kRIT", KEY_RIGHT),
    XKEYS("kDC", KEY_DELETE),
    XKEYS("kPRV", KEY_PAGE_UP),
    XKEYS("kNXT", KEY_PAGE_DOWN),
    XKEYS("kHOM", KEY_HOME),
    XKEYS("kEND", KEY_END),
};

static_assert(ARRAY_COUNT(keymap) == 23 + (9 * 7));

static size_t keymap_length = 0;

static ssize_t parse_key_sequence_from_keymap(const char *buf, size_t fill, Key *key)
{
    bool possibly_truncated = false;
    for (size_t i = 0; i < keymap_length; i++) {
        const struct TermKeyMap *const km = &keymap[i];
        const char *const keycode = km->code;
        const size_t len = km->code_length;
        BUG_ON(keycode == NULL);
        BUG_ON(len == 0);
        if (len > fill) {
            // This might be a truncated escape sequence
            if (
                possibly_truncated == false
                && !memcmp(keycode, buf, fill)
            ) {
                possibly_truncated = true;
            }
            continue;
        }
        if (memcmp(keycode, buf, len)) {
            continue;
        }
        *key = km->key;
        return len;
    }
    return possibly_truncated ? -1 : 0;
}

// These are normally declared in the <curses.h> and <term.h> headers.
// They are not included here because of the insane number of unprefixed
// symbols they declare and because of previous bugs caused by using them.
int setupterm(const char *term, int filedes, int *errret);
int tigetflag(const char *capname);
int tigetnum(const char *capname);
char *tigetstr(const char *capname);
char *tiparm(const char *str, ...);
int tputs(const char *str, int affcnt, int (*putc_fn)(int));

static char *curses_str_cap(const char *const name)
{
    char *str = tigetstr(name);
    if (str == (char *)-1) {
        // Not a string cap (bug?)
        return NULL;
    }
    // NULL = canceled or absent
    return str;
}

static int tputs_putc(int ch)
{
    buf_add_ch(ch);
    return ch;
}

static void tputs_clear_to_eol(void)
{
    const char *seq = terminal.control_codes->clear_to_eol;
    if (seq) {
        tputs(seq, 1, tputs_putc);
    }
}

static inline bool attr_is_set(const TermColor *color, unsigned short attr)
{
    if (!(color->attr & attr)) {
        return false;
    } else if (terminal.ncv_attributes & attr) {
        // Terminal only allows attr when not using colors
        return color->fg == COLOR_DEFAULT && color->bg == COLOR_DEFAULT;
    }
    return true;
}

static void tputs_set_color(const TermColor *color)
{
    static bool done_init;
    static const char *sgr, *setaf, *setab;
    if (!done_init) {
        sgr = curses_str_cap("sgr");
        setaf = curses_str_cap("setaf");
        setab = curses_str_cap("setab");
        done_init = true;
    }

    if (same_color(color, &obuf.color)) {
        return;
    }

    if (sgr) {
        const char *attrs = tiparm (
            sgr,
            0, // p1 = "standout" (unused)
            attr_is_set(color, ATTR_UNDERLINE),
            attr_is_set(color, ATTR_REVERSE),
            attr_is_set(color, ATTR_BLINK),
            attr_is_set(color, ATTR_DIM),
            attr_is_set(color, ATTR_BOLD),
            attr_is_set(color, ATTR_INVIS),
            0, // p8 = "protect" (unused)
            0  // p9 = "altcharset" (unused)
        );
        tputs(attrs, 1, tputs_putc);
    }

    TermColor c = *color;
    if (setaf && c.fg >= 0) {
        const char *seq = tiparm(setaf, (int) c.fg);
        if (seq) {
            tputs(seq, 1, tputs_putc);
        }
    }
    if (setab && c.bg >= 0) {
        const char *seq = tiparm(setab, c.bg);
        if (seq) {
            tputs(seq, 1, tputs_putc);
        }
    }
}

// See terminfo(5)
static void term_read_caps(void)
{
    terminal.parse_key_sequence = &parse_key_sequence_from_keymap;
    terminal.put_clear_to_eol = &tputs_clear_to_eol;
    terminal.set_color = &tputs_set_color;

    terminal.back_color_erase = tigetflag("bce");
    terminal.max_colors = tigetnum("colors");
    terminal.width = tigetnum("cols");
    terminal.height = tigetnum("lines");

    // Not needed or used in terminfo mode
    terminal.attributes = 0;

    const int ncv = tigetnum("ncv");
    if (ncv <= 0) {
        terminal.ncv_attributes = 0;
    } else {
        terminal.ncv_attributes = (unsigned short)ncv;
        // The ATTR_* bitflag values used in this codebase are mostly
        // the same as those used by terminfo, with the exception of
        // ITALIC, which is bit 7 here, but bit 15 in terminfo. It
        // must therefore be manually converted.
        if ((ncv & (1 << 15)) && !(ncv & ATTR_ITALIC)) {
            terminal.ncv_attributes |= ATTR_ITALIC;
        }
    }

    static TermControlCodes tcc;
    tcc = (TermControlCodes) {
        .reset_colors = curses_str_cap("op"),
        .reset_attrs = curses_str_cap("sgr0"),
        .clear_to_eol = curses_str_cap("el"),
        .keypad_off = curses_str_cap("rmkx"),
        .keypad_on = curses_str_cap("smkx"),
        .cup_mode_off = curses_str_cap("rmcup"),
        .cup_mode_on = curses_str_cap("smcup"),
        .show_cursor = curses_str_cap("cnorm"),
        .hide_cursor = curses_str_cap("civis")
    };
    terminal.control_codes = &tcc;

    for (size_t i = 0; i < ARRAY_COUNT(keymap); i++) {
        const char *const code = curses_str_cap(keymap[i].code);
        if (code && code[0] != '\0') {
            keymap[keymap_length++] = (struct TermKeyMap) {
                .code = code,
                .code_length = strlen(code),
                .key = keymap[i].key
            };
        }
    }
}

static void term_init_fallback(const char *const term)
{
    // Initialize terminfo database (or call exit(3) on failure)
    setupterm(term, 1, (int*)0);

    term_read_caps();
}

#else

static void term_init_fallback(const char *const UNUSED_ARG(term))
{
    // Use the default TerminalInfo defined at the top of this file
    // (ECMA-48 + xterm key parser).
}

#endif // ifndef TERMINFO_DISABLE

static const TerminalInfo terminal_xterm = {
    .back_color_erase = true,
    .max_colors = 8,
    .width = 80,
    .height = 24,
    .attributes = ANSI_ATTRS | ATTR_INVIS | ATTR_DIM | ATTR_ITALIC,
    .parse_key_sequence = &parse_xterm_key_sequence,
    .put_clear_to_eol = &buf_put_clear_to_eol,
    .set_color = &buf_set_color,
    .control_codes = &(TermControlCodes) {
        // https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
        .reset_colors = "\033[39;49m",
        .reset_attrs = "\033[0m",
        .clear_to_eol = "\033[K",
        .keypad_off = "\033[?1l\033>",
        .keypad_on = "\033[?1h\033=",
        .cup_mode_off = "\033[?1049l",
        .cup_mode_on = "\033[?1049h",
        .hide_cursor = "\033[?25l",
        .show_cursor = "\033[?25h",
        .save_title = "\033[22;2t",
        .restore_title = "\033[23;2t",
        .set_title_begin = "\033]2;",
        .set_title_end = "\007",
    }
};

static bool term_match(const char *term, const char *prefix)
{
    if (str_has_prefix(term, prefix) == false) {
        return false;
    }
    switch (term[strlen(prefix)]) {
    case '\0': // Exact match
    case  '-': // Prefix match
        return true;
    }
    return false;
}

void term_init(void)
{
    const char *const term = getenv("TERM");
    if (term == NULL || term[0] == '\0') {
        fputs("'TERM' not set\n", stderr);
        exit(1);
    }

    if (getenv("DTE_FORCE_TERMINFO")) {
#ifndef TERMINFO_DISABLE
        term_init_fallback(term);
        return;
#else
        fputs("'DTE_FORCE_TERMINFO' set but terminfo not linked\n", stderr);
        exit(1);
#endif
    }

    if (term_match(term, "xterm")) {
        terminal = terminal_xterm;
        terminal.control_codes->init =
            "\033[?1036s" // Save "metaSendsEscape"
            "\033[?1036h" // Enable "metaSendsEscape"
        ;
        terminal.control_codes->deinit =
            "\033[?1036r" // Restore "metaSendsEscape"
        ;
    } else if (
        term_match(term, "st")
        || term_match(term, "stterm")
    ) {
        terminal = terminal_xterm;
    } else if (
        term_match(term, "tmux")
        || term_match(term, "screen")
    ) {
        terminal = terminal_xterm;
        terminal.back_color_erase = false;
    } else if (streq(term, "linux")) {
        // Use the default TerminalInfo and just change the control codes
        terminal.control_codes->hide_cursor = "\033[?25l\033[?1c";
        terminal.control_codes->show_cursor = "\033[?25h\033[?0c";
    } else {
        term_init_fallback(term);
    }

    if (str_has_suffix(term, "256color")) {
        terminal.max_colors = 256;
    }
}