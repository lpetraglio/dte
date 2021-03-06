#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "readfile.h"
#include "xreadwrite.h"

ssize_t read_file(const char *filename, char **bufp)
{
    *bufp = NULL;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        close(fd);
        errno = EISDIR;
        return -1;
    }

    char *buf = malloc(st.st_size + 1);
    if (unlikely(!buf)) {
        close(fd);
        return -1;
    }

    ssize_t r = xread(fd, buf, st.st_size);
    close(fd);
    if (r > 0) {
        buf[r] = '\0';
        *bufp = buf;
    } else {
        free(buf);
    }
    return r;
}
