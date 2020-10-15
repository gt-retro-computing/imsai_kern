#include "stack_vec.h"

StackVec_t* sv_create(void* buffer, size_t size)
{
    if (size < sizeof(StackVec_t)) {
        return NULL;
    }

    StackVec_t* vec = buffer;
    vec->buffer = buffer + sizeof(StackVec_t);
    vec->capacity = size - sizeof(StackVec_t);
    vec->size = 0;

    return buffer;
}


SV_ERR sv_push(StackVec_t* vec, void* item, size_t size)
{
    if ((vec->size + size) > vec->capacity)
        return SV_ERR_FULL;

    memcpy(vec->buffer + vec->size, item, size);
    vec->size += size;

    return SV_OK;
}

SV_ERR sv_removeBack(StackVec_t* vec, size_t size)
{
    if (vec->size >= size) {
        vec->size -= size;
        return SV_OK;
    }
    return SV_ERR_EMPTY;
}
