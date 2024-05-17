#include <string.h>

char *stpcpy(char *restrict dst, const char *restrict src)
{
    char  *p;

    p = mempcpy(dst, src, strlen(src));
    *p = '\0';

    return p;
}