#include "imsai.h"
#include "stdio.h"

#define UART_CH1_BASE 0x10

uint8_t read_switches() {
    uint8_t val;
    asm volatile("in a, (0xFF)":"=a"(val)::);
    return val;
}

void write_leds(uint8_t val) {
    asm volatile(
        "ld a, %0 \n"
        "cpl \n"
        "out (0xFF), a"
        ::"r"(val):"a"
    );
}

int putchar(int c) {
    if ((char) c == '\n')
        putchar('\r');
    while (!(io_in(UART_CH1_BASE) & TTY_TX_READY)) {};
    io_out(UART_CH1_BASE + 1, (char)c);
    return (char)c;
}

int puts(const char *str) {
    while(*str) {
        putchar(*(str++));
    }
    putchar('\n');
    return 0;
}

void puti(uint8_t c) {
    const static char hex_digits[] = "0123456789ABCDEF";
    putchar(hex_digits[c & 0xF]);
}

char readc() {
    while(!(io_in(UART_CH1_BASE) & TTY_RX_AVAIL)){};
    return io_in(UART_CH1_BASE + 1);
}
