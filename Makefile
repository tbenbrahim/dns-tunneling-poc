CC = gcc
LD = gcc

SRC = src
ODIR = obj
DIST = dist

CFLAGS = -Wall -pipe -pedantic -Werror
OFLAGS = -c -I/usr/include
LFLAGS = $(CFLAGS) -L/usr/lib/ -luuid -lbsd 

SOURCES = $(wildcard *.c)

DEBUG = no
PROFILE = no
PEDANTIC = no
OPTIMIZATION = -O3

all: $(DIST)/dns-server $(DIST)/dns-send

$(DIST):
	mkdir $(DIST)

DNS_SERVER_OBJECTS = $(ODIR)/dns-server.o $(ODIR)/dns.o $(ODIR)/debug.o $(ODIR)/base32.o 
DNS_SEND_OBJECTS = $(ODIR)/dns-send.o $(ODIR)/base32.o  $(ODIR)/debug.o 


$(DIST)/dns-server: $(DNS_SERVER_OBJECTS)  $(DIST)
	$(CC) $(DNS_SERVER_OBJECTS) $(CFLAGS)  $(LFLAGS) -o $@

$(DIST)/dns-send: $(DNS_SEND_OBJECTS)  $(DIST)
	$(CC) $(DNS_SEND_OBJECTS) $(CFLAGS) $(LFLAGS) -o $@


$(ODIR):
	mkdir $(ODIR)

$(ODIR)/%.o: $(SRC)/%.c $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(ODIR) dist

rebuild: clean all

.PHONY : clean rebuild
.SILENT : clean