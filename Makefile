NASM?=nasm
XXD?=xxd

CFLAGS+=-Wall -Wextra -c -fPIC


.PHONY: all clean

all: remap_annon.h remap_annon.o

remap_annon.o: remap_annon.c remap_annon.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

remap_annon.h: remap_annon.bin
	$(XXD) -i $< $@

remap_annon.bin: remap_annon.asm
	$(NASM) -f bin $< -o $@

clean:
	rm remap_annon.h remap_annon.bin remap_annon.o
