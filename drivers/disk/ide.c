// WeeK OS - IDE Controller
// Secondary IDE controller support

#include "../../include/kernel.h"

#define IDE_SECONDARY_IO   0x170
#define IDE_SECONDARY_CTRL 0x376

// Initialize secondary IDE
int ide_init_secondary(void) {
    // Check for secondary controller
    outb(IDE_SECONDARY_IO + 7, 0xA0);
    io_wait();
    
    uint8_t status = inb(IDE_SECONDARY_IO + 7);
    if (status == 0) {
        return -1;  // No secondary controller
    }
    
    return 0;
}

// Read from secondary controller
int ide_read_secondary(uint32_t lba, uint8_t count, void* buffer) {
    // Similar to ATA read but on secondary port
    outb(IDE_SECONDARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IDE_SECONDARY_IO + 2, count);
    outb(IDE_SECONDARY_IO + 3, lba & 0xFF);
    outb(IDE_SECONDARY_IO + 4, (lba >> 8) & 0xFF);
    outb(IDE_SECONDARY_IO + 5, (lba >> 16) & 0xFF);
    outb(IDE_SECONDARY_IO + 7, 0x20);
    
    uint16_t* buf = (uint16_t*)buffer;
    for (int i = 0; i < count; i++) {
        while (inb(IDE_SECONDARY_IO + 7) & 0x80);
        for (int j = 0; j < 256; j++) {
            buf[j] = inw(IDE_SECONDARY_IO);
        }
        buf += 256;
    }
    
    return count;
}
