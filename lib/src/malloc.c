#include "malloc.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"

#define TOTAL_METADATA_SIZE (sizeof(metadata_t) + sizeof(size_t))
#define MIN_BLOCK_SIZE (TOTAL_METADATA_SIZE + 1)
#define CANARY_MAGIC_NUMBER 0xE629


typedef struct metadata {
    struct metadata *next;
    size_t size; // size of the user usable part of the block in number of bytes
    size_t canary;
} metadata_t;

// Free List
static metadata_t *size_list = NULL;
static size_t total_memory = 0;

static metadata_t *merge_with_front(metadata_t *front, metadata_t *front_parent, size_t empty_Size) {
    if (front_parent == NULL) {
        size_list = front->next;
    } else {
        front_parent->next = front->next;
    }
    front->size += empty_Size;
    return front;
}

static metadata_t *merge_with_back(metadata_t *back, metadata_t *back_parent, metadata_t *original) {
    if (back_parent == NULL) {
        size_list = back->next;
    } else {
        back_parent->next = back->next;
    }
    original->size += back->size + TOTAL_METADATA_SIZE;
    return original;
}

static metadata_t *add_block(void *block, size_t raw_size) {
    if (raw_size < MIN_BLOCK_SIZE) {
        return NULL;
    }
    metadata_t *metaBlock = (metadata_t *) block;
    metaBlock->size = raw_size - TOTAL_METADATA_SIZE;
    void *endAddr = (uint8_t *) metaBlock + TOTAL_METADATA_SIZE + metaBlock->size;

    metadata_t *currentBlock = size_list;
    metadata_t *parentBlock = NULL;
    while (currentBlock) {
        if ((uint8_t *) currentBlock + currentBlock->size + TOTAL_METADATA_SIZE == block) {
            metaBlock = merge_with_front(currentBlock, parentBlock, metaBlock->size+TOTAL_METADATA_SIZE);
        } else if (endAddr == (uint8_t *) currentBlock) {
            metaBlock = merge_with_back(currentBlock, parentBlock, metaBlock);
        } else {
            parentBlock = currentBlock;
        }
        currentBlock = currentBlock->next;
    }
    if (size_list && size_list->size < metaBlock->size) {
        metadata_t *loopBlock = size_list;
        while (loopBlock && loopBlock->next) {
            if (loopBlock->next->size >= metaBlock->size) {
                metaBlock->next = loopBlock->next;
                loopBlock->next = metaBlock;
                return metaBlock;
            }
            loopBlock = loopBlock->next;
        }
        if (loopBlock) {
            metaBlock->next = loopBlock->next;
            loopBlock->next = metaBlock;
        }
    } else {
        metaBlock->next = size_list;
        size_list = metaBlock;
    }
    return metaBlock;
}

static void *set_canary(metadata_t *block) {
    block->canary = ((size_t) block);
    *(size_t *) (((uint8_t *) (block + 1)) + block->size) = block->canary;
    block->next = NULL;
    return block + 1;
}

static void *remove_block(metadata_t *block, metadata_t *parent, size_t request_size) {
    if (parent) {
        parent->next = block->next;
    } else {
        size_list = block->next;
    }
    if (block->size == request_size) {
        return set_canary(block);
    } else {
        size_t oldBlockSize = block->size - (request_size + TOTAL_METADATA_SIZE);
        add_block((void *) block, oldBlockSize + TOTAL_METADATA_SIZE);
        metadata_t *newBlock = (metadata_t *) (((uint8_t *) block) + TOTAL_METADATA_SIZE + oldBlockSize);
        newBlock->size = request_size;
        return set_canary(newBlock);
    }
}

static void *find_fit_block(size_t request_size) {
    metadata_t *current_block = size_list;
    metadata_t *parent = NULL;
    while (current_block) {
        if (current_block->size == request_size || current_block->size >= request_size + MIN_BLOCK_SIZE) {
            return remove_block(current_block, parent, request_size);
        }
        parent = current_block;
        current_block = current_block->next;
    }
    return NULL;
}

/* MALLOC
 * See PDF for documentation
 */
void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    void *block = find_fit_block(size);
    if (block) {
        return block;
    } else {
        return NULL;
    }
}

static size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

/* REALLOC
 * See PDF for documentation
 */
void *realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    metadata_t *block = ((metadata_t *) ptr) - 1;
    size_t canary = ((size_t) block);
    size_t endCanary = *(size_t *) (((uint8_t *) block) + sizeof(metadata_t) + block->size);
    if (block->canary != canary || endCanary != canary) {
        // TODO Exception: Canary!!!!
        return NULL;
    }
    void* newBlk = malloc(size);
    memcpy(newBlk, ptr, min(size, block->size));
    free(ptr);
    return newBlk;
}

/* CALLOC
 * See PDF for documentation
 */
void *calloc(size_t nmemb, size_t size) {
    void *memory = malloc(nmemb * size);
    if (memory) {
        memset(memory, 0, size * nmemb);
    }
    return memory;
}

/* FREE
 * See PDF for documentation
 */
void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    metadata_t *block = ((metadata_t *) ptr) - 1;
    size_t canary = ((size_t ) block);
    size_t  endCanary = *(size_t *) (((uint8_t *) block) + sizeof(metadata_t) + block->size);
    if (block->canary != canary || endCanary != canary) {
        // TODO: Exception: Canary!!!!!
        asm volatile("ld a, 0x3\n cpl \n out (0xFF), a \nhalt");
        return;
    }
    add_block((void *) block, block->size + TOTAL_METADATA_SIZE);
}

void add_malloc_block(void* ptr, size_t size) {
    add_block(ptr, size);
    total_memory += size;
}

void malloc_get_stats(malloc_stats_t *info) {
    if (!info)
        return;

    info->total_memory = total_memory;
    size_t free_memory = 0;
    size_t chunk_count = 0;

    for (metadata_t *node = size_list; node != NULL; node = node->next) {
        free_memory += node->size;
        chunk_count++;
    }

    info->free_memory = free_memory;
    info->chunk_count = chunk_count;
}



/*
__extern void free(void *);
__extern void *malloc(size_t);
__extern void *calloc(size_t, size_t);
__extern void *realloc(void *, size_t);

__extern void add_malloc_block(void *, size_t);
__extern void get_malloc_memory_status(size_t *, size_t *);
*/









