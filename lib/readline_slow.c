#include "unp.h"

// Slow version, example only

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
again:
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                // newline is stored, like fgets()
                break;
        } else if (rc == 0) {
            *ptr = 0;
            // EOF, n-1 bytes were read
            return (n - 1);
        } else {
            if (errno == EINTR)
                goto again;
            // error, errno set by read()
            return (-1);
        }
    }

    // null terminate like fgets()
    *ptr = 0;
    return (n);
}
