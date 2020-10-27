#ifndef IMSAI_H
#define IMSAI_H

#include "stdint.h"

#define TTY_RX_AVAIL        0b01000000U
#define TTY_TX_READY        0b10000000U

#define HALT() do{asm volatile("halt" :::);}while(0)

uint8_t read_switches();
uint8_t read_serial_status();
uint8_t read_serial_data();
void write_leds(uint8_t val);
void write_serial_data(uint8_t val);

void puti(unsigned char c);
/**
 * @brief   prints a integer (16-bit) value in hex
 */
void printi(unsigned c);
void prints(uint8_t c);

void hexdump(const void *addr, const size_t len);

char readc(void);

extern size_t _total_mem_size;
extern char _stack_top;

#define FREE_MEM_START ((size_t)(&_stack_top))

static void io_out(const uint8_t port, uint8_t val) {
    asm volatile("out (%0), a"::"J"(port), "a"(val));
}

static uint8_t io_in(const uint8_t port) {
    uint8_t val;
    asm volatile("in %0, (%1)":"=a"(val):"J"(port));
    return val;
}

static void io_out_reg(uint8_t port, uint8_t val) {
    asm volatile("out (c), %1"::"c"(port), "r"(val));
}

static uint8_t io_in_reg(uint8_t port) {
    uint8_t val;
    asm volatile("in %0, (c)":"=r"(val):"c"(port));
    return val;
}

#endif
