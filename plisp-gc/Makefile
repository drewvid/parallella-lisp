
all: gencode fl onefile
	rm -f Trace.out

build:
	(cd code; python gencode.py)
	bash ./build.sh

run:
	(cd code; python gencode.py)
	bash ./run.sh

gencode:
	(cd code; python gencode.py)

fl: force
	rm -f fl fl.exe
	gcc -Os -std=gnu99 -o fl fl-device.c

onefile: force
	rm -f onefile onefile.exe
	gcc -Os -std=gnu99 -o onefile onefile.c
	wc -l onefile.c

proto:
	cproto fl-device.c > proto.h

force:

clean:
	rm -f fl onefile initmem *.elf *.srec *.o *.exe* Trace.out
