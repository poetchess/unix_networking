#include "unp.h"

// Write n bytes to a descriptor

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if ((nwritten < 0) && (errno == EINTR))
                // call write() again
                nwritten = 0;
            else
                // error
                return (-1);
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return n;
}
