// WeeK OS - Kernel Heap Manager
// Simple heap allocator for kernel memory

#include "../../include/kernel.h"

#define HEAP_START 0x200000
#define HEAP_END   0x800000
#define HEAP_SIZE  (HEAP_END - HEAP_START)

// Heap block header
typedef struct block_header {
    uint32_t size;
    bool     free;
    struct block_header* next;
} block_header_t;

static block_header_t* heap_start_block = NULL;
static uint32_t heap_current = HEAP_START;

// Initialize heap
void heap_init(void) {
    heap_start_block = (block_header_t*)HEAP_START;
    heap_start_block->size = HEAP_SIZE - sizeof(block_header_t);
    heap_start_block->free = true;
    heap_start_block->next = NULL;
    
    heap_current = HEAP_START + sizeof(block_header_t);
}

// Split a block if it's too large
static void split_block(block_header_t* block, size_t size) {
    if (block->size >= size + sizeof(block_header_t) + 4) {
        block_header_t* new_block = (block_header_t*)((uint8_t*)block + sizeof(block_header_t) + size);
        new_block->size = block->size - size - sizeof(block_header_t);
        new_block->free = true;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

// Find a free block
static block_header_t* find_free_block(size_t size) {
    block_header_t* current = heap_start_block;
    
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

// Malloc
void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    block_header_t* block = find_free_block(size);
    
    if (!block) {
        // Extend heap
        if (heap_current + sizeof(block_header_t) + size > HEAP_END) {
            return NULL;
        }
        
        block = (block_header_t*)heap_current;
        block->size = size;
        block->free = false;
        block->next = NULL;
        
        // Add to list
        block_header_t* last = heap_start_block;
        while (last->next) last = last->next;
        last->next = block;
        
        heap_current += sizeof(block_header_t) + size;
    } else {
        block->free = false;
        split_block(block, size);
    }
    
    return (void*)((uint8_t*)block + sizeof(block_header_t));
}

// Calloc
void* kcalloc(size_t nmemb, size_t size) {
    void* ptr = kmalloc(nmemb * size);
    if (ptr) {
        uint8_t* p = (uint8_t*)ptr;
        for (size_t i = 0; i < nmemb * size; i++) {
            p[i] = 0;
        }
    }
    return ptr;
}

// Free
void kfree(void* ptr) {
    if (!ptr) return;
    
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    block->free = true;
    
    // Merge adjacent free blocks
    block_header_t* current = heap_start_block;
    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Get heap usage
uint32_t heap_get_used(void) {
    return heap_current - HEAP_START;
}

uint32_t heap_get_free(void) {
    return HEAP_END - heap_current;
}
