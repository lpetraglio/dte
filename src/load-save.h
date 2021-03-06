#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#include <stdbool.h>
#include "buffer.h"
#include "encoding/encoding.h"

int load_buffer(Buffer *b, bool must_exist, const char *filename);
int save_buffer(Buffer *b, const char *filename, const Encoding *encoding, bool crlf);
int read_blocks(Buffer *b, int fd);

#endif
