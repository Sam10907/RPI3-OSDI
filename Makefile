TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
SRC_DIR = src
USER_DIR = user

LINKER_FILE = $(SRC_DIR)/linker.ld
SRCS_C = $(wildcard $(SRC_DIR)/*.c)
OBJS_C = $(SRCS_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/c/%.o)
SRCS_ASM = $(wildcard $(SRC_DIR)/*.S)
OBJS_ASM = $(SRCS_ASM:$(SRC_DIR)/%.S=$(BUILD_DIR)/asm/%.o)

USER_C = $(wildcard $(USER_DIR)/*.c)
USER_ASM = $(wildcard $(USER_DIR)/*.S)
USER_OBJS_FILES = $(USER_C:$(USER_DIR)/%.c=$(BUILD_DIR)/user/%_c.o)
USER_OBJS_FILES += $(USER_ASM:$(USER_DIR)/%.S=$(BUILD_DIR)/user/%_asm.o)

CFLAGS =  -Wall -Wextra -nostdlib -nostdinc -fno-builtin-printf -fno-builtin-memcpy -fno-builtin-exit -Iinclude -Ilib -c

.PHONY: all clean

all: build_dir kernel8.img

# build

$(BUILD_DIR)/c/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/asm/%.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(OBJS_C) $(OBJS_ASM) user_embed.elf
	$(LD) $(OBJS_C) $(OBJS_ASM) user_embed.elf -T $(LINKER_FILE) -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img 

# build user library

$(BUILD_DIR)/user/%_c.o: $(USER_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -fno-zero-initialized-in-bss $< -o $@

$(BUILD_DIR)/user/%_asm.o: $(USER_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -fno-zero-initialized-in-bss $< -o $@

user_embed.elf: $(USER_OBJS_FILES) build/asm/Sys.o build/c/my_string.o
	$(LD) $(USER_OBJS_FILES) build/asm/Sys.o build/c/my_string.o -T $(USER_DIR)/linker.ld -o user.elf
	$(OBJCPY) -O binary user.elf user.img
	$(LD) -r -b binary user.img -o user_embed.elf

# run emulator

run: $(BUILD_DIR) kernel8.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -initrd initramfs.cpio -display none -serial null -serial stdio

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
