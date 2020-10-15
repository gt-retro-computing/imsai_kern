#include <stdint.h>

void do_thing(uint8_t *buf) {
    *(buf + 20) = -2;
    *(buf + 130) = -2;
}
