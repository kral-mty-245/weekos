// WeeK OS - String Functions
// Standard string library implementation

#include "../include/kernel.h"

// String length
size_t strnlen(const char* str, size_t maxlen) {
    size_t len = 0;
    while (str[len] && len < maxlen) len++;
    return len;
}

// Compare strings
int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
        if (s1[i] == '\0') return 0;
    }
    return 0;
}

// Find character in string
char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) return (char*)str;
        str++;
    }
    return NULL;
}

// Find last occurrence of character
char* strrchr(const char* str, int c) {
    const char* last = NULL;
    while (*str) {
        if (*str == (char)c) last = str;
        str++;
    }
    return (char*)last;
}

// Tokenize string
char* strtok(char* str, const char* delim) {
    static char* last_token = NULL;
    
    if (str) last_token = str;
    if (!last_token) return NULL;
    
    // Skip leading delimiters
    while (*last_token) {
        const char* d = delim;
        bool is_delim = false;
        while (*d) {
            if (*last_token == *d) {
                is_delim = true;
                break;
            }
            d++;
        }
        if (!is_delim) break;
        last_token++;
    }
    
    if (*last_token == '\0') return NULL;
    
    char* token = last_token;
    
    // Find end of token
    while (*last_token) {
        const char* d = delim;
        while (*d) {
            if (*last_token == *d) {
                *last_token = '\0';
                last_token++;
                return token;
            }
            d++;
        }
        last_token++;
    }
    
    return token;
}

// Memory set
void* memset(void* s, int c, size_t n) {
    uint8_t* p = (uint8_t*)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }
    return s;
}

// Memory copy
void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

// Memory compare
int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

// Integer to string
char* itoa(int value, char* str, int base) {
    char* rc;
    char* ptr;
    char* low;
    
    // Check for supported base
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    rc = ptr = str;
    
    // Set '-' for negative numbers
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        value = -value;
    }
    
    // Convert to string
    low = ptr;
    do {
        int remainder = value % base;
        *ptr++ = (remainder < 10) ? '0' + remainder : 'a' + remainder - 10;
    } while (value /= base);
    
    // Terminate string
    *ptr-- = '\0';
    
    // Reverse string
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    
    return rc;
}

// Unsigned integer to string
char* utoa(unsigned int value, char* str, int base) {
    char* rc;
    char* ptr;
    char* low;
    
    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    rc = ptr = str;
    low = ptr;
    
    do {
        unsigned int remainder = value % base;
        *ptr++ = (remainder < 10) ? '0' + remainder : 'a' + remainder - 10;
    } while (value /= base);
    
    *ptr-- = '\0';
    
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    
    return rc;
}
