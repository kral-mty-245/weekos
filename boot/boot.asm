; WeeK OS Bootloader
; GRUB Multiboot compatible

[bits 32]
[global _start]
[extern kernel_main]

; Multiboot constants
MBOOT_MAGIC     equ 0x1BADB002
MBOOT_MEMINFO   equ 0x00000003
MBOOT_VIDMODE   equ 0x00000004

; Multiboot header
section .multiboot
    align 4
    dd MBOOT_MAGIC
    dd MBOOT_MEMINFO | MBOOT_VIDMODE
    dd -(MBOOT_MAGIC + (MBOOT_MEMINFO | MBOOT_VIDMODE))
    
    ; a.out kludge
    dd 0, 0, 0, 0, 0
    
    ; Video mode
    dd 0    ; linear graphics mode
    dd 1920 ; width
    dd 1080 ; height
    dd 32   ; bpp

; Stack
section .bss
    align 16
    stack_bottom:
        resb 65536  ; 64KB stack
    stack_top:

; Entry point
section .text
_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Push multiboot info
    push eax        ; Magic number
    push ebx        ; Multiboot info pointer
    
    ; Call kernel
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang

; GDT setup (basic)
section .data
gdt_start:
    dd 0x0          ; Null descriptor
    dd 0x0
    
gdt_code:           ; Code segment
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags, Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_data:           ; Data segment
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Function to load GDT
[global gdt_load]
gdt_load:
    lgdt [gdt_descriptor]
    mov ax, 0x10    ; Data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.code_segment
    
.code_segment:
    ret
