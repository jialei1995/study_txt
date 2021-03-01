CROSS = arm-none-linux-gnueabi-
CC = $(CROSS)gcc
LD = $(CROSS)ld 
OBJCOPY = $(CROSS)objcopy 

led.bin:led.elf
	$(OBJCOPY) -O binary -S $< $@
led.elf:start.o main.o
	$(LD) $^ -Tmap.lds -o $@ $<
main.o:main.c
	$(CC) -g -c -o $@ $<
start.o:start.s
	$(CC) -g -c -o $@ $<
clean:
	rm -f *.o *.elf *.bin *.dis
