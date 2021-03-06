#include <stdlib.h>
#include "regexp.h"
#include "debug.h"
#include "error.h"
#include "util/macros.h"
#include "util/xmalloc.h"

bool regexp_match_nosub(const char *pattern, const StringView *buf)
{
    regex_t re;
    bool compiled = regexp_compile(&re, pattern, REG_NEWLINE | REG_NOSUB);
    BUG_ON(!compiled);
    regmatch_t m;
    bool ret = regexp_exec(&re, buf->data, buf->length, 1, &m, 0);
    regfree(&re);
    return ret;
}

bool regexp_compile_internal(regex_t *re, const char *pattern, int flags)
{
    int err = regcomp(re, pattern, flags);
    if (err) {
        char msg[1024];
        regerror(err, re, msg, sizeof(msg));
        error_msg("%s: %s", msg, pattern);
        return false;
    }
    return true;
}

bool regexp_exec (
    const regex_t *re,
    const char *buf,
    size_t size,
    size_t nr_m,
    regmatch_t *m,
    int flags
) {
    BUG_ON(!nr_m);
// ASan/MSan don't seem to take REG_STARTEND into account
#if defined(REG_STARTEND) && !defined(ASAN_ENABLED) && !defined(MSAN_ENABLED)
    m[0].rm_so = 0;
    m[0].rm_eo = size;
    return !regexec(re, buf, nr_m, m, flags | REG_STARTEND);
#else
    // Buffer must be null-terminated if REG_STARTEND isn't supported
    char *tmp = xstrcut(buf, size);
    int ret = !regexec(re, tmp, nr_m, m, flags);
    free(tmp);
    return ret;
#endif
}
