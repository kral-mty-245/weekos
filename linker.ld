/* WeeK OS Linker Script */
/* Links kernel and drivers into a single binary */

ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1MB (standard for GRUB) */
    . = 1M;
    
    /* Multiboot header must be first */
    .multiboot : {
        *(.multiboot)
    }
    
    /* Code section */
    .text : {
        *(.text)
        *(.text.*)
    }
    
    /* Read-only data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Initialized data */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* Uninitialized data (BSS) */
    .bss : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }
    
    /* Kernel end address */
    __kernel_end = .;
    
    /* Stack */
    .stack : {
        *(.stack)
    }
    
    /* Discard unwanted sections */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
