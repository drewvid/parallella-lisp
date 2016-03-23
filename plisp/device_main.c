#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "defines.h"
#include "structures.h"
#define EXTERNAL extern
#include "globals.h"
#include "device_proto.h"

int main(int argc, char *argv[]) {
    unsigned int row, col;
    char *input;

    //
    // get the core id
    //
    id = coreID(&row, &col);
    //
    // Initialize the core
    //
    input = coreInit(argc, argv, id);
    //
    // Read, Eval and Print
    //
    REPL(input);
    //
    // Print stats and exit
    //
    setflag("Exited normally!");
    return 0;
}
