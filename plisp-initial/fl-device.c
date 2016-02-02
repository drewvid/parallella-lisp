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
// string i/o
//
void itos(int i, char *buf) {
    sprintf(buf, "%d", i);
}

int stoi(char* snum) {
    int i;
    sscanf(snum, "%d", &i);
    return i;
}

void appendString(char *item) { // add a string to the output
    for(char *s = item; *s != '\0'; s++)
        *result++ = *s;
    *result = '\0';
}

void appendStrings(int count, ...) {
    va_list args;
    va_start(args, count);
    while (count--)
        appendString(va_arg(args, char *));
    va_end(args);
}

void appendInt(int num, char bool) {
    char buf[NAMESTRMAX + 1];
    itos(num, buf);
    appendString(buf);
    if (bool) appendString("\n");
}


#if EPIPHANY
void setflag() {
    unsigned *d;

    appendString("\nnode size: ");
    appendInt(sizeof(node), TRUE);
    appendString("strings allocated: ");
    appendInt(freeStringIndex, TRUE);
    appendString("node allocated: ");
    appendInt(freeNodeIndex, TRUE);
    appendString("names allocated: ");
    appendInt(freeNameIndex, TRUE);
    appendString("memory size: ");
    appendInt(sizeof(ememory), TRUE);

    memory->data[id].finished = 1;

    d = (unsigned *) 0x7000;
    (*(d)) = 0x00000001;

    __asm__ __volatile__("idle");
}
#else
void setflag() {

    appendString("\nnode size: ");
    appendInt(sizeof(node), TRUE);
    appendString("strings allocated: ");
    appendInt(freeStringIndex, TRUE);
    appendString("node allocated: ");
    appendInt(freeNodeIndex, TRUE);
    appendString("names allocated: ");
    appendInt(freeNameIndex, TRUE);
    appendString("memory size: ");
    appendInt(sizeof(ememory), TRUE);

    printf("%s", output);
    exit(0);
}
#endif

// LISP Code

#include "lisp.c"

// End of LISP Code

#if EPIPHANY

int coreID(unsigned int *row, unsigned int *col) {

    e_coreid_t coreid;

    coreid = e_get_coreid();
    coreid = coreid - e_group_config.group_id;
    *row = (coreid >> 6) & 0x3f;
    *col = coreid & 0x3f;

    return((*row * 4) + *col);
}

void coreInit() {

    memory = (ememory *)(BUF_ADDRESS);

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

}

#else

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

int coreID(unsigned int *row, unsigned int *col) {

    *row = 1;
    *col = 1;

    return((*row * 4) + *col);
}

void coreInit(void) {
    char *code;

    memory = (ememory *)malloc(sizeof(ememory));

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

    code = readFile("code/p2.lisp");
    sprintf(memory->data[id].code, "%s", code);
}

#endif

int main(void) {
    unsigned int row, col;
    char tmpbuf[16];

    id = coreID(&row, &col);

    coreInit();


    input = &memory->data[id].code[0];
    output = &memory->data[id].output[0];

    result = output;
    memset(output, 0, BANKSIZE);

    appendInt(id, TRUE);
    sprintf(tmpbuf, "(%d, %d)\n", row + 1, col + 1);
    appendString(tmpbuf);

    REPL(input);

    setflag();

    return 0;
}
