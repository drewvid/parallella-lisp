#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#if EPIPHANY
#include "e-lib.h"
#endif

#include "common.h"
#include "proto.h"

#define BUF_ADDRESS 0x8f000000

//
// Add items to the history
//
void pr(node *cell) {
    atl(&history, cell);
}

void addInt(long i) {
    pr(integer(i));
}

void addString(char *s) {
    pr(sym(s));
}

void addValue(char *s, long i) {
    addString(s); addInt(i);
}

//
// local version of strcpy
//
char *scopy(char *s1, const char *s2) {
    char *s = s1;
    while ((*s++ = *s2++) != '\0')
        ;
    *s = '\0';
    return (s1);
}

//
// local version of atoi
//
long stoi(const char *c)
{
    long value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' ) {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c)) {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}

//
// length of a string
//
int slen(char *s) {
   int c = 0;
   while(*(s+c))
      c++;
   return c;
}

//
// add memory stats to the history list
//
void prStats() {
    addValue("id: ", id);
    addValue("memory: ", sizeof(ememory));
    addValue("node size: ", sizeof(node));
    addValue("nnodes: ", nnodes);
    addValue("nodemem: ", nodemem);
    addValue("nnames: ", nnames);
    addValue("namemem: ", namemem);
    addValue("nstrings: ", nstrings);
    addValue("stringmem: ", stringmem);
}

#if EPIPHANY

//
// get the core ID
//
int coreID(unsigned int *row, unsigned int *col) {

    e_coreid_t coreid;

    coreid = e_get_coreid();
    coreid = coreid - e_group_config.group_id;
    *row = (coreid >> 6) & 0x3f;
    *col = coreid & 0x3f;

    return((*row * 4) + *col);
}

//
// Initilaize core memory
//
void coreInit(int argc, char *argv[]) {

    memory = (ememory *)(BUF_ADDRESS);

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

    freelist = freeNodeArray;
    stringfreelist = freeStringArray;
    namefreelist = freeNameArray;

}

//
// Print memory stats and store a pointer to the history list.
// Put the processor in idle mode
//
void setflag() {
    unsigned *d;

    if(nnodes < FREEOBJECT && nnames < FREENAME)
        prStats();

    memory->data[id].NULLPTR = NULLPTR;
    memory->data[id].history = history;
    memory->data[id].finished = 1;

    d = (unsigned *) 0x7000;
    (*(d)) = 0x00000001;

    __asm__ __volatile__("idle");
}

#else

//
// Read a text file
//
char *readFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    string *code;
    size_t n = 0;
    int c;
    if (file == NULL) return NULL;
    code = smalloc();
    while ((c = fgetc(file)) != EOF)
        code->s[n++] = (char)c;
    code->s[n] = '\0';
    return code->s;
}

//
// Create the freelist
//
void createFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    node *freeNodeArray;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeNodeArray = memory->data[id].freeNodeArray;
            for (k = 0; k < FREEOBJECT - 1; k++) {
                freeNodeArray[k].next = &freeNodeArray[k + 1];
                freeNodeArray[k].type = FREE;
            }
            freeNodeArray[FREEOBJECT - 1].next = NULL;
        }
    }
}

void createStringFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    string *freeStringArray;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeStringArray = memory->data[id].freeStringArray;
            for (k = 0; k < FREESTRING - 1; k++) {
                freeStringArray[k].next = &freeStringArray[k + 1];
            }
            freeStringArray[FREESTRING - 1].next = NULL;
        }
    }
}

void createNameFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    namestr *freeNameArray;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeNameArray = memory->data[id].freeNameArray;
            for (k = 0; k < FREENAME - 1; k++) {
                freeNameArray[k].next = &freeNameArray[k + 1];
            }
            freeNameArray[FREENAME - 1].next = NULL;
        }
    }
}

//
// Generate a core ID for testing
//
int coreID(unsigned int *row, unsigned int *col) {

    *row = 1;
    *col = 1;

    return((*row * 4) + *col);
}

//
// Initialize globals
//
void coreInit(int argc, char *argv[]) {
    char *code;

    memory = (ememory *)malloc(sizeof(ememory));

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

    freelist = freeNodeArray;
    stringfreelist = freeStringArray;
    namefreelist = freeNameArray;

    if (argc == 2)
        code = readFile(argv[1]);
    else
        code = readFile("code/p2.lisp");

    scopy(memory->data[id].code, code);

    createFreelist(memory, 4, 4);
    createNameFreelist(memory, 4, 4);
    createStringFreelist(memory, 4, 4);

}

//
// Printing routines
//

void nl(void) {
    printf("\n");
}

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
        printf(" %s ", name(l));
    else if (intp(l)) // integer
        printf(" %ld ", ival(l));
    else if(lambdap(l)) { // lambda expression
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
        if (not nullp(cdr(l)) and not consp(cdr(l))) // untyped dotted pair
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
// Print out the history list and exit
//
void setflag() {

    if(nnodes < FREEOBJECT && nnames < FREENAME) {
        prStats();
        forlist (ptr in history) {
            print(car(ptr));
            printf("\n");
        }
    } else {
        printf("OUT OF MEMORY\n");
        printf("nnodes %d\n", nnodes);
        printf("nnames %d\n", nnames);
        printf("nstrings %d\n", nstrings);
    }

    exit(0);
}

#endif

// LISP Code
#include "lisp.c"
// End of LISP Code

//
// test on the host - simulate the info for a single core
//
int main(int argc, char *argv[]) {
    unsigned int row, col;

    //
    // get the core id
    //
    id = coreID(&row, &col);

    //
    // Initialize the core
    //
    coreInit(argc, argv);

    //
    // load the code
    //
    input = &memory->data[id].code[0];

    //
    // Read, Eval and Print
    //
    REPL(input);

    //
    // Print stats and exit
    //
    setflag();

    return 0;
}
