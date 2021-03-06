#ifndef UTIL_REGEXP_H
#define UTIL_REGEXP_H

#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include "util/string-view.h"

bool regexp_match_nosub(const char *pattern, const StringView *buf);

bool regexp_compile_internal(regex_t *re, const char *pattern, int flags);
bool regexp_exec(const regex_t *re, const char *buf, size_t size, size_t nr_m, regmatch_t *m, int flags);

static inline bool regexp_compile(regex_t *re, const char *pattern, int flags)
{
    return regexp_compile_internal(re, pattern, flags | REG_EXTENDED);
}

static inline bool regexp_is_valid(const char *pattern, int flags)
{
    regex_t re;
    if (!regexp_compile(&re, pattern, flags | REG_NOSUB)) {
        return false;
    }
    regfree(&re);
    return true;
}

static inline bool regexp_compile_basic(regex_t *re, const char *pattern, int flags)
{
    return regexp_compile_internal(re, pattern, flags);
}

#endif
