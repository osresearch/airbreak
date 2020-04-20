
# Big ol' party in here.
all: monitor_init.elf monitor_irq.elf monitor_init.bin monitor_irq.bin

# We find some spare room - you can do this automatically in the patch
# file, but requires a rebuild to do so...
monitor_irq.elf: monitor_irq.o
monitor_irq-offset := 0x800c600

#This one just replaces something else
monitor_init.elf: monitor_init.o
monitor_init-offset := 0x080c339a


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
	-DSTM32F407xx \
	-std=gnu99 \
	-I.

ASFLAGS ?= $(CFLAGS)

CFLAGS += -Wa,-adhlns=$(<:%.c=%.lst)

LDFLAGS ?= \
	--nostdlib \
	--no-dynamic-linker \
	--Ttext $($*-offset) \
	--entry start

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<
%.elf:
	$(LD) $(LDFLAGS) -o $@ $^

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

clean:
	$(RM) *.o *.elf *.bin *.lst

.PHONY: clean all