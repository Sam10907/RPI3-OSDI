TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc-8
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

SRC_DIR = src
OUT_DIR = out

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/start.S
ENTRY_OBJS = $(OUT_DIR)/start.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)

CFLAGS = -Wall -I include -c -fno-stack-protector -fno-builtin

.PHONY: all clean asm run debug directories

all: directories kernel8.img

$(ENTRY_OBJS): $(ENTRY)
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(OBJS) $(ENTRY_OBJS)
	$(LD) $(ENTRY_OBJS) $(OBJS) -T $(LINKER_FILE) -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

asm:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -d in_asm

run: all
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial pty

debug: all
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial pty -display none -S -s

directories: $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -f out/* kernel8.*
