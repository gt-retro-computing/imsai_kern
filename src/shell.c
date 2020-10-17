#include "shell.h"

#include "string.h"
#include "stdint.h"
#include "imsai.h"
#include "stdlib.h"
#include "stdio.h"

#define MAX_LENGTH 127

#define C_BELL      0x07
#define C_BS        0x08
#define C_LF        0x0A
#define C_CR        0x0D
#define C_SPACE     0x20
#define C_DEL       0x7F

uint8_t readLine(char *buffer) {
    uint8_t len = 0;
    for(;;)
    {
        char c = readc();
        if (c >= C_SPACE && c < C_DEL)
            if (len < MAX_LENGTH)
            {
                buffer[len++] = c;
                putc(c);
            }
            else
                putc(C_BELL);
        else if (c == C_DEL || c == C_BS) // Delete
            if (len > 0)
            {
                len--;
                putc(C_BS); putc(C_SPACE); putc(C_BS);
            }
            else
                putc(C_BELL);
        else if (c == C_LF)
            continue;
        else if (c == C_CR) {
            buffer[len] = '\0';
            break;
        }
        else
            putc(C_BELL);
    }
    return len;
}

void run_shell(char* prefix, char *buffer) {
    for(;;) {
        puts(prefix);
        char *buf = buffer;
        readLine(buf);
        putc(C_CR); putc(C_LF);

        if (strncmp("echo ", buffer, 5) == 0) {
            puts(buffer + 5);
            putc(C_CR); putc(C_LF);
        } else {
            int num = atoi(buffer);
            printf("Number in Hex: 0x%x\n", num);
            putc(C_CR); putc(C_LF);
        }
    }
}
