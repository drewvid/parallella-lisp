
all: clean fl onefile
	rm -f Trace.out

build:
	(cd code; python gencode.py)
	bash ./build.sh

run:
	(cd code; python gencode.py)
	bash ./run.sh

fl: force
	(cd code; python gencode.py)
	gcc -std=gnu99 -o fl fl-device.c

onefile: force
	python onefile.py
	gcc -std=gnu99 -o onefile onefile.c
	wc -l onefile.c

proto:
	cproto fl-device.c > proto.h

force:

clean:
	rm -f fl onefile *.elf *.srec *.exe*
