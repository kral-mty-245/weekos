# WeeK OS Build System
# C kernel + GNU/Linux tools + KDE Plasma-like desktop

CC = gcc
AS = nasm
LD = ld
CFLAGS = -ffreestanding -O2 -Wall -Wextra -Iinclude -m32
LDFLAGS = -m elf_i386 -T linker.ld
ASMFLAGS = -f elf32

# Kernel sources
KERNEL_SRC = kernel/main.c kernel/io.c kernel/string.c kernel/printf.c \
             kernel/mm/pmm.c kernel/mm/vmm.c kernel/mm/heap.c \
             kernel/sched/scheduler.c kernel/sched/process.c \
             kernel/irq/irq.c kernel/irq/timer.c \
             kernel/fs/vfs.c kernel/fs/ext2.c kernel/fs/ramdisk.c

# Driver sources  
DRIVER_SRC = drivers/vga/vga.c drivers/vga/framebuffer.c \
             drivers/keyboard/keyboard.c drivers/mouse/mouse.c \
             drivers/disk/ata.c drivers/disk/ide.c

# Userspace sources
USER_SRC = userspace/gui/desktop/desktop.c userspace/gui/settings/settings.c \
           userspace/gui/defender/defender.c userspace/gui/taskmanager/taskmanager.c

# Object files
KERNEL_OBJ = $(KERNEL_SRC:.c=.o)
DRIVER_OBJ = $(DRIVER_SRC:.c=.o)
USER_OBJ = $(USER_SRC:.c=.o)
ASM_SRC = boot/boot.asm kernel/entry.asm
ASM_OBJ = $(ASM_SRC:.asm=.o)

# Targets
all: weekos.iso

kernel.bin: $(ASM_OBJ) $(KERNEL_OBJ) $(DRIVER_OBJ) $(USER_OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $^

weekos.iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	cp boot/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o weekos.iso isodir

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(KERNEL_OBJ) $(DRIVER_OBJ) $(USER_OBJ) $(ASM_OBJ) kernel.bin weekos.iso

run: weekos.iso
	qemu-system-i386 -cdrom weekos.iso -m 512M -vga std

run-vbox: weekos.iso
	VBoxManage startvm "WeeK OS" --type gui

.PHONY: all clean run run-vbox
