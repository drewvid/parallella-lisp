import sys

def prfile(lineIn):
    incfile = lineIn.split('"')[1]
    fp = open(incfile, "r")
    for line in fp:
	if "include" in line:
            prfile(line)
        else:
            out.write(line)
    out.write("\n")
    fp.close


if __name__ == '__main__':

    out = open("onefile.c", "w")

    n = 0
    fp = open("fl-device.c", "r")
    for line in fp:
        n += 1
        if "#include" in line and n > 4 and "e-lib" not in line:
            prfile(line)
        else:
            out.write(line)
    fp.close()
    out.close()
