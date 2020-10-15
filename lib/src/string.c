#include "string.h"

int memcmp(const void* s1, const void* s2,size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    while(n--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            p1++,p2++;
    return 0;
}

void* memset(void *ptr, int value, size_t num)
{
    void* fill = ptr;
    void* target = ptr + num;
    while(fill < target)
        *((unsigned char *)fill++) = (unsigned char)value;

    return ptr;
}

int strcmp(const char *str1, const char *str2)
{
    while(*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(const char*)str1 - *(const char*)str2;
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*s) ++s;
    return s - p;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}
