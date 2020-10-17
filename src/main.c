#include "stdint.h"
#include "imsai.h"
#include "floppy.h"
#include "shell.h"
#include "stdarg.h"

static uint8_t DRIVE = 0;

static char buffer[129];

void main() {

  volatile unsigned char num = 32;
  volatile unsigned char num2 = 81;

  printf("LMAO reee: %04d\n", (unsigned char)(num * num2));

  HALT();

  // run_shell("IMSAI >", buffer);

  for(;;){}
}
//     DRIVE = read_switches() & 0b11;
//     fdc_select(DRIVE);
//     fdc_home();

//     prints(DRIVE);
//     puts(" is Drive\n");

// #ifdef FORMAT
//     for (uint8_t i = 0; i <= 76; ++i)
//     {
//         fdc_seek(i);
//         prepare_buffer(buffer, i);
//         FDC_ERR format_err = fdc_write_track(buffer);
//         if (format_err != FDC_OK) {
//             puts("FORMAT ERROR");
//             HALT();
//         }
//     }
//     puts("Format Complete");
//     // HALT();
// #endif

//     uint8_t DD = 0;
//     uint16_t sectorSize = 512;

//     for (uint8_t j = 0; j <= 76; j++) {
//     retry:
//         puts("-Track 0x");
//         prints(j);
//         putc('\n');

//         fdc_seek(j);

//         uint8_t *buf = buffer;
//         for (int i = 1; i <= 7; ++i)
//         {
//             FDC_ERR err;
//         retry_sector:
//             err = fdc_read(i, buf, (sectorSize - 1));
//             if (err == FDC_ERR_NOT_FOUND) {
//                 puts("-Invalid Read Status. Is disk formatted? Switching to DD and try again\n");
//                 // HALT();
//                 if (DD)
//                     HALT();
//                 DD = 1;
//                 fdc_select_dd(DRIVE);
//                 sectorSize = 256;
//                 goto retry;
//             } else if (err == FDC_ERR_LOST_DATA) {
//                 puts("Overrun, Retrying... \n");
//                 goto retry_sector;
//             }

//             puts("-Sector 0x");
//             prints(i);
//             putc('\n');
//             hexdump(buf, sectorSize);
//             /* code */
//         }

//     }
/*
#define DEV_A 0x10
#define DEV_B 0x20

#define INT_JMP 0x4000

    puts("setting up\n");

    uint16_t *jmp = (uint16_t *)INT_JMP;
    for (uint8_t i = 0; i < 16; i++) {
        *(jmp++) = (uint16_t) int_handler;
    }

    uint8_t a = ((uint16_t)INT_JMP) >> 8;
    asm volatile (".byte 0xed, 0x47" :: "a"(a) :);

    io_out(DEV_A + 3, 1);
    io_out(DEV_B + 3, 0);

    asm volatile(".byte 0xed, 0x5e\n.byte 0xfb" :::);
    // for(;;) {
    //     char i = readc();
    //     write_leds(i);
    //     putc(i);
    // }
    // HALT();

    puts("loop\n");

    io_out(DEV_A + 5, 255);
*/


// __attribute__((used))
// void int_handler() {

//     io_out(DEV_A + 5, 255);

//     puts("I");

//     asm volatile("ei\n.byte 0xed, 0x4d":::);
// }
















// __attribute__((noinline))
// int fib(int n)
// {
//     int i;
//     int f1 = 0;
//     int f2 = 1;
//     int fi;

//     if(n == 0)
//         return 0;
//     if(n == 1)
//         return 1;

//     for(i = 2 ; i <= n ; i++ )
//     {
//         fi = f1 + f2;
//         f1 = f2;
//         f2 = fi;
//     }
//     return fi;
// }


// void sleep() {
//     for (int i = 0; i < 0xFFFF; ++i) {
//         asm("nop");
//     }
// }
