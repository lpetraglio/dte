#ifndef BLOCK_ITER_H
#define BLOCK_ITER_H

#include <stdbool.h>
#include <stddef.h>
#include "block.h"
#include "util/list.h"
#include "util/macros.h"
#include "util/string-view.h"
#include "util/unicode.h"

typedef struct {
    Block *blk;
    ListHead *head;
    size_t offset;
} BlockIter;

#define BLOCK_ITER_INIT(head_) { \
    .blk = BLOCK((head_)->next), \
    .head = (head_), \
    .offset = 0 \
}

static inline void block_iter_bof(BlockIter *bi)
{
    bi->blk = BLOCK(bi->head->next);
    bi->offset = 0;
}

static inline void block_iter_eof(BlockIter *bi)
{
    bi->blk = BLOCK(bi->head->prev);
    bi->offset = bi->blk->size;
}

static inline bool block_iter_is_eof(const BlockIter *const bi)
{
    return bi->offset == bi->blk->size && bi->blk->node.next == bi->head;
}

void block_iter_normalize(BlockIter *bi);
size_t block_iter_eat_line(BlockIter *bi);
size_t block_iter_next_line(BlockIter *bi);
size_t block_iter_prev_line(BlockIter *bi);
size_t block_iter_get_char(BlockIter *bi, CodePoint *up);
size_t block_iter_next_char(BlockIter *bi, CodePoint *up);
size_t block_iter_prev_char(BlockIter *bi, CodePoint *up);
size_t block_iter_next_column(BlockIter *bi);
size_t block_iter_prev_column(BlockIter *bi);
size_t block_iter_bol(BlockIter *bi);
size_t block_iter_eol(BlockIter *bi);
void block_iter_back_bytes(BlockIter *bi, size_t count);
void block_iter_skip_bytes(BlockIter *bi, size_t count);
void block_iter_goto_offset(BlockIter *bi, size_t offset);
void block_iter_goto_line(BlockIter *bi, size_t line);
size_t block_iter_get_offset(const BlockIter *bi);
bool block_iter_is_bol(const BlockIter *bi);
char *block_iter_get_bytes(const BlockIter *bi, size_t len);

void fill_line_ref(BlockIter *bi, StringView *line);
void fill_line_nl_ref(BlockIter *bi, StringView *line);
size_t fetch_this_line(const BlockIter *bi, StringView *line);

#endif
