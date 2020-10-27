
#include "stdint.h"


typedef struct {
    size_t total_memory;
    size_t free_memory;
    size_t chunk_count;
} malloc_stats_t;

void malloc_get_stats(malloc_stats_t *info);
