// WeeK OS - I/O Functions
// Port I/O and basic I/O operations

#include "../include/kernel.h"

// Port I/O functions are inline in kernel.h
// This file provides additional I/O utilities

// Read a word from port
uint16_t port_word_in(uint16_t port) {
    uint16_t result;
    asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write a word to port
void port_word_out(uint16_t port, uint16_t data) {
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

// Read a dword from port
uint32_t port_dword_in(uint16_t port) {
    uint32_t result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write a dword to port
void port_dword_out(uint16_t port, uint32_t data) {
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}

// Wait for I/O
void io_wait(void) {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

// Disable interrupts
void cli(void) {
    asm volatile("cli");
}

// Enable interrupts
void sti(void) {
    asm volatile("sti");
}

// Halt CPU
void hlt(void) {
    asm volatile("hlt");
}

// Read CR2 (faulting address)
uint32_t read_cr2(void) {
    uint32_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    return cr2;
}

// Read CR3 (page directory)
uint32_t read_cr3(void) {
    uint32_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// Write CR3
void write_cr3(uint32_t cr3) {
    asm volatile("mov %0, %%cr3" : : "r"(cr3));
}
