/*
 * stdio.h
 */

#ifndef _STDIO_H
#define _STDIO_H

#include "klibc/extern.h"
#include "klibc/inline.h"
#include "stdarg.h"
#include "stddef.h"
#include "string.h"
#include "printf.h"

int putchar(int c);
int puts(const char *str);

#endif				/* _STDIO_H */
