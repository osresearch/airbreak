all: stm32-unlocked.bin

stm32-unlocked.bin: breath.bin graph.bin
	./patch-airsense ../cpap/stm32.bin $@

serve:
	mkdocs serve
deploy:
	mkdocs gh-deploy

# The breath extension replaces the function at 0x80bb734 with
# a simple on/off timer for alternating between two pressures.
# It can't be too long or it will overlap another function.
#
# TODO: add a size check
#
# To add another extension at a different address in the firmware,
# define a .elf target and a variable with the offset that it will
# be patched into the image.
breath.elf: breath.o stubs.o
breath-offset := 0x80bb734

#
# The graphing is too large to fit directly in the location at 0x8067d2c,
# so it is in high in the flash and the function pointer is fixed up at 0x80f9c88
graph.elf: graph.o stubs.o
graph-offset := 0x80fd000

# If there is a new version of the ghidra XML, the stubs.S
# file will be regenerated so that the addresses and functions
# are at the correct address in the ELF image.
stubs.S: stm32.bin.xml
	./ghidra2stubs < $< > $@


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
