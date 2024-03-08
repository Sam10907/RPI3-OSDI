TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc-8
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
SRC_DIR = src

LINKER_FILE = $(SRC_DIR)/linker.ld
SRCS_C = $(wildcard $(SRC_DIR)/*.c)
OBJS_C = $(SRCS_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/c/%.o)
SRCS_ASM = $(wildcard $(SRC_DIR)/*.S)
OBJS_ASM = $(SRCS_ASM:$(SRC_DIR)/%.S=$(BUILD_DIR)/asm/%.o)

CFLAGS = -Wall -I include -c -fno-stack-protector -fno-builtin

.PHONY: all clean

all: build_dir kernel8.img

# build

$(BUILD_DIR)/c/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/asm/%.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(OBJS_C) $(OBJS_ASM)
	$(LD) $(OBJS_C) $(OBJS_ASM) -T $(LINKER_FILE) -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img 

# run emulator

run: $(BUILD_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio

tty: $(BUILD_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial pty

asm:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -d in_asm

debug: all
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio -display none -S -s

# utility 

build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.elf *.img
