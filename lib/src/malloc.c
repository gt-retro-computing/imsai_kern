#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct chunk_header {
    size_t prev_size;
    size_t _size;

    struct chunk_header *prev;
    struct chunk_header *next;
} chunk_header;

#define PREV_IN_USE(x) ((x)->_size & 1)
#define SIZE(x) ((x)->_size & ~1)

/*
__extern void free(void *);
__extern void *malloc(size_t);
__extern void *calloc(size_t, size_t);
__extern void *realloc(void *, size_t);

__extern void add_malloc_block(void *, size_t);
__extern void get_malloc_memory_status(size_t *, size_t *);
*/

static size_t mem_base_address = 0;
static chunk_header *free_list = NULL;

void add_malloc_block(void *base, size_t size) {
    mem_base_address = (size_t)base;
    if (mem_base_address & 1) {
        mem_base_address++;
        size--;
        size &= ~1;
    }

    free_list = (chunk_header *)mem_base_address;
    free_list->_size = size | 1;
    free_list->prev = NULL;
    free_list->next = NULL;
}

/* | header ......    size| */
/*  */





void *malloc(size_t size) {
    if (size & 1)
        size++;

    // for each node in free list
    // if node has size == size unlink and remove from free list
    // otherwise if node has size + 2 * header unlink, split relink remainder.

    return NULL;
}

// free









