CC = gcc
LD = gcc

CFLAGS = -Wall -pipe -pedantic -Werror
OFLAGS = -c -I/usr/include
LFLAGS = $(CFLAGS) -L/usr/lib/
EXECUTABLE = dns-server

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

DEBUG = no
PROFILE = no
PEDANTIC = no
OPTIMIZATION = -O3

ifeq ($(DEBUG), yes)
	CFLAGS += -g
	OPTIMIZATION = -O0
endif

ifeq ($(PROFILE), yes)
	CFLAGS += -pg
endif

CFLAGS += $(OPTIMIZATION)

all: dns-server

dist:
	mkdir dist

dns-server: $(OBJECTS) dist
	$(CC) $(OBJECTS) $(CFLAGS) -o dist/$(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

rebuild: clean all

.PHONY : clean
.SILENT : clean