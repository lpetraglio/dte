#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tag.h"
#include "completion.h"
#include "util/path.h"
#include "util/str-util.h"
#include "util/xmalloc.h"
#include "util/xreadwrite.h"

static TagFile *current_tag_file;
static char *current_filename; // For sorting tags

static int visibility_cmp(const Tag *a, const Tag *b)
{
    bool a_this_file = false;
    bool b_this_file = false;

    if (!a->local && !b->local) {
        return 0;
    }

    // Is tag visibility limited to the current file?
    if (a->local) {
        a_this_file = current_filename && streq(current_filename, a->filename);
    }
    if (b->local) {
        b_this_file = current_filename && streq(current_filename, b->filename);
    }

    // Tags local to other file than current are not interesting.
    if (a->local && !a_this_file) {
        // a is not interesting
        if (b->local && !b_this_file) {
            // b is equally uninteresting
            return 0;
        }
        // b is more interesting, sort it before a
        return 1;
    }
    if (b->local && !b_this_file) {
        // b is not interesting
        return -1;
    }

    // Both are NOT UNinteresting

    if (a->local && a_this_file) {
        if (b->local && b_this_file) {
            return 0;
        }
        // a is more interesting because it is local symbol
        return -1;
    }
    if (b->local && b_this_file) {
        // b is more interesting because it is local symbol
        return 1;
    }
    return 0;
}

static int kind_cmp(const Tag *a, const Tag *b)
{
    if (a->kind == b->kind) {
        return 0;
    }

    // Struct member (m) is not very interesting.
    if (a->kind == 'm') {
        return 1;
    }
    if (b->kind == 'm') {
        return -1;
    }

    // Global variable (v) is not very interesting.
    if (a->kind == 'v') {
        return 1;
    }
    if (b->kind == 'v') {
        return -1;
    }

    // Struct (s), union (u)
    return 0;
}

static int tag_cmp(const void *ap, const void *bp)
{
    const Tag *a = *(const Tag **)ap;
    const Tag *b = *(const Tag **)bp;

    int ret = visibility_cmp(a, b);
    if (ret) {
        return ret;
    }

    return kind_cmp(a, b);
}

// Find "tags" file from directory path and its parent directories
static int open_tag_file(char *path)
{
    const char tags[] = "tags";

    while (*path) {
        size_t len = strlen(path);
        char *slash = strrchr(path, '/');

        if (slash != path + len - 1) {
            path[len++] = '/';
        }
        memcpy(path + len, tags, sizeof(tags));
        int fd = open(path, O_RDONLY);
        if (fd >= 0) {
            return fd;
        }
        if (errno != ENOENT) {
            return -1;
        }
        *slash = 0;
    }
    errno = ENOENT;
    return -1;
}

static bool tag_file_changed (
    const TagFile *tf,
    const char *filename,
    const struct stat *st
) {
    if (tf->mtime != st->st_mtime) {
        return true;
    }
    return !streq(tf->filename, filename);
}

static void tag_file_free(TagFile *tf)
{
    free(tf->filename);
    free(tf->buf);
    free(tf);
}

TagFile *load_tag_file(void)
{
    char path[4096];
    if (!getcwd(path, sizeof(path) - 5)) { // 5 = length of "/tags"
        return NULL;
    }

    int fd = open_tag_file(path);
    if (fd < 0) {
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) != 0 || st.st_size <= 0) {
        close(fd);
        return NULL;
    }

    if (
        current_tag_file != NULL
        && tag_file_changed(current_tag_file, path, &st)
    ) {
        tag_file_free(current_tag_file);
        current_tag_file = NULL;
    }
    if (current_tag_file != NULL) {
        close(fd);
        return current_tag_file;
    }

    char *buf = xmalloc(st.st_size);
    ssize_t size = xread(fd, buf, st.st_size);
    close(fd);
    if (size < 0) {
        free(buf);
        return NULL;
    }

    TagFile *tf = xnew0(TagFile, 1);
    tf->filename = xstrdup(path);
    tf->buf = buf;
    tf->size = size;
    tf->mtime = st.st_mtime;
    current_tag_file = tf;
    return current_tag_file;
}

static void free_tags_cb(Tag *t)
{
    free_tag(t);
    free(t);
}

void free_tags(PointerArray *tags)
{
    ptr_array_free_cb(tags, FREE_FUNC(free_tags_cb));
}

// Both parameters must be absolute and clean
static char *path_relative(const char *filename, const char *dir)
{
    size_t dlen = strlen(dir);

    if (!str_has_prefix(filename, dir)) {
        return NULL;
    }
    if (filename[dlen] == '\0') {
        // Equal strings
        return xmemdup_literal(".");
    }
    if (filename[dlen] != '/') {
        return NULL;
    }
    return xstrdup(filename + dlen + 1);
}

void tag_file_find_tags (
    const TagFile *tf,
    const char *filename,
    const char *name,
    PointerArray *tags
) {
    Tag *t = xnew(Tag, 1);
    size_t pos = 0;
    while (next_tag(tf, &pos, name, true, t)) {
        ptr_array_append(tags, t);
        t = xnew(Tag, 1);
    }
    free(t);

    if (filename == NULL) {
        current_filename = NULL;
    } else {
        char *dir = path_dirname(tf->filename);
        current_filename = path_relative(filename, dir);
        free(dir);
    }
    ptr_array_sort(tags, tag_cmp);
    free(current_filename);
    current_filename = NULL;
}

char *tag_file_get_tag_filename(const TagFile *tagfile, const Tag *tag)
{
    const StringView dir = path_slice_dirname(tagfile->filename);
    const size_t tag_filename_len = strlen(tag->filename);

    char *filename = xmalloc(dir.length + tag_filename_len + 2);
    memcpy(filename, dir.data, dir.length);
    filename[dir.length] = '/';
    memcpy(filename + dir.length + 1, tag->filename, tag_filename_len + 1);
    return filename;
}

void collect_tags(const TagFile *tf, const char *prefix)
{
    Tag t;
    size_t pos = 0;
    char *prev = NULL;

    while (next_tag(tf, &pos, prefix, false, &t)) {
        if (!prev || !streq(prev, t.name)) {
            add_completion(t.name);
            prev = t.name;
            t.name = NULL;
        }
        free_tag(&t);
    }
}
