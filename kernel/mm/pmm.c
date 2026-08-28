// WeeK OS - Physical Memory Manager
// Manages physical memory pages

#include "../../include/kernel.h"

#define PAGE_SIZE 4096
#define MAX_PAGES 131072  // 512MB / 4KB

// Bitmap for tracking free pages
static uint32_t physical_bitmap[MAX_PAGES / 32];
static uint32_t total_pages = 0;
static uint32_t free_pages = 0;

// Initialize PMM
void pmm_init(uint32_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    free_pages = total_pages;
    
    // Mark all pages as free
    for (uint32_t i = 0; i < MAX_PAGES / 32; i++) {
        physical_bitmap[i] = 0xFFFFFFFF;
    }
    
    // Mark first 1MB as used (kernel space)
    uint32_t used_pages = 0x100000 / PAGE_SIZE;  // 1MB
    for (uint32_t i = 0; i < used_pages; i++) {
        pmm_free_page(i * PAGE_SIZE);
        free_pages--;
    }
}

// Allocate a physical page
void* pmm_alloc_page(void) {
    for (uint32_t i = 0; i < MAX_PAGES / 32; i++) {
        if (physical_bitmap[i] != 0) {
            for (int j = 0; j < 32; j++) {
                if (physical_bitmap[i] & (1 << j)) {
                    physical_bitmap[i] &= ~(1 << j);
                    free_pages--;
                    return (void*)(i * 32 * PAGE_SIZE + j * PAGE_SIZE);
                }
            }
        }
    }
    return NULL;  // Out of memory
}

// Free a physical page
void pmm_free_page(void* addr) {
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    physical_bitmap[page / 32] |= (1 << (page % 32));
    free_pages++;
}

// Get free memory info
uint32_t pmm_get_free_pages(void) {
    return free_pages;
}

uint32_t pmm_get_total_pages(void) {
    return total_pages;
}

// kmalloc/kfree using PMM
static uint32_t heap_start = 0x200000;  // 2MB
static uint32_t heap_end = 0x200000;
static uint32_t heap_size = 0;

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Simple bump allocator
    void* ptr = (void*)heap_end;
    heap_end += size;
    
    // Align to 4 bytes
    heap_end = (heap_end + 3) & ~3;
    
    return ptr;
}

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

void kfree(void* ptr) {
    // Simple heap - no-op for now
    // In a real kernel, this would free the memory
    (void)ptr;
}
