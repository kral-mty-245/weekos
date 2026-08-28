// WeeK OS - printf Implementation
// Minimal printf for kernel use

#include "../include/kernel.h"

// Buffer for formatted output
static char printf_buffer[1024];

// Format a string with variable arguments
int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int pos = 0;
    
    while (*format) {
        if (*format == '%') {
            format++;
            
            // Handle format specifiers
            switch (*format) {
                case 'd':
                case 'i': {
                    int value = va_arg(args, int);
                    char num_str[32];
                    itoa(value, num_str, 10);
                    for (int i = 0; num_str[i]; i++) {
                        str[pos++] = num_str[i];
                    }
                    break;
                }
                case 'u': {
                    unsigned int value = va_arg(args, unsigned int);
                    char num_str[32];
                    utoa(value, num_str, 10);
                    for (int i = 0; num_str[i]; i++) {
                        str[pos++] = num_str[i];
                    }
                    break;
                }
                case 'x': {
                    unsigned int value = va_arg(args, unsigned int);
                    char num_str[32];
                    utoa(value, num_str, 16);
                    for (int i = 0; num_str[i]; i++) {
                        str[pos++] = num_str[i];
                    }
                    break;
                }
                case 'p': {
                    void* value = va_arg(args, void*);
                    char num_str[32];
                    utoa((unsigned int)value, num_str, 16);
                    str[pos++] = '0';
                    str[pos++] = 'x';
                    for (int i = 0; num_str[i]; i++) {
                        str[pos++] = num_str[i];
                    }
                    break;
                }
                case 's': {
                    char* value = va_arg(args, char*);
                    if (value) {
                        while (*value) {
                            str[pos++] = *value++;
                        }
                    }
                    break;
                }
                case 'c': {
                    char value = (char)va_arg(args, int);
                    str[pos++] = value;
                    break;
                }
                case '%': {
                    str[pos++] = '%';
                    break;
                }
                default:
                    str[pos++] = '%';
                    str[pos++] = *format;
                    break;
            }
        } else {
            str[pos++] = *format;
        }
        format++;
    }
    
    str[pos] = '\0';
    va_end(args);
    
    return pos;
}

// Printf to VGA
int kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int len = sprintf(printf_buffer, format, args);
    
    vga_puts(printf_buffer, VGA_WHITE);
    
    va_end(args);
    return len;
}
