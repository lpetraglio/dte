#include <errno.h>
#include "convert.h"
#include "debug.h"
#include "util/macros.h"

#ifndef ICONV_DISABLE

#include <iconv.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "encoding.h"
#include "util/ascii.h"
#include "util/str-util.h"
#include "util/utf8.h"
#include "util/xmalloc.h"

static unsigned char replacement[2] = "\xc2\xbf"; // U+00BF

struct cconv {
    iconv_t cd;

    char *obuf;
    size_t osize;
    size_t opos;

    size_t consumed;
    size_t errors;

    // Temporary input buffer
    char tbuf[16];
    size_t tcount;

    // Replacement character 0xBF (inverted question mark)
    char rbuf[4];
    size_t rcount;

    // Input character size in bytes. zero for UTF-8.
    size_t char_size;
};

static struct cconv *create(iconv_t cd)
{
    struct cconv *c = xnew0(struct cconv, 1);
    c->cd = cd;
    c->osize = 8192;
    c->obuf = xmalloc(c->osize);
    return c;
}

static size_t encoding_char_size(const char *encoding)
{
    if (str_has_prefix(encoding, "UTF-16")) {
        return 2;
    }
    if (str_has_prefix(encoding, "UTF-32")) {
        return 4;
    }
    return 1;
}

static size_t iconv_wrapper (
    iconv_t cd,
    char **restrict inbuf,
    size_t *restrict inbytesleft,
    char **restrict outbuf,
    size_t *restrict outbytesleft
) {
    // POSIX defines the second parameter of iconv(3) as "char **restrict"
    // but NetBSD declares it as "const char **restrict".
    #ifdef __NetBSD__
     #if HAS_WARNING("-Wincompatible-pointer-types-discards-qualifiers")
      IGNORE_WARNING("-Wincompatible-pointer-types-discards-qualifiers")
     #else
      IGNORE_WARNING("-Wincompatible-pointer-types")
     #endif
    #endif

    return iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);

    #ifdef __NetBSD__
     UNIGNORE_WARNINGS
    #endif
}

static void encode_replacement(struct cconv *c)
{
    char *ib = replacement;
    char *ob = c->rbuf;
    size_t ic = sizeof(replacement);
    size_t oc = sizeof(c->rbuf);
    size_t rc = iconv_wrapper(c->cd, &ib, &ic, &ob, &oc);

    if (rc == (size_t)-1) {
        c->rbuf[0] = '\xbf';
        c->rcount = 1;
    } else {
        c->rcount = ob - c->rbuf;
    }
}

static void resize_obuf(struct cconv *c)
{
    c->osize *= 2;
    xrenew(c->obuf, c->osize);
}

static void add_replacement(struct cconv *c)
{
    if (c->osize - c->opos < 4) {
        resize_obuf(c);
    }

    memcpy(c->obuf + c->opos, c->rbuf, c->rcount);
    c->opos += c->rcount;
}

static size_t handle_invalid(struct cconv *c, const char *buf, size_t count)
{
    DEBUG_LOG("%zu %zu\n", c->char_size, count);
    add_replacement(c);
    if (c->char_size == 0) {
        // Converting from UTF-8
        size_t idx = 0;
        CodePoint u = u_get_char(buf, count, &idx);
        DEBUG_LOG("U+%04" PRIX32 "\n", u);
        return idx;
    }
    if (c->char_size > count) {
        // wtf
        return 1;
    }
    return c->char_size;
}

static int xiconv(struct cconv *c, char **ib, size_t *ic)
{
    while (1) {
        char *ob = c->obuf + c->opos;
        size_t oc = c->osize - c->opos;
        size_t rc = iconv_wrapper(c->cd, ib, ic, &ob, &oc);

        c->opos = ob - c->obuf;
        if (rc == (size_t)-1) {
            switch (errno) {
            case EILSEQ:
                c->errors++;
                // Reset
                iconv(c->cd, NULL, NULL, NULL, NULL);
                return errno;
            case EINVAL:
                return errno;
            case E2BIG:
                resize_obuf(c);
                continue;
            default:
                BUG("iconv: %s", strerror(errno));
            }
        } else {
            c->errors += rc;
        }
        return 0;
    }
}

