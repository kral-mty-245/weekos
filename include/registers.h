#ifndef WEEKOS_REGISTERS_H
#define WEEKOS_REGISTERS_H

#include "types.h"

// CPU registers structure (pushed by ISR stubs)
typedef struct {
    uint32_t ds;                                     // Data segment
    uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                       // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss;          // Pushed by CPU
} registers_t;

#endif
