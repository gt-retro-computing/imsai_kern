#ifndef STDINT_H
#define STDINT_H

#include "stddef.h"

typedef unsigned char uint8_t;
typedef unsigned uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef int int16_t;
typedef long int32_t;
typedef long long int64_t;

typedef unsigned* uintptr_t;

typedef int intmax_t;
typedef unsigned uintmax_t;

#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_##MSG[(COND)?1:-1]

STATIC_ASSERT(sizeof(uint8_t) == 1, uint8_t);
STATIC_ASSERT(sizeof(uint16_t) == 2, uint16_t);
STATIC_ASSERT(sizeof(uint32_t) == 4, uint32_t);
STATIC_ASSERT(sizeof(uint64_t) == 8, uint64_t);

#undef STATIC_ASSERT

#endif
