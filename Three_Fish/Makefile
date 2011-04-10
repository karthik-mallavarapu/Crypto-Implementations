rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: threefish_driver

threefish_driver: threefish.o
	$(CC) $(CFLAGS) -o threefish_driver threefish_driver.c threefish.o $(LIBS)

threefish.o: threefish.c
	$(CC) $(CFLAGS) -c threefish.c $(LIBS)

clean:
	$(rm) threefish.o threefish_driver *.o core *~

