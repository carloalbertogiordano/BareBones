#include <string.h>

void *mempcpy(void *dstptr, const void *srcptr, size_t size){
    unsigned char *dst = (unsigned char *)dstptr;
    const unsigned char *src = (const unsigned char *)srcptr;

    unsigned int i = 0;
    for(; i < size; i++){
        dst[i] = src[i];
    }
    return (dstptr + (++i));
}