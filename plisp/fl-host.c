#define EPIPHANY 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <e-hal.h>
#include "common.h"

#define _BufOffset (0x01000000)

char *readFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    char *code;
    size_t n = 0;
    int c;
    if (file == NULL) return NULL;
    code = malloc(BANKSIZE);
    while ((c = fgetc(file)) != EOF && n < BANKSIZE-1)
        code[n++] = (char)c;
    code[n] = '\0';
    fclose(file);
    return code;
}

int main(void) {
    char *code, filename[64];
    int done[16], all_done, i, j, rows, cols, id;
    e_platform_t platform;
    e_epiphany_t dev;
    e_mem_t emem;
    ememory memory;
    unsigned int clr = (unsigned int)0x00000000;

    memset(&memory, 0, sizeof(ememory));

    for(i=0; i<NCORES; i++) {
        sprintf(filename, "code/p%d.lisp", i);
        code = readFile(filename);
        sprintf(memory.data[i].code, "%s", code);
        free(code);
    }

    e_init(NULL);
    e_reset_system();
    e_get_platform_info(&platform);

    rows = platform.rows;
    cols = platform.cols;

    e_alloc(&emem, _BufOffset, sizeof(ememory));

    e_open(&dev, 0, 0, rows, cols);

    e_write(&emem, 0, 0, 0x0, &memory, sizeof(ememory));


    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++) {
            e_write(&dev, i, j, 0x7000, &clr, sizeof(clr));
        }
    }

    e_load_group("./fl-device.srec", &dev, 0, 0, rows, cols, E_TRUE);

    printf("loaded programs %d %d\n", rows, cols);

    while(1) {
        all_done=0;
        for (i=0; i<rows; i++) {
            for (j=0; j<cols; j++) {
                e_read(&dev, i, j, 0x7000, &done[i*platform.cols+j], sizeof(int));
                all_done+=done[i*platform.cols+j];
            }
        }
        if(all_done==16) {
            break;
        }
    }

    printf ("all_done = %d\n", all_done);

    memset(&memory, 0, sizeof(ememory));
    e_read(&emem, 0, 0, 0x0, &memory, sizeof(ememory));

    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++) {
            id = (4 * i) + j;
            printf("%s\n", memory.data[id].output);
        }
    }
    e_close(&dev);
    e_finalize();
}
