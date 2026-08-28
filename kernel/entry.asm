; WeeK OS Kernel Entry Point
; Assembly stubs for interrupt handlers

[bits 32]
[extern kernel_main]

; Macro for interrupt stubs
%macro ISR_NOERRCODE 1
[global isr%1]
isr%1:
    cli
    push dword 0        ; Dummy error code
    push dword %1       ; Interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
[global isr%1]
isr%1:
    cli
    push dword %1       ; Interrupt number
    jmp isr_common_stub
%endmacro

%macro IRQ 2
[global irq%1]
irq%1:
    cli
    push dword 0        ; Dummy error code
    push dword %2       ; Interrupt number
    jmp irq_common_stub
%endmacro

; ISR stubs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; IRQ stubs
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; ISR common stub
isr_common_stub:
    pusha               ; Push all general purpose registers
    
    mov ax, ds
    push eax            ; Push data segment
    
    mov ax, 0x10        ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C handler
    push esp            ; Push registers_t pointer
    ; call isr_handler  ; Uncomment when ISR handler is implemented
    add esp, 4          ; Clean up
    
    pop eax             ; Pop data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                ; Pop all general purpose registers
    add esp, 8          ; Clean up error code and interrupt number
    sti
    iret

; IRQ common stub
irq_common_stub:
    pusha
    
    mov ax, ds
    push eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp
    ; call irq_handler  ; Uncomment when IRQ handler is implemented
    add esp, 4
    
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa
    add esp, 8
    sti
    iret

; Global descriptor table
[global gdt_flush]
gdt_flush:
    lgdt [esp+4]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

; Load IDT
[global idt_load]
idt_load:
    push ebp
    mov ebp, esp
    lidt [ebp+8]
    pop ebp
    ret
