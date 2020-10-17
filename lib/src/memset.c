/*
 * memset.c
 */

#include "string.h"
#include "stdint.h"

void *memset(void *dst, uint8_t c, size_t n)
{
	char *q = dst;

	while (n--) {
		*q++ = c;
	}

	return dst;
}
