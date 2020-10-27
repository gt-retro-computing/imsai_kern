#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "imsai.h"

extern char _bss_start;
extern char _bss_end;
size_t _total_mem_size = 0;
int main();

static void _memtest() {
    volatile unsigned char* mem_loc = 0x0; // Almos 1K
    #define INCREMENT 1024
    do {
        mem_loc = (void*)((size_t)mem_loc + INCREMENT);
        unsigned char save = *mem_loc;
        if (*mem_loc == 0xFF) {
            *mem_loc = 0x55;
            if (*mem_loc != 0x55) {
                _total_mem_size = ((size_t)mem_loc) - 1;
                return;
            }
            *mem_loc = save;
        }
    } while((size_t)mem_loc != 0);
    _total_mem_size = ((size_t)mem_loc) - 1;
}

int c_start() {
    for (char *i = &_bss_start; i < &_bss_end; ++i)
    {
        *i = 0;
    }

    _memtest();

    add_malloc_block((void*)FREE_MEM_START, _total_mem_size-FREE_MEM_START);

    return main();
}
