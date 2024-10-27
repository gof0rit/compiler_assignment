CC = ~/Software/gnu/bin/riscv32-unknown-elf-gcc
QEMU = ~/Software/gnu/bin/qemu-riscv32

all: test

test: main.c test.s
	$(CC) test.s main.c -o test
	$(QEMU) -B 0x10000000 test

test.s: test.c
	../pcc -S test.c

clean:
	rm -f test test.s
