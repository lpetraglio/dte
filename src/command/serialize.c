#include <string.h>
#include "serialize.h"
#include "util/ascii.h"

void string_append_escaped_arg_sv(String *s, StringView sv, bool escape_tilde)
{
    static const char escmap[32] = {
        [0x07] = 'a', [0x08] = 'b',
        [0x09] = 't', [0x0A] = 'n',
        [0x0B] = 'v', [0x0C] = 'f',
        [0x0D] = 'r', [0x1B] = 'e',
    };

    const char *arg = sv.data;
    size_t len = sv.length;
    if (len == 0) {
        string_append_literal(s, "''");
        return;
    }

    bool has_tilde_slash_prefix = strview_has_prefix(&sv, "~/");
    if (has_tilde_slash_prefix && !escape_tilde) {
        // Print "~/" and skip past it, so it doesn't get quoted
        string_append_literal(s, "~/");
        arg += 2;
        len -= 2;
    }

    bool squote = false;
    for (size_t i = 0; i < len; i++) {
        char c = arg[i];
        switch (c) {
        case ' ':
        case '"':
        case '$':
        case ';':
        case '\\':
            squote = true;
            continue;
        case '\'':
            goto dquote;
        }
        if (ascii_iscntrl(c)) {
            goto dquote;
        }
    }

    if (squote) {
        string_append_byte(s, '\'');
        string_append_buf(s, arg, len);
        string_append_byte(s, '\'');
    } else {
        if (has_tilde_slash_prefix && escape_tilde) {
            string_append_byte(s, '\\');
        }
        string_append_buf(s, arg, len);
    }
    return;

dquote:
    string_append_byte(s, '"');
    for (size_t i = 0; i < len; i++) {
        const unsigned char ch = arg[i];
        if (ch < sizeof(escmap)) {
            if (escmap[ch]) {
                string_append_byte(s, '\\');
                string_append_byte(s, escmap[ch]);
            } else {
                string_sprintf(s, "\\x%02hhX", ch);
            }
        } else if (ch == 0x7F) {
            string_append_literal(s, "\\x7F");
        } else if (ch == '"' || ch == '\\') {
            string_append_byte(s, '\\');
            string_append_byte(s, ch);
        } else {
            string_append_byte(s, ch);
        }
    }
    string_append_byte(s, '"');
}

char *escape_command_arg(const char *arg, bool escape_tilde)
{
    size_t n = strlen(arg);
    String buf = string_new(n + 16);
    string_append_escaped_arg_sv(&buf, string_view(arg, n), escape_tilde);
    return string_steal_cstring(&buf);
}
