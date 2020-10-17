/*
 * klibc/inline.h
 */

#ifndef _KLIBC_INLINE_H
#define _KLIBC_INLINE_H

#ifndef __extern_inline
#define __extern_inline extern inline __attribute__((gnu_inline))
#endif

#define NOINLINE __attribute__((noinline))

#endif				/* _KLIBC_INLINE_H */
