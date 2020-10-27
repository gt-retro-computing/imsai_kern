#include "shell.h"

#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"

#include "imsai.h"
#include "floppy.h"

#define MAX_LENGTH 127

#define C_BELL      0x07
#define C_BS        0x08
#define C_LF        0x0A
#define C_CR        0x0D
#define C_SPACE     0x20
#define C_DEL       0x7F

#define MAX_ARG 32
typedef struct {
    uint8_t argc;
    char** argv;
} command_t;

struct cmd_entry {
    const char *name;
    int (*func)(command_t*);
};

static int last_command_return = 0;

NOINLINE
uint8_t readLine(char *buffer) {
    uint8_t len = 0;
    for(;;)
    {
        char c = readc();
        if (c >= C_SPACE && c < C_DEL)
            if (len < MAX_LENGTH)
            {
                buffer[len++] = c;
                putchar(c);
            }
            else
                putchar(C_BELL);
        else if (c == C_DEL || c == C_BS) // Delete
            if (len > 0)
            {
                len--;
                putchar(C_BS); putchar(C_SPACE); putchar(C_BS);
            }
            else
                putchar(C_BELL);
        else if (c == C_LF)
            continue;
        else if (c == C_CR) {
            buffer[len] = '\0';
            break;
        }
        else
            putchar(C_BELL);
    }
    return len;
}

// NOINLINE
static char* shell_sep(char* line) {
    static char* str = NULL;
    if (line != NULL)
        str = line;

    if (str == NULL)
        return NULL;

    char* begin = str;

    while(*str) {
        if (*str == ' ') {
            *str++ = '\0';
            return begin;
        }
        str++;
    }

    if (*begin) {
        str = NULL;
        return begin;
    }
    return NULL;
}

// NOINLINE
static void parse_command(command_t* command, char* line) {
    if (command->argv == NULL) {
        command->argv = malloc(sizeof(char**) * MAX_ARG);
        if (command->argv == NULL)
            return;
    }
    command->argc = 0;

    char* tok = shell_sep(line); //
    for (uint8_t i = 0; i < MAX_ARG; ++i)
    {
        if (tok == NULL) {
            break;
        }
        command->argv[command->argc++] = tok;
        tok = shell_sep(NULL);
    }
}

// NOINLINE
static int cmd_echo(command_t* cmd) {
    if (cmd->argc == 2 && strcmp("$?", cmd->argv[1]) == 0) {
        printf("$? = %d\n", last_command_return);
        return 0;
    }
    for (uint8_t i = 1; i < cmd->argc; i++)
    {
        printf("%s ", cmd->argv[i]);
    }
    putchar('\n');
    return 0;
}

static int cmd_free(command_t *cmd) {
    printf("This is free command\n");

    malloc_stats_t stats;
    malloc_get_stats(&stats);

    printf("total mem: %u\n", stats.total_memory);
    printf(" free mem: %u (%u used)\n", stats.free_memory, stats.total_memory - stats.free_memory);
    printf("chunk cnt: %u\n", stats.chunk_count);
    return 0;
}

void prepare_buffer(uint8_t *buffer, uint8_t track, uint8_t num_sector, uint8_t dd);

static int cmd_format(command_t* cmd) {
    if (cmd->argc < 4) {
    usage:
        printf("Invalid argument: format <drive> <sd/dd> <# track>\n");
        return 1;
    }
    bool dd = false;
    size_t sector_size = 512;
    if (strcmp("dd", cmd->argv[2]) == 0) {
        dd = true;
    } else if (strcmp("sd", cmd->argv[2]) == 0) {
        dd = false;
    } else {
        goto usage;
    }

    int8_t drive = atoi(cmd->argv[1]);
    if (drive < 0 || drive > 3) {
        printf("Drive number is limited to 0-3\n");
        return 2;
    }

    uint8_t track = atoi(cmd->argv[3]);
    if (track == 0) {
        goto usage;
    }

    printf("About to format drive %u with %u tracks and "
        "sector size = %u bytes. DD = %u.\nProceed? (Y/N)\n",
        drive, track, sector_size, dd);

    char c = readc();
    if (c == 'y' || c == 'Y') {
        printf("Preparing buffer...\n");
        void * format_buffer = malloc(0x1a00); // 6.5k

        if (dd)
            fdc_select_dd(drive);
        else
            fdc_select(drive);

        fdc_home();

        for (uint8_t i = 0; i < track; ++i)
        {
            fdc_seek(i);
            prepare_buffer(format_buffer, i, 9, dd);
            printf("Formatting track %u...\n", i);
            fdc_write_track(format_buffer, 0x1a00);
        }

        free(format_buffer);
    }
    else
        printf("Aborting...\n");

    return 0;
}

