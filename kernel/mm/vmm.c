// WeeK OS - Virtual Memory Manager
// Page directory and page table management

#include "../../include/kernel.h"

#define PAGE_SIZE 4096
#define PAGE_PRESENT 0x01
#define PAGE_WRITE   0x02
#define PAGE_USER    0x04

// Page directory entry
typedef struct {
    uint32_t present    : 1;
    uint32_t write      : 1;
    uint32_t user       : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;


// Page table entry
typedef struct {
    uint32_t present    : 1;
    uint32_t write      : 1;
    uint32_t user       : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;
} page_table_entry_t;

// Page directory
static page_directory_entry_t page_directory[1024] __attribute__((aligned(4096)));
static page_table_entry_t page_tables[1024][1024] __attribute__((aligned(4096)));

// Initialize virtual memory
void vmm_init(void) {
    // Clear page directory
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }
    
    // Identity map first 4MB (kernel space)
    for (uint32_t addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        vmm_map_page(addr, addr, PAGE_PRESENT | PAGE_WRITE);
    }
    
    // Load page directory
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));
    
    // Enable paging
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set PG bit
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

// Map a virtual page to a physical frame
void vmm_map_page(uint32_t virtual, uint32_t physical, uint32_t flags) {
    uint32_t pd_index = virtual >> 22;
    uint32_t pt_index = (virtual >> 12) & 0x3FF;
    
    // Create page table if needed
    if (!(page_directory[pd_index].present)) {
        uint32_t pt_addr = (uint32_t)&page_tables[pd_index];
        page_directory[pd_index].present = 1;
        page_directory[pd_index].write = 1;
        page_directory[pd_index].user = 1;
        page_directory[pd_index].frame = pt_addr >> 12;
    }
    
    // Map the page
    page_tables[pd_index][pt_index].present = 1;
    page_tables[pd_index][pt_index].write = (flags & PAGE_WRITE) ? 1 : 0;
    page_tables[pd_index][pt_index].user = (flags & PAGE_USER) ? 1 : 0;
    page_tables[pd_index][pt_index].frame = physical >> 12;
}

// Unmap a virtual page
void vmm_unmap_page(uint32_t virtual) {
    uint32_t pd_index = virtual >> 22;
    uint32_t pt_index = (virtual >> 12) & 0x3FF;
    
    page_tables[pd_index][pt_index].present = 0;
    page_tables[pd_index][pt_index].frame = 0;
    
    // Flush TLB
    asm volatile("invlpg (%0)" :: "r"(virtual));
}

// Get physical address from virtual
uint32_t vmm_get_physical(uint32_t virtual) {
    uint32_t pd_index = virtual >> 22;
    uint32_t pt_index = (virtual >> 12) & 0x3FF;
    
    if (!page_directory[pd_index].present) return 0;
    if (!page_tables[pd_index][pt_index].present) return 0;
    
    return (page_tables[pd_index][pt_index].frame << 12) | (virtual & 0xFFF);
}

// Switch page directory
void vmm_switch_directory(uint32_t* new_directory) {
    asm volatile("mov %0, %%cr3" :: "r"(new_directory));
}
