#include <string.h>
#include "test.h"
#include "../src/terminal/key.h"
#include "../src/terminal/xterm.h"

static void test_xterm_parse_key(void)
{
    static const struct xterm_key_test {
        const char *escape_sequence;
        ssize_t expected_length;
        KeyCode expected_key;
    } tests[] = {
        {"\033[Z", 3, MOD_SHIFT | '\t'},
        {"\033[1;2A", 6, MOD_SHIFT | KEY_UP},
        {"\033[1;2B", 6, MOD_SHIFT | KEY_DOWN},
        {"\033[1;2C", 6, MOD_SHIFT | KEY_RIGHT},
        {"\033[1;2D", 6, MOD_SHIFT | KEY_LEFT},
        {"\033[1;2F", 6, MOD_SHIFT | KEY_END},
        {"\033[1;2H", 6, MOD_SHIFT | KEY_HOME},
        {"\033[1;8H", 6, MOD_SHIFT | MOD_META | MOD_CTRL | KEY_HOME},
        {"\033[", -1, 0},
        {"\033]", 0, 0},
        {"\033[1", -1, 0},
        {"\033[9", 0, 0},
        {"\033[1;", -1, 0},
        {"\033[1[", 0, 0},
        {"\033[1;2", -1, 0},
        {"\033[1;8", -1, 0},
        {"\033[1;9", 0, 0},
        {"\033[1;_", 0, 0},
        {"\033[1;8Z", 0, 0},
        {"\033O", -1, 0},
        {"\033[\033", 0, 0},
        {"\033[A", 3, KEY_UP},
        {"\033[B", 3, KEY_DOWN},
        {"\033[C", 3, KEY_RIGHT},
        {"\033[D", 3, KEY_LEFT},
        {"\033[F", 3, KEY_END},
        {"\033[H", 3, KEY_HOME},
        {"\033[L", 3, KEY_INSERT},
        {"\033[1~", 4, KEY_HOME},
        {"\033[2~", 4, KEY_INSERT},
        {"\033[3~", 4, KEY_DELETE},
        {"\033[4~", 4, KEY_END},
        {"\033[5~", 4, KEY_PAGE_UP},
        {"\033[6~", 4, KEY_PAGE_DOWN},
        {"\033O ", 3, ' '},
        {"\033OA", 3, KEY_UP},
        {"\033OB", 3, KEY_DOWN},
        {"\033OC", 3, KEY_RIGHT},
        {"\033OD", 3, KEY_LEFT},
        {"\033OF", 3, KEY_END},
        {"\033OH", 3, KEY_HOME},
        {"\033OI", 3, '\t'},
        {"\033OM", 3, '\r'},
        {"\033OP", 3, KEY_F1},
        {"\033OQ", 3, KEY_F2},
        {"\033OR", 3, KEY_F3},
        {"\033OS", 3, KEY_F4},
        {"\033OX", 3, '='},
        {"\033Oj", 3, '*'},
        {"\033Ok", 3, '+'},
        {"\033Ol", 3, ','},
        {"\033Om", 3, '-'},
        {"\033Oo", 3, '/'},
        {"\033[11~", 5, KEY_F1},
        {"\033[12~", 5, KEY_F2},
        {"\033[13~", 5, KEY_F3},
        {"\033[14~", 5, KEY_F4},
        {"\033[15~", 5, KEY_F5},
        {"\033[17~", 5, KEY_F6},
        {"\033[18~", 5, KEY_F7},
        {"\033[19~", 5, KEY_F8},
        {"\033[20~", 5, KEY_F9},
        {"\033[21~", 5, KEY_F10},
        {"\033[23~", 5, KEY_F11},
        {"\033[24~", 5, KEY_F12},
        {"\033[6;3~", 6, MOD_META | KEY_PAGE_DOWN},
        {"\033[6;5~", 6, MOD_CTRL | KEY_PAGE_DOWN},
        {"\033[6;8~", 6, MOD_SHIFT | MOD_META | MOD_CTRL | KEY_PAGE_DOWN},
        {"\033[[A", 4, KEY_F1},
        {"\033[[B", 4, KEY_F2},
        {"\033[[C", 4, KEY_F3},
        {"\033[[D", 4, KEY_F4},
        {"\033[[E", 4, KEY_F5},
    };
    FOR_EACH_I(i, tests) {
        const char *seq = tests[i].escape_sequence;
        KeyCode key;
        ssize_t length = xterm_parse_key(seq, strlen(seq), &key);
        IEXPECT_EQ(length, tests[i].expected_length, i, "lengths");
        if (length > 0) {
            IEXPECT_EQ(key, tests[i].expected_key, i, "keys");
        }
    }
}

