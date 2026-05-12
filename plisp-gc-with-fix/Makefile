
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
	gcc -Os -std=gnu99 -Wall -o fl device_main.c libdevice.c libplisp.c

onefile: force
	python onefile.py > onefile.c
	rm -f onefile onefile.exe
	gcc -Os -std=gnu99 -o onefile onefile.c
	wc -l onefile.c

proto:
	cproto device_main.c libdevice.c libplisp.c > device_proto.h
	cproto -I/opt/adapteva/esdk/tools/host.armv7l/include host_main.c libhost.c > host_proto.h

force:

clean:
	rm -f fl onefile initmem *.elf *.srec *.o *.exe* Trace.out
