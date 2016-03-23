#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#if EPIPHANY
#include "e-lib.h"
#endif
#include "defines.h"
#include "structures.h"
#define EXTERNAL
#include "globals.h"
#include "device_proto.h"

#define BUF_ADDRESS 0x8f000000
