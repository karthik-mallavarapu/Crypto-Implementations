rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: skein_driver

skein_driver: skein.o
	$(CC) $(CFLAGS) -o skein_driver skein_driver.c skein.o $(LIBS)

skein.o: skein.c
	$(CC) $(CFLAGS) -c skein.c $(LIBS)

clean:
	$(rm) skein.o skein_driver *.o core *~

