// Static trie of switch statements for parsing xterm key sequences.
// Generated by: src/lookup/xterm-keys.lua
// See also: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html

static ssize_t parse_xterm_key_sequence(const char *buf, size_t length, Key *k)
{
    if (length == 0 || buf[0] != '\033') {
        return 0;
    }
    size_t i = 1;
    if (i >= length) return -1;
    switch(buf[i++]) {
    case 'O':
        if (i >= length) return -1;
        switch(buf[i++]) {
        case ' ':
            *k = ' ';
            return 3;
        case 'A':
            *k = KEY_UP;
            return 3;
        case 'B':
            *k = KEY_DOWN;
            return 3;
        case 'C':
            *k = KEY_RIGHT;
            return 3;
        case 'D':
            *k = KEY_LEFT;
            return 3;
        case 'F':
            *k = KEY_END;
            return 3;
        case 'H':
            *k = KEY_HOME;
            return 3;
        case 'I':
            *k = '\t';
            return 3;
        case 'M':
            *k = '\r';
            return 3;
        case 'P':
            *k = KEY_F1;
            return 3;
        case 'Q':
            *k = KEY_F2;
            return 3;
        case 'R':
            *k = KEY_F3;
            return 3;
        case 'S':
            *k = KEY_F4;
            return 3;
        case 'j':
            *k = '*';
            return 3;
        case 'k':
            *k = '+';
            return 3;
        case 'm':
            *k = '-';
            return 3;
        case 'o':
            *k = '/';
            return 3;
        }
        return 0;
    case '[':
        if (i >= length) return -1;
        switch(buf[i++]) {
        case '1':
            if (i >= length) return -1;
            switch(buf[i++]) {
            case '5':
                *k = KEY_F5;
                goto check_trailing_tilde;
            case '7':
                *k = KEY_F6;
                goto check_trailing_tilde;
            case '8':
                *k = KEY_F7;
                goto check_trailing_tilde;
            case '9':
                *k = KEY_F8;
                goto check_trailing_tilde;
            case ';':
                if (i >= length) return -1;
                switch(buf[i++]) {
                case '2':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_SHIFT | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_SHIFT | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_SHIFT | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_SHIFT | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_SHIFT | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_SHIFT | KEY_HOME;
                        return 6;
                    }
                    return 0;
                case '3':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_META | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_META | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_META | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_META | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_META | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_META | KEY_HOME;
                        return 6;
                    }
                    return 0;
                case '4':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_SHIFT | MOD_META | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_SHIFT | MOD_META | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_SHIFT | MOD_META | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_SHIFT | MOD_META | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_SHIFT | MOD_META | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_SHIFT | MOD_META | KEY_HOME;
                        return 6;
                    }
                    return 0;
                case '5':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_CTRL | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_CTRL | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_CTRL | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_CTRL | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_CTRL | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_CTRL | KEY_HOME;
                        return 6;
                    }
                    return 0;
                case '6':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_SHIFT | MOD_CTRL | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_SHIFT | MOD_CTRL | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_SHIFT | MOD_CTRL | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_SHIFT | MOD_CTRL | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_SHIFT | MOD_CTRL | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_SHIFT | MOD_CTRL | KEY_HOME;
                        return 6;
                    }
                    return 0;
                case '7':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_META | MOD_CTRL | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_META | MOD_CTRL | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_META | MOD_CTRL | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_META | MOD_CTRL | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_META | MOD_CTRL | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_META | MOD_CTRL | KEY_HOME;
                        return 6;
                    }
                    return 0;
                case '8':
                    if (i >= length) return -1;
                    switch(buf[i++]) {
                    case 'A':
                        *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_UP;
                        return 6;
                    case 'B':
                        *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_DOWN;
                        return 6;
                    case 'C':
                        *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_RIGHT;
                        return 6;
                    case 'D':
                        *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_LEFT;
                        return 6;
                    case 'F':
                        *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_END;
                        return 6;
                    case 'H':
                        *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_HOME;
                        return 6;
                    }
                    return 0;
                }
                return 0;
            case '~':
                *k = KEY_HOME;
                return 4;
            }
            return 0;
        case '2':
            if (i >= length) return -1;
            switch(buf[i++]) {
            case '0':
                *k = KEY_F9;
                goto check_trailing_tilde;
            case '1':
                *k = KEY_F10;
                goto check_trailing_tilde;
            case '3':
                *k = KEY_F11;
                goto check_trailing_tilde;
            case '4':
                *k = KEY_F12;
                goto check_trailing_tilde;
            case ';':
                if (i >= length) return -1;
                switch(buf[i++]) {
                case '2':
                    *k = MOD_SHIFT | KEY_INSERT;
                    goto check_trailing_tilde;
                case '3':
                    *k = MOD_META | KEY_INSERT;
                    goto check_trailing_tilde;
                case '4':
                    *k = MOD_SHIFT | MOD_META | KEY_INSERT;
                    goto check_trailing_tilde;
                case '5':
                    *k = MOD_CTRL | KEY_INSERT;
                    goto check_trailing_tilde;
                case '6':
                    *k = MOD_SHIFT | MOD_CTRL | KEY_INSERT;
                    goto check_trailing_tilde;
                case '7':
                    *k = MOD_META | MOD_CTRL | KEY_INSERT;
                    goto check_trailing_tilde;
                case '8':
                    *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_INSERT;
                    goto check_trailing_tilde;
                }
                return 0;
            case '~':
                *k = KEY_INSERT;
                return 4;
            }
            return 0;
        case '3':
            if (i >= length) return -1;
            switch(buf[i++]) {
            case ';':
                if (i >= length) return -1;
                switch(buf[i++]) {
                case '2':
                    *k = MOD_SHIFT | KEY_DELETE;
                    goto check_trailing_tilde;
                case '3':
                    *k = MOD_META | KEY_DELETE;
                    goto check_trailing_tilde;
                case '4':
                    *k = MOD_SHIFT | MOD_META | KEY_DELETE;
                    goto check_trailing_tilde;
                case '5':
                    *k = MOD_CTRL | KEY_DELETE;
                    goto check_trailing_tilde;
                case '6':
                    *k = MOD_SHIFT | MOD_CTRL | KEY_DELETE;
                    goto check_trailing_tilde;
                case '7':
                    *k = MOD_META | MOD_CTRL | KEY_DELETE;
                    goto check_trailing_tilde;
                case '8':
                    *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_DELETE;
                    goto check_trailing_tilde;
                }
                return 0;
            case '~':
                *k = KEY_DELETE;
                return 4;
            }
            return 0;
        case '4':
            *k = KEY_END;
            goto check_trailing_tilde;
        case '5':
            if (i >= length) return -1;
            switch(buf[i++]) {
            case ';':
                if (i >= length) return -1;
                switch(buf[i++]) {
                case '2':
                    *k = MOD_SHIFT | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                case '3':
                    *k = MOD_META | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                case '4':
                    *k = MOD_SHIFT | MOD_META | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                case '5':
                    *k = MOD_CTRL | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                case '6':
                    *k = MOD_SHIFT | MOD_CTRL | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                case '7':
                    *k = MOD_META | MOD_CTRL | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                case '8':
                    *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_PAGE_UP;
                    goto check_trailing_tilde;
                }
                return 0;
            case '~':
                *k = KEY_PAGE_UP;
                return 4;
            }
            return 0;
        case '6':
            if (i >= length) return -1;
            switch(buf[i++]) {
            case ';':
                if (i >= length) return -1;
                switch(buf[i++]) {
                case '2':
                    *k = MOD_SHIFT | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                case '3':
                    *k = MOD_META | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                case '4':
                    *k = MOD_SHIFT | MOD_META | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                case '5':
                    *k = MOD_CTRL | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                case '6':
                    *k = MOD_SHIFT | MOD_CTRL | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                case '7':
                    *k = MOD_META | MOD_CTRL | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                case '8':
                    *k = MOD_SHIFT | MOD_META | MOD_CTRL | KEY_PAGE_DOWN;
                    goto check_trailing_tilde;
                }
                return 0;
            case '~':
                *k = KEY_PAGE_DOWN;
                return 4;
            }
            return 0;
        case 'A':
            *k = KEY_UP;
            return 3;
        case 'B':
            *k = KEY_DOWN;
            return 3;
        case 'C':
            *k = KEY_RIGHT;
            return 3;
        case 'D':
            *k = KEY_LEFT;
            return 3;
        case 'F':
            *k = KEY_END;
            return 3;
        case 'H':
            *k = KEY_HOME;
            return 3;
        case 'L':
            *k = KEY_INSERT;
            return 3;
        case 'Z':
            *k = MOD_SHIFT | '\t';
            return 3;
        }
        return 0;
    }
    return 0;
check_trailing_tilde:
    if (i >= length) {
        return -1;
    } else if (buf[i++] == '~') {
        return i;
    }
    return 0;
}
