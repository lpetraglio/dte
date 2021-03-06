#include "ecma48.h"
#include "output.h"
#include "util/ascii.h"
#include "util/macros.h"

void ecma48_clear_screen(void)
{
    term_add_literal (
        "\033[0m" // Reset colors and attributes
        "\033[H"  // Move cursor to 1,1 (done only to mimic terminfo(5) "clear")
        "\033[2J" // Clear whole screen (regardless of cursor position)
    );
}

void ecma48_clear_to_eol(void)
{
    term_add_literal("\033[K");
}

void ecma48_move_cursor(unsigned int x, unsigned int y)
{
    term_xnprintf(64, "\033[%u;%uH", y + 1, x + 1);
}

void ecma48_repeat_byte(char ch, size_t count)
{
    if (!ascii_isprint(ch) || count < 6 || count > 30000) {
        term_repeat_byte(ch, count);
        return;
    }
    term_xnprintf(16, "%c\033[%zub", ch, count - 1);
}

static void do_set_color(int32_t color, char ch)
{
    if (color < 0) {
        return;
    }

    if (color < 8) {
        term_add_byte(';');
        term_add_byte(ch);
        term_add_byte('0' + color);
    } else if (color < 256) {
        term_xnprintf(16, ";%c8;5;%hhu", ch, (uint8_t)color);
    } else {
        uint8_t r, g, b;
        color_split_rgb(color, &r, &g, &b);
        term_xnprintf(32, ";%c8;2;%hhu;%hhu;%hhu", ch, r, g, b);
    }
}

void ecma48_set_color(const TermColor *color)
{
    static const struct {
        char code;
        unsigned int attr;
    } attr_map[] = {
        {'1', ATTR_BOLD},
        {'2', ATTR_DIM},
        {'3', ATTR_ITALIC},
        {'4', ATTR_UNDERLINE},
        {'5', ATTR_BLINK},
        {'7', ATTR_REVERSE},
        {'8', ATTR_INVIS},
        {'9', ATTR_STRIKETHROUGH}
    };

    term_add_literal("\033[0");

    for (size_t i = 0; i < ARRAY_COUNT(attr_map); i++) {
        if (color->attr & attr_map[i].attr) {
            term_add_byte(';');
            term_add_byte(attr_map[i].code);
        }
    }

    do_set_color(color->fg, '3');
    do_set_color(color->bg, '4');
    term_add_byte('m');
}