static size_t convert_incomplete(struct cconv *c, const char *input, size_t len)
{
    size_t ic, ipos = 0;
    char *ib;

    while (c->tcount < sizeof(c->tbuf) && ipos < len) {
        size_t skip;

        c->tbuf[c->tcount++] = input[ipos++];

        ib = c->tbuf;
        ic = c->tcount;
        int rc = xiconv(c, &ib, &ic);

        if (ic > 0) {
            memmove(c->tbuf, ib, ic);
        }
        c->tcount = ic;

        switch (rc) {
        case EINVAL:
            // Incomplete character at end of input buffer.
            // Try again with more input data.
            continue;
        case EILSEQ:
            // Invalid multibyte sequence
            skip = handle_invalid(c, c->tbuf, c->tcount);
            c->tcount -= skip;
            if (c->tcount > 0) {
                DEBUG_LOG("tcount=%zu, skip=%zu\n", c->tcount, skip);
                memmove(c->tbuf, c->tbuf + skip, c->tcount);
                continue;
            }
            return ipos;
        }
        break;
    }
    DEBUG_LOG("%zu %zu\n", ipos, c->tcount);
    return ipos;
}

void cconv_process(struct cconv *c, const char *input, size_t len)
{
    if (c->consumed > 0) {
        size_t fill = c->opos - c->consumed;
        memmove(c->obuf, c->obuf + c->consumed, fill);
        c->opos = fill;
        c->consumed = 0;
    }

    if (c->tcount > 0) {
        size_t ipos = convert_incomplete(c, input, len);
        input += ipos;
        len -= ipos;
    }

    char *ib = (char *)input;
    size_t ic = len;
    while (ic > 0) {
        size_t skip;

        switch (xiconv(c, &ib, &ic)) {
        case EINVAL:
            // Incomplete character at end of input buffer.
            if (ic < sizeof(c->tbuf)) {
                memcpy(c->tbuf, ib, ic);
                c->tcount = ic;
            } else {
                // FIXME
            }
            ic = 0;
            break;
        case EILSEQ:
            // Invalid multibyte sequence.
            skip = handle_invalid(c, ib, ic);
            ic -= skip;
            ib += skip;
            break;
        }
    }
}

struct cconv *cconv_to_utf8(const char *encoding)
{
    iconv_t cd = iconv_open("UTF-8", encoding);
    if (cd == (iconv_t)-1) {
        return NULL;
    }
    struct cconv *c = create(cd);
    memcpy(c->rbuf, replacement, sizeof(replacement));
    c->rcount = sizeof(replacement);
    c->char_size = encoding_char_size(encoding);
    return c;
}

struct cconv *cconv_from_utf8(const char *encoding)
{
    iconv_t cd = iconv_open(encoding, "UTF-8");
    if (cd == (iconv_t)-1) {
        return NULL;
    }
    struct cconv *c = create(cd);
    encode_replacement(c);
    return c;
}

void cconv_flush(struct cconv *c)
{
    if (c->tcount > 0) {
        // Replace incomplete character at end of input buffer.
        DEBUG_LOG("incomplete character at EOF\n");
        add_replacement(c);
        c->tcount = 0;
    }
}

size_t cconv_nr_errors(const struct cconv *c)
{
    return c->errors;
}

char *cconv_consume_line(struct cconv *c, size_t *len)
{
    char *line = c->obuf + c->consumed;
    char *nl = memchr(line, '\n', c->opos - c->consumed);

    if (nl == NULL) {
        *len = 0;
        return NULL;
    }

    size_t n = nl - line + 1;
    c->consumed += n;
    *len = n;
    return line;
}

char *cconv_consume_all(struct cconv *c, size_t *len)
{
    char *buf = c->obuf + c->consumed;
    *len = c->opos - c->consumed;
    c->consumed = c->opos;
    return buf;
}

void cconv_free(struct cconv *c)
{
    iconv_close(c->cd);
    free(c->obuf);
    free(c);
}

bool encoding_supported_by_iconv(const char *encoding)
{
    if (encoding[0] == '\0') {
        return false;
    }
    iconv_t cd = iconv_open("UTF-8", encoding);
    if (cd == (iconv_t) -1) {
        return false;
    }
    iconv_close(cd);
    return true;
}

#else // Not using iconv -- replace conversion routines with stubs

DISABLE_WARNING("-Wunused-parameter")

bool encoding_supported_by_iconv(const char *encoding)
{
    return false;
}

struct cconv *cconv_to_utf8(const char *encoding)
{
    errno = EILSEQ;
    return NULL;
}

struct cconv *cconv_from_utf8(const char *encoding)
{
    errno = EILSEQ;
    return NULL;
}

#define FAIL() BUG("unsupported"); fatal_error(__func__, ENOSYS)

void cconv_process(struct cconv *c, const char *input, size_t len) {FAIL();}
void cconv_flush(struct cconv *c) {FAIL();}
size_t cconv_nr_errors(const struct cconv *c) {FAIL();}
char *cconv_consume_line(struct cconv *c, size_t *len) {FAIL();}
char *cconv_consume_all(struct cconv *c, size_t *len) {FAIL();}
void cconv_free(struct cconv *c) {FAIL();}

#endif
