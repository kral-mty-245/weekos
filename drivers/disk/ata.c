// WeeK OS - ATA/IDE Disk Driver
// Primary ATA controller driver

#include "../../include/kernel.h"

#define ATA_PRIMARY_IO      0x1F0
#define ATA_PRIMARY_CTRL    0x3F6
#define ATA_DATA            0
#define ATA_ERROR           1
#define ATA_SECTOR_COUNT    2
#define ATA_LBA_LO          3
#define ATA_LBA_MID         4
#define ATA_LBA_HI          5
#define ATA_DRIVE           6
#define ATA_STATUS          7
#define ATA_COMMAND         7

// ATA commands
#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_WRITE_PIO   0x30
#define ATA_CMD_IDENTIFY    0xEC

// Status bits
#define ATA_SR_BSY   0x80
#define ATA_SR_DRDY  0x40
#define ATA_SR_DRQ   0x08
#define ATA_SR_ERR   0x01

static int ata_initialized = 0;
static uint32_t ata_sector_size = 512;

// Wait for drive ready
static void ata_wait_ready(void) {
    while (inb(ATA_PRIMARY_IO + ATA_STATUS) & ATA_SR_BSY);
}

// Wait for data request
static void ata_wait_drq(void) {
    while (!(inb(ATA_PRIMARY_IO + ATA_STATUS) & ATA_SR_DRQ));
}

// Select drive
static void ata_select_drive(uint8_t drive) {
    outb(ATA_PRIMARY_IO + ATA_DRIVE, 0xA0 | (drive << 4));
    io_wait();
}

// Initialize ATA
int disk_init(void) {
    // Select master drive
    ata_select_drive(0);
    
    // Send IDENTIFY command
    outb(ATA_PRIMARY_IO + ATA_COMMAND, ATA_CMD_IDENTIFY);
    io_wait();
    
    // Check if drive exists
    uint8_t status = inb(ATA_PRIMARY_IO + ATA_STATUS);
    if (status == 0) {
        ata_initialized = 0;
        return -1;  // No drive
    }
    
    // Wait for drive
    ata_wait_ready();
    
    // Read identification space
    uint16_t ident[256];
    for (int i = 0; i < 256; i++) {
        ident[i] = inw(ATA_PRIMARY_IO + ATA_DATA);
    }
    
    // Get sector size
    ata_sector_size = 512;  // Standard
    
    ata_initialized = 1;
    return 0;
}

// Read sectors
int disk_read_sectors(uint32_t lba, uint8_t count, void* buffer) {
    if (!ata_initialized) return -1;
    
    // Set up registers
    outb(ATA_PRIMARY_IO + ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + ATA_SECTOR_COUNT, count);
    outb(ATA_PRIMARY_IO + ATA_LBA_LO, lba & 0xFF);
    outb(ATA_PRIMARY_IO + ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_IO + ATA_LBA_HI, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_IO + ATA_COMMAND, ATA_CMD_READ_PIO);
    
    // Read data
    uint16_t* buf = (uint16_t*)buffer;
    for (int i = 0; i < count; i++) {
        ata_wait_ready();
        ata_wait_drq();
        
        for (int j = 0; j < 256; j++) {
            buf[j] = inw(ATA_PRIMARY_IO + ATA_DATA);
        }
        buf += 256;
    }
    
    return count;
}

// Write sectors
int disk_write_sectors(uint32_t lba, uint8_t count, const void* buffer) {
    if (!ata_initialized) return -1;
    
    // Set up registers
    outb(ATA_PRIMARY_IO + ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + ATA_SECTOR_COUNT, count);
    outb(ATA_PRIMARY_IO + ATA_LBA_LO, lba & 0xFF);
    outb(ATA_PRIMARY_IO + ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_IO + ATA_LBA_HI, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_IO + ATA_COMMAND, ATA_CMD_WRITE_PIO);
    
    // Write data
    const uint16_t* buf = (const uint16_t*)buffer;
    for (int i = 0; i < count; i++) {
        ata_wait_ready();
        ata_wait_drq();
        
        for (int j = 0; j < 256; j++) {
            outw(ATA_PRIMARY_IO + ATA_DATA, buf[j]);
        }
        buf += 256;
        
        // Flush cache
        outb(ATA_PRIMARY_IO + ATA_COMMAND, 0xE7);
    }
    
    return count;
}