static void test_xterm_parse_key_combo(void)
{
    static const struct {
        char escape_sequence[8];
        KeyCode key;
    } templates[] = {
        {"\033[1;_A", KEY_UP},
        {"\033[1;_B", KEY_DOWN},
        {"\033[1;_C", KEY_RIGHT},
        {"\033[1;_D", KEY_LEFT},
        {"\033[1;_F", KEY_END},
        {"\033[1;_H", KEY_HOME},
        {"\033[2;_~", KEY_INSERT},
        {"\033[3;_~", KEY_DELETE},
        {"\033[5;_~", KEY_PAGE_UP},
        {"\033[6;_~", KEY_PAGE_DOWN},
        {"\033[1;_P", KEY_F1},
        {"\033[1;_Q", KEY_F2},
        {"\033[1;_R", KEY_F3},
        {"\033[1;_S", KEY_F4},
        {"\033[15;_~", KEY_F5},
        {"\033[17;_~", KEY_F6},
        {"\033[18;_~", KEY_F7},
        {"\033[19;_~", KEY_F8},
        {"\033[20;_~", KEY_F9},
        {"\033[21;_~", KEY_F10},
        {"\033[23;_~", KEY_F11},
        {"\033[24;_~", KEY_F12},
    };

    static const struct {
        char ch;
        KeyCode mask;
    } modifiers[] = {
        {'2', MOD_SHIFT},
        {'3', MOD_META},
        {'4', MOD_SHIFT | MOD_META},
        {'5', MOD_CTRL},
        {'6', MOD_SHIFT | MOD_CTRL},
        {'7', MOD_META | MOD_CTRL},
        {'8', MOD_SHIFT | MOD_META | MOD_CTRL}
    };

    FOR_EACH_I(i, templates) {
        FOR_EACH_I(j, modifiers) {
            char seq[8];
            memcpy(seq, templates[i].escape_sequence, 8);
            BUG_ON(seq[7] != '\0');
            char *underscore = strchr(seq, '_');
            BUG_ON(underscore == NULL);
            *underscore = modifiers[j].ch;
            size_t seq_length = strlen(seq);
            KeyCode key;
            ssize_t parsed_length = xterm_parse_key(seq, seq_length, &key);
            EXPECT_EQ(parsed_length, seq_length);
            EXPECT_EQ(key, modifiers[j].mask | templates[i].key);
        }
    }
}

static void test_key_to_string(void)
{
    static const struct key_to_string_test {
        const char *str;
        KeyCode key;
    } tests[] = {
        {"a", 'a'},
        {"Z", 'Z'},
        {"0", '0'},
        {"{", '{'},
        {"space", ' '},
        {"enter", KEY_ENTER},
        {"tab", '\t'},
        {"insert", KEY_INSERT},
        {"delete", KEY_DELETE},
        {"home", KEY_HOME},
        {"end", KEY_END},
        {"pgup", KEY_PAGE_UP},
        {"pgdown", KEY_PAGE_DOWN},
        {"left", KEY_LEFT},
        {"right", KEY_RIGHT},
        {"up", KEY_UP},
        {"down", KEY_DOWN},
        {"C-A", MOD_CTRL | 'A'},
        {"M-S-{", MOD_META | MOD_SHIFT | '{'},
        {"C-S-A", MOD_CTRL | MOD_SHIFT | 'A'},
        {"F1", KEY_F1},
        {"F12", KEY_F12},
        {"M-enter", MOD_META | KEY_ENTER},
        {"M-space", MOD_META | ' '},
        {"S-tab", MOD_SHIFT | '\t'},
        {"C-M-S-F12", MOD_CTRL | MOD_META | MOD_SHIFT | KEY_F12},
        {"C-M-S-up", MOD_CTRL | MOD_META | MOD_SHIFT | KEY_UP},
        {"C-M-delete", MOD_CTRL | MOD_META | KEY_DELETE},
        {"C-home", MOD_CTRL | KEY_HOME},
    };
    FOR_EACH_I(i, tests) {
        char *str = key_to_string(tests[i].key);
        EXPECT_STREQ(str, tests[i].str);
        free(str);
    }
}

static void test_key_to_ctrl(void)
{
    unsigned char c;
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | '@', &c), true);
    EXPECT_EQ(c, '\0');
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | '_', &c), true);
    EXPECT_EQ(c, 0x1F);
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 'G', &c), true);
    EXPECT_EQ(c, '\a');
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 'I', &c), true);
    EXPECT_EQ(c, '\t');
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 'J', &c), true);
    EXPECT_EQ(c, '\n');
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 'M', &c), true);
    EXPECT_EQ(c, '\r');
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | '?', &c), true);
    EXPECT_EQ(c, 0x7F);

    EXPECT_EQ(key_to_ctrl(MOD_CTRL | ' ', &c), false);
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 'g', &c), false);
    EXPECT_EQ(key_to_ctrl(MOD_META | '@', &c), false);
    EXPECT_EQ(key_to_ctrl(MOD_SHIFT | '_', &c), false);
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | MOD_SHIFT | 'M', &c), false);
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 0x80, &c), false);
    EXPECT_EQ(key_to_ctrl(MOD_CTRL | 0xFF, &c), false);
    EXPECT_EQ(key_to_ctrl('@', &c), false);
    EXPECT_EQ(key_to_ctrl('\0', &c), false);
    EXPECT_EQ(key_to_ctrl('_', &c), false);
    EXPECT_EQ(key_to_ctrl('M', &c), false);
    EXPECT_EQ(key_to_ctrl(0x80, &c), false);
    EXPECT_EQ(key_to_ctrl(0xFF, &c), false);
}

void test_terminal(void)
{
    test_xterm_parse_key();
    test_xterm_parse_key_combo();
    test_key_to_string();
    test_key_to_ctrl();
}