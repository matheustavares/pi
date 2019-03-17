CC=gcc
CCFLAGS=-Wall -O2
LDFLAGS=-lm -lpthread

pi: pi.c
	$(CC) $(CCFLAGS) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f pi
