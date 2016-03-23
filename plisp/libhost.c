#define EPIPHANY 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <e-hal.h>
#include "libhost.h"
#include "structures.h"
#include "host_proto.h"
#define EXTERNAL
#include "globals.h"

#define _BufOffset (0x01000000)

char *lmem;

//
// Convert a pointer from device and host
//
char *host_ptr(char *ptr) {
    unsigned int diff = ptr - (char *)0x8f000000;
    return (char *)(lmem + diff);
}

//
// Covert pointer references to structure elements
//
node *dr_node(node *cell) {
    if (cell == NULL) {
        return NULL;
    }
    return (node *)host_ptr((char *)cell);
}

node *car(node *cell) {
    if (cell->car == NULL) {
        return NULL;
    }
    return (node *)host_ptr((char *)(cell->car));
}

node *cdr(node *cell) {
    if (cell->cdr == NULL) {
        return NULL;
    }
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
long long ival(node *cell) {
    long long i = cell->i;
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
    if (nullp(l)) {
        printf(" NULL ");
    }
    else if (teep(l)) {
        printf(" t ");
    }
    else if (nilp(l)) {
        printf(" nil ");
    }
    else if (symp(l)) { // symbol
        printf(" %s ", dr_name(l));
    }
    else if (intp(l)) { // integer
        printf(" %lld ", ival(l));
    } else if (lambdap(l)) { // lambda expression
        printf(" #lambda ");
        print(largs(l));
        print(lbody(l));
    } else if (subrp(l)) {
        printf(" subr ");
    }
    else if (fsubrp(l)) {
        printf(" fsubr ");
    }
    else if (pairp(l)) { // pair
        prpair(l);
    }
    else if (consp(l)) {
        if (!nullp(cdr(l)) && !consp(cdr(l))) { // untyped dotted pair
            prpair(l);
        }
        else { // list
            printf("( ");
            for (node *ptr = l; ptr != NULLPTR; ptr = cdr(ptr)) {
                print(car(ptr));
            }
            printf(" )");
        }
    } else {
        printf(" Something went wrong \n");
    }
}

//
// Read a text file
//
char *readFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        fprintf(stderr, "%s\n", "file not found");
        exit(-1);
    }
    char *code;
    size_t n = 0;
    int c;
    if (file == NULL) {
        return NULL;
    }
    code = malloc(BANKSIZE);
    while ((c = fgetc(file)) != EOF && n < BANKSIZE-1) {
        code[n++] = (char)c;
    }
    code[n] = '\0';
    fclose(file);
    return code;
}

//
// convert a pointer from host to device
//
void *device_ptr(char *base, char *ptr) {
    unsigned int diff = ptr - base;
    return (void *)(0x8f000000 + diff);
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
            id = (cols * i) + j;
            freeNodeArray = memory->data[id].freeNodeArray;
            for (k = 0; k < FREEOBJECT - 1; k++) {
                char *ptr = (char *)&freeNodeArray[k + 1];
                freeNodeArray[k].next = (node *)device_ptr(base, ptr);
                freeNodeArray[k].type = FREE;
            }
            freeNodeArray[FREEOBJECT - 1].type = FREE;
            freeNodeArray[FREEOBJECT - 1].next = NULL;
            char *ptr = (char *)memory->data[id].freeNodeArray;
            memory->data[id].freelist = (node *)device_ptr(base, ptr);
        }
    }
}

void createStringFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    string *freeStringArray;
    char *base = (char *)memory;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeStringArray = memory->data[id].freeStringArray;
            for (k = 0; k < FREESTRING - 1; k++) {
                char *ptr = (char *)&freeStringArray[k + 1];
                freeStringArray[k].next = (string *)device_ptr(base, ptr);
            }
            freeStringArray[FREESTRING - 1].next = NULL;
            char *ptr = (char *)memory->data[id].freeStringArray;
            memory->data[id].stringfreelist = (string *)device_ptr(base, ptr);
        }
    }
}

void createNameFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    namestr *freeNameArray;
    char *base = (char *)memory;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeNameArray = memory->data[id].freeNameArray;
            for (k = 0; k < FREENAME - 1; k++) {
                char *ptr = (char *)&freeNameArray[k + 1];
                freeNameArray[k].next = (namestr *)device_ptr(base, ptr);
            }
            freeNameArray[FREENAME - 1].next = NULL;
            char *ptr = (char *)memory->data[id].freeNameArray;
            memory->data[id].namefreelist = (namestr *)device_ptr(base, ptr);
        }
    }
}

//
// Allocate and initialize the ememory data structure
//
ememory *init_ememory(int argc, char *argv[], int rows, int cols) {
    char *code, filename[128];
    ememory *memory = (ememory *)calloc(1, sizeof(ememory));
    if (!memory) {
        fprintf(stderr, "%s\n", "out of memory in init_ememory");
        exit(-1);
    }
    lmem = (char *)memory;
    if (argc == 2) {
        code = readFile(argv[1]);
    }
    for (int i=0; i < NCORES; i++) {
        if (argc != 2) {
            sprintf(filename, "code/p%d.lisp", i);
            code = readFile(filename);
            sprintf(memory->data[i].code, "%s", code);
            free(code);
        } else {
            sprintf(memory->data[i].code, "%s", code);
        }
    }
    if (argc == 2) {
        free(code);
    }
    createFreelist(memory, rows, cols);
    createStringFreelist(memory, rows, cols);
    createNameFreelist(memory, rows, cols);
    return memory;
}

//
// Write the ememory data structure to device memory
//
void write_ememory(e_mem_t *emem, ememory *memory) {
    if (E_OK != e_alloc(emem, _BufOffset, sizeof(ememory))) {
        fprintf(stderr, "\nERROR: Can't allocate Epiphany DRAM!\n\n");
        exit(1);
    }
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
    while (1) {
        all_done=0;
        for (int i=0; i<rows; i++) {
            for (int j=0; j<cols; j++) {
                e_read(dev, i, j, 0x7000, &done[i*cols+j], sizeof(int));
                all_done+=done[i*cols+j];
            }
        }
        if (all_done==16) {
            break;
        }
    }
}

//
// Print out global variables
//
void prGlobals(ememory *memory, int id) {
    edata *data = &memory->data[id];
    printf("\n");
    printf("processor id: \t\t%d\n", data->id);
    printf("memory: \t\t%d\n", data->ememory_size);
    printf("node size: \t\t%d\n", data->node_size);
    printf("nnodes: \t\t%d\n", data->nnodes);
    printf("nodemem: \t\t%d\n", data->nodemem);
    printf("nnames: \t\t%d\n", data->nnames);
    printf("namemem: \t\t%d\n", data->namemem);
    printf("nstrings: \t\t%d\n", data->nstrings);
    printf("stringmem: \t\t%d\n", data->stringmem);
    printf("setflag message: \t%s\n\n", data->message);
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
            int n = 1;
            for (node *ptr = history; ptr != NULLPTR; ptr = cdr(ptr)) {
                if (n) {
                    printf("> ");
                }
                n = !n;
                print(car(ptr));
                printf("\n\n");
            }
            prGlobals(memory, id);
        }
    }
}
