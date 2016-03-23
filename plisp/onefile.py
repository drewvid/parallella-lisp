
def cat(filename, skip):
    fp = open(filename, "r")
    lines = fp.readlines()
    for line in lines[skip:]:
        print line.rstrip()


if __name__ == "__main__":

    cat("includes.h", 0)
    cat("libplisp.c", 10)
    cat("libdevice.c", 12)
    cat("device_main.c", 9)
