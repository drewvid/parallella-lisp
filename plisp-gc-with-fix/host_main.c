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
#define EXTERNAL extern
#include "globals.h"

int e_load_group(char *executable, e_epiphany_t *dev, unsigned row, unsigned col, unsigned rows, unsigned cols, e_bool_t start);

//
// Lets go!
//
int main(int argc, char *argv[]) {
    int rows, cols, result;
    char *code, filename[64];
    e_platform_t platform;
    e_epiphany_t dev;
    e_mem_t emem;
    //
    // init the device and get platform data
    //
    if (E_OK != e_init(NULL)) {
        fprintf(stderr, "\nERROR: epiphinay initialization failed!\n\n");
        exit(1);
    }
    if (E_OK != e_reset_system() ) {
        fprintf(stderr, "\nWARNING: epiphinay system rest failed!\n\n");
    }
    fprintf(stderr, "Getting platform info\n");
    if ( E_OK != e_get_platform_info(&platform) ) {
        fprintf(stderr, "Failed to get Epiphany platform info\n");
        exit(1);
    }
    fprintf(stderr, "Platform version: %s, HAL version 0x%08x\n",
            platform.version, platform.hal_ver);
    rows = platform.rows;
    cols = platform.cols;
    memory = init_ememory(argc, argv, rows, cols);
    //
    // open the device
    //
    if (E_OK != e_open(&dev, 0, 0, rows, cols)) {
        fprintf(stderr, "\nERROR: Can't establish connection to Epiphany device!\n\n");
        exit(1);
    }
    //
    // Write the ememory data structure to device memory
    //
    write_ememory(&emem, memory);
    //
    // Load the code
    //
    clear_done_flags(&dev, rows, cols);
    result = e_load_group("./fl-device.srec", &dev, 0, 0, rows, cols, E_TRUE);
    if (result == E_ERR) {
        printf("Error loading Epiphany program.\n");
        exit(1);
    }
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
    if (e_close(&dev)) {
        printf( "\nERROR: Can't close connection to Epiphany device!\n\n");
        exit(1);
    }
    if (e_free(&emem)) {
        printf( "\nERROR: Can't release Epiphany DRAM!\n\n");
        exit(1);
    }
    e_finalize();
}
