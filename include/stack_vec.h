#pragma once

#include <stdint.h>

typedef uint8_t SV_ERR;

#define SV_OK               0;
#define SV_ERR_FULL         1;
#define SV_ERR_EMPTY        2;

typedef struct StackVec{
    void* buffer;
    size_t capacity;
    size_t size;
} StackVec_t;

StackVec_t* sv_create(void* buffer, size_t size);

SV_ERR sv_push(StackVec_t* vec, void* item, size_t size);

SV_ERR sv_removeBack(StackVec_t* vec, size_t size);

static void* sv_getPointer(StackVec_t* vec)
{
    return vec->buffer;
}
