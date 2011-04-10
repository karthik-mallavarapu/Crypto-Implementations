rm=/bin/rm -f
CC=cc
DEFS=
INCLUDES=-I.
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: gf2m_driver

gf2m_driver: gf2m.o
	$(CC) $(CFLAGS) -o gf2m_driver gf2m_driver.c gf2m.o $(LIBS)

gf2m.o: gf2m.c
	$(CC) $(CFLAGS) -c gf2m.c $(LIBS)

clean:
	$(rm) gf2m.o gf2m_driver *.o core *~

