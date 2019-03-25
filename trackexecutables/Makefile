CC=gcc
CFLAGS=-g -Wall

all:	wx_scan setwx

wx_scan:	wx_scan.c
	$(CC) $(CFLAGS) -o wx_scan  wx_scan.c

setwx:	setwx.c
	$(CC) $(CFLAGS) -o setwx setwx.c

test:
	./setwx &
	./setwx &
	./wx_scan
	./setwx &
	./setwx &
	./setwx &
	./setwx &
	./wx_scan
	./setwx &
	./setwx &
	./setwx &
	./setwx &
	./setwx &
	./setwx &
	./wx_scan

clean:
	rm -f *.o all
	rm -f *~ all
	rm -f *# all
	rm -f wx_scan all
	rm -f setwx all
