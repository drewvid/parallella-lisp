#define EPIPHANY 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <e-hal.h>
#include "fl-host.h"
#include "structures.h"
#include "globals.h"

#define _BufOffset (0x01000000)

char *lmem;

//
// Convert a pointer from device to host
//
char *host_ptr(char *ptr) {
    unsigned int diff = ptr - (char *)0x8f000000;
    return (char *)(lmem + diff);
}

//
// Covert pointer references to structure elements
//
node *dr_node(node *cell) {
    if (cell == NULL) return NULL;
    return (node *)host_ptr((char *)cell);
}

node *car(node *cell) {
    if (cell->car == NULL) return NULL;
    return (node *)host_ptr((char *)(cell->car));
}

node *cdr(node *cell) {
    if (cell->cdr == NULL) return NULL;
    return (node *)host_ptr((char *)(cell->cdr));
}

node *largs(node *cell) {
    return (node *)host_ptr((char *)(cell->args));
}

node *lbody(node *cell) {
    return (node *)host_ptr((char *)(cell->body));
}

char *dr_name(node *cell) {
    namestr *n = (namestr *)host_ptr((char *)(cell->name));
    return n->s;
}

//
// Access the integer value
//

long ival(node *cell) {
    long i = cell->i;
    return i;
}

//
// Print out a list
//
void print(node *l);

void prpair(node *l) {
    printf("%s", "(");
    print(car(l));
    printf("%s", ".");
    print(cdr(l));
    printf("%s", ")");
}

void print(node *l) {
    if (nullp(l))
        printf(" NULL ");
    else if (teep(l))
         printf(" t ");
    else if (nilp(l))
         printf(" nil ");
    else if (symp(l)) // symbol
        printf(" %s ", dr_name(l));
    else if (intp(l)) { // integer
        printf(" %ld ", ival(l));
    } else if(lambdap(l)) { // lambda expression
        printf(" #lambda ");
        print(largs(l));
        print(lbody(l));
    } else if (subrp(l))
        printf(" subr ");
    else if (fsubrp(l))
        printf(" fsubr ");
    else if (pairp(l)) // pair
        prpair(l);
    else if (consp(l)) {
        if (!nullp(cdr(l)) && !consp(cdr(l))) // untyped dotted pair
            prpair(l);
        else { // list
            printf("( ");
            for (node *ptr = l; ptr != NULLPTR; ptr = cdr(ptr))
                print(car(ptr));
            printf(" )");
        }
    } else
        printf(" Something went wrong \n");
}

//
// Read a text file
//
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

//
// convert a pointer from host to device
//

node *device_ptr(char *base, char *ptr) {
    unsigned int diff = ptr - base;
    return (node *)(0x8f000000 + diff);
}

//
// create the freelist for node allocation on the device
//
void createFreelist(ememory *memory, int rows, int cols) {
	int id, k;
	node *freeNodeArray;
    char *base = (char *)memory;

    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (4 * i) + j;
            freeNodeArray = memory->data[id].freeNodeArray;
            for (k = 0; k < FREEOBJECT - 1; k++) {
                char *ptr = (char *)&freeNodeArray[k + 1];
                freeNodeArray[k].cdr = device_ptr(base, ptr);
                freeNodeArray[k].type = FREE;
                freeNodeArray[k].i = k + 1;
            }
            freeNodeArray[FREEOBJECT - 1].cdr = NULL;
            freeNodeArray[FREEOBJECT - 1].i = k + 1;
            char *ptr = (char *)memory->data[id].freeNodeArray;
            memory->data[id].freelist = device_ptr(base, ptr);
        }
    }
}

//
// Allocate and initialize the ememory data structure
//
ememory *init_ememory(int rows, int cols) {
    char *code, filename[128];
    ememory *memory = (ememory *)malloc(sizeof(ememory));
    lmem = (char *)memory;
    memset(memory, 0, sizeof(ememory));
    for(int i=0; i < NCORES; i++) {
        sprintf(filename, "code/p%d.lisp", i);
        code = readFile(filename);
        sprintf(memory->data[i].code, "%s", code);
        free(code);
    }
    createFreelist(memory, rows, cols);
    return memory;
}

//
// Write the ememory data structure to device memory
//
void write_ememory(e_mem_t *emem, ememory *memory) {
    e_alloc(emem, _BufOffset, sizeof(ememory));
    e_write(emem, 0, 0, 0x0, memory, sizeof(ememory));
}

//
// Clear the done flag on each core
//
void clear_done_flags(e_epiphany_t *dev, int rows, int cols) {
    unsigned int clr = (unsigned int)0x00000000;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            e_write(dev, i, j, 0x7000, &clr, sizeof(clr));
        }
    }
}

//
// Poll each core checking the done flag
//
void poll_device(e_epiphany_t *dev, int rows, int cols) {
    int done[16], all_done;
    while(1) {
        all_done=0;
        for (int i=0; i<rows; i++) {
            for (int j=0; j<cols; j++) {
                e_read(dev, i, j, 0x7000, &done[i*cols+j], sizeof(int));
                all_done+=done[i*cols+j];
            }
        }
        if(all_done==16) {
            break;
        }
    }
}

//
// Process the ememory structure read from device memory after all cores have finished.
// In this case just print the history list.
//
void process_ememory(e_mem_t *emem, ememory *memory, int rows, int cols) {
    e_read(emem, 0, 0, 0x0, memory, sizeof(ememory));
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            int id = (cols * i) + j;
            NULLPTR = dr_node((node *)(memory->data[id].NULLPTR));
            history = dr_node(memory->data[id].history);
            for (node *ptr = history; ptr != NULLPTR; ptr = cdr(ptr)) {
                print(car(ptr));
                printf("\n");
            }
        }
    }
}

int main(void) {
    int rows, cols;
    char *code, filename[64];
    e_platform_t platform;
    e_epiphany_t dev;
    e_mem_t emem;

    //
    // init the device and get platform data
    //
    e_init(NULL);
    e_reset_system();
    e_get_platform_info(&platform);

    rows = platform.rows;
    cols = platform.cols;

    //
    // initialize the ememory data structure and freelist
    //
    memory = init_ememory(rows, cols);

    //
    // open the device
    //
    e_open(&dev, 0, 0, rows, cols);

    //
    // Write the ememory data structure to device memory
    //
    write_ememory(&emem, memory);

    //
    // Load the code
    //
    clear_done_flags(&dev, rows, cols);
    e_load_group("./fl-device.srec", &dev, 0, 0, rows, cols, E_TRUE);

    //
    // Poll the device waiting for all cores to finish
    //
    poll_device(&dev, rows, cols);

    //
    // Process the results of device processing
    //
    process_ememory(&emem, memory, rows, cols);

    //
    // Close and finalize the device
    //
    e_close(&dev);
    e_finalize();
}
