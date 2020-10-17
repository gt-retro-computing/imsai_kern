extern char _bss_start;
extern char _bss_end;

#include "string.h"

int main();

int c_start() {
    for (char *i = &_bss_start; i < &_bss_end; ++i)
    {
        *i = 0;
    }

    return main();
}
