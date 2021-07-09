CC = gcc
LD = gcc

ODIR = obj
DIST = dist

CFLAGS = -Wall -pipe -pedantic -Werror
OFLAGS = -c -I/usr/include
LFLAGS = $(CFLAGS) -L/usr/lib/

SOURCES = $(wildcard *.c)

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

all: $(DIST)/dns-server

$(DIST):
	mkdir $(DIST)

DNS_SERVER_OBJECTS = $(ODIR)/dns-server.o $(ODIR)/dns.o $(ODIR)/debug.o

$(DIST)/dns-server: $(DNS_SERVER_OBJECTS)  $(DIST)
	$(CC) $(DNS_SERVER_OBJECTS) $(CFLAGS) -o $@

$(ODIR):
	mkdir $(ODIR)

$(ODIR)/%.o: %.c $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(ODIR) dist

rebuild: clean all

.PHONY : clean rebuild
.SILENT : clean