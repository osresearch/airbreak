../cpap/stm32-breath.bin: breath.bin
	./patch-airsense ../cpap/stm32.bin $@


CROSS ?= arm-none-eabi-
CC := $(CROSS)gcc
AS := $(CC)
LD := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

CFLAGS ?= \
	-g \
	-O3 \
	-mcpu=cortex-m4 \
	-mhard-float \
	-mthumb \
	-W \
	-Wall \
	-nostdlib \
	-nostdinc \

ASFLAGS ?= $(CFLAGS)

LDFLAGS ?= \
	--nostdlib \
	--no-dynamic-linker \
	--Ttext $($*-offset) \
	--entry start \

breath.elf: breath.o stubs.o
breath-offset := 0x80bb734

stubs.S: stm32.bin.xml
	./ghidra2stubs < $< > $@


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<
%.elf:
	$(LD) $(LDFLAGS) -o $@ $^

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

clean:
	$(RM) *.o stubs.S