static int cmd_fdread(command_t* cmd) {
    if (cmd->argc < 5) {
    usage:
        printf("Invalid argument: fdread <drive> <sd/dd> <track> <sector>\n");
        return 1;
    }
    bool dd = false;
    size_t sector_size = 512;
    if (strcmp("dd", cmd->argv[2]) == 0) {
        dd = true;
    } else if (strcmp("sd", cmd->argv[2]) == 0) {
        dd = false;
    } else {
        goto usage;
    }

    int8_t drive = atoi(cmd->argv[1]);
    if (drive < 0 || drive > 3) {
        printf("Drive number is limited to 0-3\n");
        return 2;
    }

    uint8_t track = atoi(cmd->argv[3]);

    uint8_t sector = atoi(cmd->argv[4]);
    if (sector == 0) {
        goto usage;
    }

    if (dd)
        fdc_select_dd(drive);
    else
        fdc_select(drive);

    fdc_home();
    fdc_seek(track);

    void *buffer = malloc(sector_size + 40);
    FDC_ERR status = fdc_read(sector, buffer, sector_size);
    if (status != FDC_ERR_NOT_FOUND)
        hexdump(buffer, sector_size);
    printf("Returned: 0x%X\n", status);
    free(buffer);

    return 0;
}

static int cmd_fdcopy(command_t* cmd) {
    if (cmd->argc < 6) {
    usage:
        printf("Invalid argument: fdread <src> <dst> <sd/dd> <from track> <to track>\n");
        return 1;
    }
    bool dd = false;
    size_t sector_size = 512;
    if (strcmp("dd", cmd->argv[3]) == 0) {
        dd = true;
    } else if (strcmp("sd", cmd->argv[3]) == 0) {
        dd = false;
    } else {
        goto usage;
    }

    int8_t src = atoi(cmd->argv[1]);
    if (src < 0 || src > 3) {
        printf("src number is limited to 0-3\n");
        return 2;
    }

    int8_t dst = atoi(cmd->argv[2]);
    if (src < 0 || src > 3) {
        printf("dst number is limited to 0-3\n");
        return 2;
    }


    uint8_t track_start = atoi(cmd->argv[4]);

    uint8_t track_end = atoi(cmd->argv[5]);

    void *buffer = malloc(sector_size + 40);

    for (uint8_t i = track_start; i <= track_end; ++i)
    {
        if (dd) {
            fdc_select_dd(src);
        } else {
            fdc_select(src);
        }
        fdc_home();
        fdc_seek(i);
        if (dd) {
            fdc_select_dd(dst);
        } else {
            fdc_select(dst);
        }
        fdc_home();
        fdc_seek(i);
        for (uint8_t sec = 0; sec < 9; sec++) {
            if (dd)
                fdc_select_dd(src);
            else
                fdc_select(src);
            FDC_ERR status = fdc_read(sec + 1, buffer, sector_size);
            if (status != FDC_OK) {
                printf("ERROR: failed to read track: %u sec: %u\n", i, sec + 1);
                goto end;
            }
            if (dd)
                fdc_select_dd(dst);
            else
                fdc_select(dst);
            status = fdc_write(sec + 1, buffer, sector_size);
            if (status != FDC_OK) {
                printf("ERROR: failed to write track: %u sec: %u\n", i, sec + 1);
                goto end;
            } else
                printf("Copied track %u, sec: %u\n", i, sec + 1);
        }

    }


end:
    free(buffer);

    return 0;
}

struct cmd_entry cmd_table[] = {
    {"echo", cmd_echo},
    {"free", cmd_free},
    {"format", cmd_format},
    {"fdread", cmd_fdread},
    {"fdcopy", cmd_fdcopy}
};


void run_shell(const char* prefix) {
    char* text_buffer = malloc(sizeof(char) * MAX_LENGTH + 1);

    if (text_buffer == NULL)
        return;

    command_t cmd;
    memset(&cmd, 0, sizeof(command_t));

    for(;;) {
        printf("%s",prefix);
        readLine(text_buffer); putchar('\n');
        parse_command(&cmd, text_buffer);

        if (cmd.argc > 0) {
            for (size_t i = 0; i < (sizeof(cmd_table) / sizeof(cmd_table[0])); i++) {
                if (strcmp(cmd_table[i].name, cmd.argv[0]) == 0) {
                    last_command_return = cmd_table[i].func(&cmd);
                    goto success;
                }
            }

            printf("command not found: %s\n", cmd.argv[0]);
        success:
            ;
        }
    }

    free(cmd.argv);
    free(text_buffer); // No Leaking plz
}
