#include "imsai.h"
#include "floppy.h"

#define IO_BASE 0xf0

// read / write wait
#define IO_WAIT (IO_BASE + 0x06)
#define IO_EXT (IO_BASE + 0x04)
#define IO_COM (IO_BASE + 0x00)
#define IO_DATA (IO_BASE + 0x03)
#define IO_STAT (IO_BASE + 0x00)
#define IO_SECT (IO_BASE + 0x02)

#define STAT_LOST      1 << 2
#define STAT_NOT_FOUND 1 << 4

static uint8_t busy_wait() {
    while(1) {
        uint8_t stat = io_in(IO_STAT);
        if (!(stat & 1))
            return stat;
    }
}

static void drq_wait() {
    io_in(IO_WAIT);
}

#define double_density 0


FDC_ERR fdc_select(uint8_t index) {
    uint8_t sel = 0x7f; // 8"
    // uint8_t sel = 0x3f;
    sel &= ~(1 << index);
    io_out(IO_EXT, sel);
    busy_wait();
    return FDC_OK;
}

FDC_ERR fdc_select_dd(uint8_t index) {
    uint8_t sel = 0x5f;
    sel &= ~(1 << index);
    io_out(IO_EXT, sel);
    busy_wait();
    return FDC_OK;
}

FDC_ERR fdc_home() {
    busy_wait();

    io_out(IO_COM, 0x03);

    busy_wait();
    return FDC_OK;
}

FDC_ERR fdc_seek(uint8_t track) {
    // TODO track limit lol

    busy_wait();

    io_out(IO_DATA, track);
    io_out(IO_COM, 0x13);

    busy_wait();
    return FDC_OK;
}

FDC_ERR fdc_read(uint8_t sector, uint8_t *buf, uint8_t size) {
    uint8_t *end = buf + size;

    busy_wait();

    uint8_t io_data = IO_DATA;
    uint16_t addr = (uint16_t)buf;

    io_out(IO_SECT, sector);
    io_out(IO_COM, 0x84);

    asm volatile(
        ".fdc_read_loop:\n"
        "in a, (%3)\n"
        "ini\n"
        "jp nz, .fdc_read_loop \n"
        "in a, (%3)\n"
        "ini"
        :: "hl"(addr), "b"(size), "c"(io_data), "J"(IO_WAIT) : "a");

    uint8_t finalStatus = io_in(IO_STAT);

    write_leds(finalStatus);

    if (finalStatus & STAT_NOT_FOUND)
        return FDC_ERR_NOT_FOUND;

    if (finalStatus & STAT_LOST)
        return FDC_ERR_LOST_DATA;

    return FDC_OK;
}


FDC_ERR fdc_write(uint8_t sector, uint8_t *buf, uint8_t size) {
    uint8_t *end = buf + size;

    io_out(IO_SECT, sector);
    io_out(IO_COM, 0xA4);

    uint8_t io_data = IO_DATA;
    uint16_t addr = (uint16_t)buf;

    asm volatile(
        ".fdc_write_loop:\n"
        "in a, (%3)\n"
        "outi\n"
        "jp nz, .fdc_write_loop \n"
        "in a, (%3)\n"
        "outi"
        :: "hl"(addr), "b"(size), "c"(io_data), "J"(IO_WAIT) : "a");

    uint8_t finalStatus = io_in(IO_STAT);

    write_leds(finalStatus);

    if (finalStatus & STAT_NOT_FOUND)
        return FDC_ERR_NOT_FOUND;

    if (finalStatus & STAT_LOST)
        return FDC_ERR_LOST_DATA;

    return FDC_OK;
}


void prepare_buffer(uint8_t *buffer, uint8_t track) {
    uint8_t *write = buffer;

    for (uint8_t i = 0; i < 40; ++i)
        *(write ++) = 0xFF;

    for (uint8_t i = 0; i < 6; ++i)
        *(write ++) = 0x00;

    *(write ++) = 0xFC; // Index Mark

    for (uint8_t i = 0; i < 26; ++i)
        *(write ++) = 0xFF;

    for (uint8_t i = 1; i < 7; ++i)
    {
        for (uint8_t j = 0; j < 6; ++j)
            *(write ++) = 0x00;

        *(write ++) = 0xFE;

        *(write ++) = track; // Track #

        *(write ++) = 0x00; // Side #

        *(write ++) = i; // Sector #

        *(write ++) = 0x00; // Sector Length??

        *(write ++) = 0xF7; // 2 CRC's written

        for (uint8_t j = 0; j < 11; ++j)
            *(write ++) = 0xFF;

        for (uint8_t j = 0; j < 6; ++j)
            *(write ++) = 0x00;

        *(write ++) = 0xFB; // Data Address Mark

        for (uint16_t j = 0; j < 512; ++j) // Data
            *(write ++) = 0xE5;

        *(write ++) = 0xF7; // 2 CRC's

        for (uint8_t j = 0; j < 27; ++j) // Data
            *(write ++) = 0xFF;
    }
    for (uint16_t i = 0; i < 280; ++i)
        *(write ++) = 0xFF;
}

FDC_ERR fdc_write_track(uint8_t *buffer) {
    busy_wait();

    io_out(IO_COM, 0xF4); // Write Track

    uint8_t io_data = IO_DATA;
    uint16_t addr = (uint16_t)buffer;

    asm volatile(
        ".fdc_write_track_loop:\n"
        "in a, (%3)\n"
        "outi\n"
        "in a, (%4)\n"
        "and 1\n"
        "jp nz, .fdc_write_track_loop"
        :: "hl"(addr), "b"(io_data), "c"(io_data), "J"(IO_WAIT), "J"(IO_STAT) : "a");

    uint8_t finalStatus = io_in(IO_STAT);

    write_leds(finalStatus);

    if (finalStatus & STAT_NOT_FOUND)
        return FDC_ERR_NOT_FOUND;

    if (finalStatus & STAT_LOST)
        return FDC_ERR_LOST_DATA;

    return FDC_OK;
}
