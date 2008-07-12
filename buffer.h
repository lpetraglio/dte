#ifndef BUFFER_H
#define BUFFER_H

#include "common.h"
#include "iter.h"
#include "list.h"
#include "uchar.h"
#include "options.h"

#define VIEW(item) container_of((item), struct view, node)

#define SET_CURSOR(bi) \
	do { \
		view->cursor.blk = bi.blk; \
		view->cursor.offset = bi.offset; \
	} while (0)

struct change_head {
	struct change_head *next;
	struct change_head **prev;
	unsigned int nr_prev;
};

struct change {
	struct change_head head;
	unsigned int offset;
	unsigned int del_count;
	unsigned ins_count : 31;
	// after undoing backspace move after the text
	unsigned move_after : 1;
	// deleted bytes (inserted bytes need not to be saved)
	char *buf;
};

struct buffer {
	struct list_head blocks;
	struct change_head change_head;
	struct change_head *cur_change_head;

	// used to determine if buffer is modified
	struct change_head *save_change_head;

	struct stat st;

	unsigned int nl;

	char *filename;
	char *abs_filename;

	unsigned utf8 : 1;
	unsigned ro : 1;

	enum newline_sequence newline;

	int tab_width;

	unsigned int (*next_char)(struct block_iter *i, uchar *up);
	unsigned int (*prev_char)(struct block_iter *i, uchar *up);
};

struct view {
	struct list_head node;
	struct buffer *buffer;
	struct window *window;

	struct block_iter cursor;

	// cursor y
	int cy;

	// cursor x (wide char 2, tab 1-8, control character 2, invalid char 4)
	int cx;

	// cursor x in characters (invalid utf8 character (byte) is one char)
	int cx_idx;

	// top left corner
	int vx, vy;

	// preferred cursor x (cx)
	int preferred_x;

	// Selection always starts at exact position of cursor and ends to
	// current position of cursor regardless of whether your are selecting
	// lines or not.
	struct block_iter sel;
	unsigned sel_is_lines : 1;
};

enum undo_merge {
	UNDO_MERGE_NONE,
	UNDO_MERGE_INSERT,
	UNDO_MERGE_DELETE,
	UNDO_MERGE_BACKSPACE
};

enum input_mode {
	INPUT_NORMAL,
	INPUT_COMMAND,
	INPUT_SEARCH,
};

// from smallest update to largest. UPDATE_CURSOR_LINE includes
// UPDATE_STATUS_LINE and so on.
#define UPDATE_STATUS_LINE	(1 << 0)
#define UPDATE_CURSOR_LINE	(1 << 1)
#define UPDATE_FULL		(1 << 2)

// buffer = view->buffer = window->view->buffer
extern struct view *view;
extern struct buffer *buffer;

extern enum undo_merge undo_merge;
extern unsigned int update_flags;
extern enum input_mode input_mode;

extern char *line_buffer;
extern size_t line_buffer_len;

static inline int buffer_modified(struct buffer *b)
{
	return b->save_change_head != b->cur_change_head;
}

unsigned int buffer_offset(void);
void move_offset(unsigned int offset);
void record_insert(unsigned int len);
void record_delete(char *buf, unsigned int len, int move_after);
void record_replace(char *deleted, unsigned int del_count, unsigned int ins_count);
void begin_change_chain(void);
void end_change_chain(void);
void undo(void);
void redo(void);

char *buffer_get_bytes(unsigned int *lenp);
void fetch_eol(const struct block_iter *bi);

void update_cursor_x(struct view *v);
void update_cursor(struct view *v);

void update_preferred_x(void);
void do_insert(const char *buf, unsigned int len);
char *do_delete(unsigned int *lenp);
void delete(unsigned int len, int move_after);
void insert(const char *buf, unsigned int len);
void replace(unsigned int del_count, const char *inserted, int ins_count);
void cut(unsigned int len, int is_lines);
void copy(unsigned int len, int is_lines);
unsigned int count_bytes_eol(struct block_iter *bi);
unsigned int prepare_selection(void);

void select_start(int is_lines);
void select_end(void);
void paste(void);
void delete_ch(void);
void backspace(void);
void insert_ch(unsigned int ch);
void join_lines(void);
void erase_word(void);
void shift_lines(int count);

void move_left(int count);
void move_right(int count);
void move_bol(void);
void move_eol(void);
void move_up(int count);
void move_down(int count);
void move_bof(void);
void move_eof(void);

void goto_tag(const char *name);
void push_location(void);
void pop_location(void);

int buffer_get_char(uchar *up);
char *get_word_under_cursor(void);

void error_msg(const char *format, ...) __FORMAT(1, 2);
void info_msg(const char *format, ...) __FORMAT(1, 2);
char get_confirmation(const char *choices, const char *format, ...) __FORMAT(2, 3);

#endif
