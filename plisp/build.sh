#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

CROSS_PREFIX=
case $(uname -p) in
    arm*)
        # Use native arm compiler (no cross prefix)
        CROSS_PREFIX=
        ;;
       *)
        # Use cross compiler
        CROSS_PREFIX="arm-linux-gnueabihf-"
        ;;
esac

${CROSS_PREFIX}gcc -Os -std=gnu99 -Wno-format-security host_main.c libhost.c -o fl-host.elf -I ${EINCS} -L ${ELIBS} -le-hal  -le-loader -lpthread

e-gcc -Os -DEPIPHANY=1 -std=gnu99 -T ${ELDF} device_main.c libdevice.c libplisp.c -o fl-device.elf -le-lib

e-objcopy --srec-forceS3 --output-target srec fl-device.elf fl-device.srec

