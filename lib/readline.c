#include "unp.h"

// By using static variables to maintain the state information across successive
// calls, the functions are not re-entrant or thread-safe.
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr)
{
    if (read_cnt <= 0) {
again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return (-1);
        } else if (read_cnt == 0) {
            return 0;          
        }
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return (1);
}

// readline() can be problematic. System functions like select() still won't know
// about readline's internal buffer, so a carelessly written program could easily
// find itself waiting in select for data already received and stored in readline's
// buffers. For that matter, mixing readn() and readline() calls will not work as
// expected unless readn() is modified to check the internal buffer as well.
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for(n = 1; n < maxlen; n++) {
        if ((rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            // newline is stored, like fgets()
            if (c == '\n')
                break;
        } else if (rc == 0) {
            // EOF, n-1 bytes were read
            *ptr = 0;
            return (n - 1);
        } else
            // error, errno set by read()
            return (-1);
    }

    // null terminate like fgets()
    *ptr = 0;
    return (n);
}

// Expose the internal buffer state so that callers can check and see if more
// data was received beyond a single line.
ssize_t readlinebuf(void **vptrptr)
{
    if (read_cnt)
        *vptrptr = read_ptr;

    return (read_cnt);
}