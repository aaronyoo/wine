CPP_SOURCES = $(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp)
INCLUDE_DIRS = src
BUILD_DIR = build
SRC_DIR = src
LINKABLE_OBJECT_FILES = $(BUILD_DIR)/boot.o $(BUILD_DIR)/system.o $(BUILD_DIR)/interrupts.o $(BUILD_DIR)/logger.o

$(info $$CPP_SOURCES is [${CPP_SOURCES}])

default: build

run: build
	qemu-system-i386 -display none -m 1G -serial stdio -kernel $(BUILD_DIR)/kernel.bin

mon: build
	qemu-system-i386 -m 4G -monitor stdio -kernel $(BUILD_DIR)/kernel.bin

debug: build
	qemu-system-i386 -display none -m 4G -s -S -serial stdio -kernel $(BUILD_DIR)/kernel.bin

debug-no-mon: build
	qemu-system-i386 -m 4G -s -S -serial stdio -kernel $(BUILD_DIR)/kernel.bin

build: asm_stubs linker.ld
	$(info $$LINKABLE_OBJECT_FILES is [${LINKABLE_OBJECT_FILES}])
	i386-elf-g++ -std=c++14 -fno-exceptions -g -I $(INCLUDE_DIRS) $(CPP_SOURCES) -T linker.ld $(LINKABLE_OBJECT_FILES) -o $(BUILD_DIR)/kernel.bin -nostdlib -ffreestanding

asm_stubs:
	nasm -f elf32 $(SRC_DIR)/kernel/boot.asm -o $(BUILD_DIR)/boot.o
	nasm -f elf32 $(SRC_DIR)/system/system.asm -o $(BUILD_DIR)/system.o
	nasm -f elf32 $(SRC_DIR)/system/interrupts.asm -o $(BUILD_DIR)/interrupts.o
	nasm -f elf32 $(SRC_DIR)/kernel/logger/logger.asm -o $(BUILD_DIR)/logger.o
