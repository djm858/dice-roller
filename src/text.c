#include "text.h"

size_t strlen_safe(const char *str, size_t maxlen) {
    void *ptr = memchr(str, '\0', maxlen);
    if (ptr) {
        return (size_t)((char *)ptr - str);
    } else {
        return maxlen;
    }
}
