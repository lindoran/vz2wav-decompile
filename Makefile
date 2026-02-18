CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm
TARGETS = vz2wav wav2vz text2bas-vz text2bas-cg

all: $(TARGETS)

vz2wav: vz2wav.c
	$(CC) $(CFLAGS) -o vz2wav vz2wav.c $(LDFLAGS)

text2bas-vz: text2bas.c
	$(CC) $(CFLAGS) -DCGENIE=0 -o text2bas-vz text2bas.c

text2bas-cg: text2bas.c
	$(CC) $(CFLAGS) -DCGENIE=1 -o text2bas-cg text2bas.c

clean:
	rm -f $(TARGETS) *.o

install: $(TARGETS)
	install -m 755 vz2wav /usr/local/bin/
	install -m 755 wav2vz /usr/local/bin/
	install -m 755 text2bas-vz /usr/local/bin/
	install -m 755 text2bas-cg /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/vz2wav /usr/local/bin/wav2vz /usr/local/bin/text2bas-vz /usr/local/bin/text2bas-cg

.PHONY: all clean install uninstall
