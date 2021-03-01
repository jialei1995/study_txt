CROSS = arm-none-linux-gnueabi-
CC = $(CROSS)gcc
LD = $(CROSS)ld 
OBJCOPY = $(CROSS)objcopy 

all:
	$(CC) -g -c -o start.o start.s

	$(CC) -g -c -o main.o main.c
	
	$(LD) start.o main.o -Tmap.lds -o interrupt.elf

	$(OBJCOPY) -O binary -S interrupt.elf interrupt.bin

clean:
	rm -f *.o *.elf *.bin *.dis
