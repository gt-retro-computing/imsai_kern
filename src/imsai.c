#include "imsai.h"

#define UART_CH1_BASE 0x10

uint8_t read_switches() {
    uint8_t val;
    asm volatile("in a, (0xFF)":"=a"(val)::);
    return val;
}

uint8_t read_serial_status() {
    uint8_t val;
    asm volatile("in a, (0x03)":"=a"(val)::);
    return val;
}

uint8_t read_serial_data() {
    uint8_t val;
    asm volatile("in a, (0x02)":"=a"(val)::);
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


void putc(char c) {
    while (!(io_in(UART_CH1_BASE) & TTY_TX_READY)) {};
    io_out(UART_CH1_BASE + 1, c);
}

void puts(char *str) {
    while(*str) {
        putc(*(str++));
    }
}

void puti(uint8_t c) {
    const static char hex_digits[] = "0123456789ABCDEF";
    putc(hex_digits[c & 0xF]);
}

void printi(unsigned c) {
    uint8_t val;
    for (int i = 3; i >= 0; i--)
    {
        val = (c >> (i * 4)) & 0xF;
        puti(val);
    }
}

void prints(uint8_t c) {
    uint8_t val;
    for (int i = 1; i >= 0; i--)
    {
        val = (c >> (i * 4)) & 0xF;
        puti(val);
    }
}

void hexdump(uint8_t* buf, size_t len) {
    uint16_t offset = 0;
    while(offset < len)
    {
        if (offset % 16 == 0) {
            printi(offset);
            puts(" : ");
        }
        prints(*(buf + offset));
        putc(' ');
        offset++;
        if (offset % 16 == 0){
            putc('\n');
        }
    }
}

char readc() {
    while(!(io_in(UART_CH1_BASE) & TTY_RX_AVAIL)){};
    return io_in(UART_CH1_BASE + 1);
}
