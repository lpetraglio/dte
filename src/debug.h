#ifndef DEBUG_H
#define DEBUG_H

#include "util/macros.h"

#if DEBUG >= 1
  #define BUG(...) bug(__FILE__, __LINE__, __func__, __VA_ARGS__)
  #define BUG_ON(a) do {if (unlikely(a)) {BUG("%s", #a);}} while (0)
  #define DEBUG_VAR(x)

  NORETURN COLD PRINTF(4)
  void bug(const char *file, int line, const char *func, const char *fmt, ...);
#else
  #define BUG_ON(a)
  #define DEBUG_VAR(x) (void)(x)

  PRINTF(1)
  static inline void BUG(const char* UNUSED_ARG(fmt), ...) {}
#endif

#ifdef DEBUG_PRINT
  #define d_print(...) debug_print(__func__, __VA_ARGS__)

  PRINTF(2)
  void debug_print(const char *function, const char *fmt, ...);
#else
  PRINTF(1)
  static inline void d_print(const char* UNUSED_ARG(fmt), ...) {}
#endif

void term_cleanup(void);

#endif
