rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: scipher_driver

scipher_driver: scipher.o
	$(CC) $(CFLAGS) -o scipher_driver scipher_driver.c scipher.o $(LIBS)

scipher.o: scipher.c scipher.h
	$(CC) $(CFLAGS) -c scipher.c $(LIBS)

clean:
	$(rm) scipher.o scipher_driver *.o core *~

