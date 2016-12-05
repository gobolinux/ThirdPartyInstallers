CC       = gcc
MYCFLAGS = -O3 -Wall
TARGETS  = rpminfo

# first rule
default: all

.PHONY: all default

all: $(TARGETS)

rpminfo: rpminfo.c
	$(CC) $(MYCFLAGS) $^ -o $@ -lrpm -lrpmdb -lrpmio -I/usr/include/rpm

debug: MYCFLAGS = -g -ggdb -O0 -DDEBUG -Wall
debug: all

clean:
	rm -f $(TARGETS)

.PHONY: all clean debug install
