// WeeK OS - Virtual File System
// Abstract file system layer

#include "../../include/kernel.h"

#define MAX_VFS_NODES 256

// VFS node types
#define VFS_FILE      0
#define VFS_DIRECTORY 1
#define VFS_SYMLINK   2
#define VFS_CHARDEV   3
#define VFS_BLOCKDEV  4

// VFS operations
typedef struct vfs_ops {
    int  (*read)(int fd, void* buffer, uint32_t size);
    int  (*write)(int fd, const void* buffer, uint32_t size);
    int  (*open)(const char* path, int flags);
    void (*close)(int fd);
    int  (*mkdir)(const char* path);
    int  (*readdir)(const char* path, vfs_node_t* entries, uint32_t max_entries);
} vfs_ops_t;

// VFS node
typedef struct vfs_node {
    char       name[256];
    uint32_t   inode;
    uint32_t   type;
    uint32_t   size;
    uint32_t   permissions;
    uint32_t   uid;
    uint32_t   gid;
    vfs_ops_t* ops;
    void*      data;
} vfs_node_internal_t;

static vfs_node_internal_t vfs_nodes[MAX_VFS_NODES];
static int vfs_node_count = 0;
static int vfs_initialized = 0;

// Root directory
static vfs_node_internal_t root_node;

// Initialize VFS
int vfs_init(void) {
    // Clear all nodes
    for (int i = 0; i < MAX_VFS_NODES; i++) {
        vfs_nodes[i].name[0] = '\0';
    }
    
    // Create root directory
    strcpy(root_node.name, "/");
    root_node.inode = 1;
    root_node.type = VFS_DIRECTORY;
    root_node.size = 0;
    root_node.permissions = 0755;
    
    // Create default directories
    vfs_mkdir("/home");
    vfs_mkdir("/home/user");
    vfs_mkdir("/home/user/Desktop");
    vfs_mkdir("/home/user/Documents");
    vfs_mkdir("/home/user/Downloads");
    vfs_mkdir("/home/user/Pictures");
    vfs_mkdir("/home/user/Music");
    vfs_mkdir("/home/user/Videos");
    vfs_mkdir("/home/user/.config");
    vfs_mkdir("/home/user/.local");
    vfs_mkdir("/tmp");
    vfs_mkdir("/var");
    vfs_mkdir("/var/log");
    vfs_mkdir("/etc");
    vfs_mkdir("/usr");
    vfs_mkdir("/usr/bin");
    vfs_mkdir("/usr/lib");
    vfs_mkdir("/opt");
    vfs_mkdir("/sys");
    vfs_mkdir("/proc");
    vfs_mkdir("/dev");
    vfs_mkdir("/boot");
    vfs_mkdir("/trash");
    
    vfs_initialized = 1;
    return 0;
}

// Create directory
int vfs_mkdir(const char* path) {
    if (vfs_node_count >= MAX_VFS_NODES) return -1;
    
    vfs_node_internal_t* node = &vfs_nodes[vfs_node_count];
    
    // Extract filename from path
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        strcpy(node->name, last_slash + 1);
    } else {
        strcpy(node->name, path);
    }
    
    node->inode = vfs_node_count + 2;
    node->type = VFS_DIRECTORY;
    node->size = 0;
    node->permissions = 0755;
    
    vfs_node_count++;
    return 0;
}

// Create file
int vfs_create(const char* path, uint32_t permissions) {
    if (vfs_node_count >= MAX_VFS_NODES) return -1;
    
    vfs_node_internal_t* node = &vfs_nodes[vfs_node_count];
    
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        strcpy(node->name, last_slash + 1);
    } else {
        strcpy(node->name, path);
    }
    
    node->inode = vfs_node_count + 2;
    node->type = VFS_FILE;
    node->size = 0;
    node->permissions = permissions;
    
    vfs_node_count++;
    return 0;
}

// Open file
int vfs_open(const char* path, int flags) {
    // Find node by path
    for (int i = 0; i < vfs_node_count; i++) {
        if (strcmp(vfs_nodes[i].name, path) == 0) {
            return i;
        }
    }
    
    // If not found and O_CREAT flag, create it
    if (flags & 0x100) {  // O_CREAT
        return vfs_create(path, 0644);
    }
    
    return -1;  // Not found
}

// Close file
void vfs_close(int fd) {
    // Nothing to do for now
    (void)fd;
}

// Read file
int vfs_read(const char* path, void* buffer, uint32_t size) {
    int fd = vfs_open(path, 0);
    if (fd < 0) return -1;
    
    // In real implementation, this would read from disk
    // For now, return 0 bytes read
    return 0;
}

// Write file
int vfs_write(const char* path, const void* buffer, uint32_t size) {
    int fd = vfs_open(path, 0x200);  // O_WRONLY | O_CREAT
    if (fd < 0) return -1;
    
    vfs_nodes[fd].size = size;
    return size;
}

// List directory
int vfs_readdir(const char* path, vfs_node_t* entries, uint32_t max_entries) {
    int count = 0;
    
    for (int i = 0; i < vfs_node_count && count < max_entries; i++) {
        if (vfs_nodes[i].name[0] != '\0') {
            strcpy(entries[count].name, vfs_nodes[i].name);
            entries[count].size = vfs_nodes[i].size;
            entries[count].type = vfs_nodes[i].type;
            entries[count].inode = vfs_nodes[i].inode;
            count++;
        }
    }
    
    return count;
}

// Get node count
int vfs_get_node_count(void) {
    return vfs_node_count;
}
