
def cat(filename, skip):
    fp = open(filename, "r")
    lines = fp.readlines()
    for line in lines[skip:]:
        print line.rstrip()


if __name__ == "__main__":

    cat("includes.h", 0)
    cat("libplisp.c", 8)
    cat("libdevice.c", 15)
    cat("device_main.c", 7)
