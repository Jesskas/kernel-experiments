ARMGNU ?= arm-none-eabi
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding
gcc : kernel.img
all : gcc clang

SOURCE = 	periph/periph.c \
					mailbox/mailbox.c \
					fb/fb.c \

clean :
	rm -f *.o
	rm -f *.hex
	rm -f *.bin
	rm -f *.elf
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.s

start.o : start.s
	$(ARMGNU)-as start.s -o start.o

kernel.o : kernel.c
	$(ARMGNU)-gcc $(COPS) -c kernel.c -o kernel.o

periph.o : periph/periph.c
	$(ARMGNU)-gcc $(COPS) -c periph/periph.c -o periph.o

mailbox.o : mailbox/mailbox.c
	$(ARMGNU)-gcc $(COPS) -c mailbox/mailbox.c -o mailbox.o

fb.o : fb/fb.c
	$(ARMGNU)-gcc $(COPS) -c fb/fb.c -o fb.o

kernel.img : linker.ld start.o periph.o mailbox.o fb.o kernel.o
	$(ARMGNU)-ld start.o periph.o mailbox.o fb.o kernel.o -T linker.ld -o kernel.elf
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
