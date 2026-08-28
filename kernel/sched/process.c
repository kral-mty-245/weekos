// WeeK OS - Process Management
// Process creation, deletion, and management

#include "../../include/kernel.h"

#define MAX_PROCESSES 256

// Process states
typedef enum {
    PROC_STATE_UNUSED = 0,
    PROC_STATE_READY,
    PROC_STATE_RUNNING,
    PROC_STATE_BLOCKED,
    PROC_STATE_ZOMBIE
} process_state_t;

// Process structure
typedef struct process {
    pid_t           pid;
    char            name[64];
    process_state_t state;
    uint32_t        priority;
    uint32_t        stack[1024];
    uint32_t        esp;
    uint32_t        eip;
    uint32_t        cr3;
    uint32_t        time_slice;
    uint32_t        cpu_time;
    uid_t           uid;
    gid_t           gid;
    struct process* next;
} process_internal_t;

static process_internal_t processes[MAX_PROCESSES];
static int process_count = 0;
static pid_t next_pid = 1;

// Initialize process management
void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].state = PROC_STATE_UNUSED;
    }
    
    // Create kernel process (PID 0)
    processes[0].pid = 0;
    strcpy(processes[0].name, "kernel");
    processes[0].state = PROC_STATE_RUNNING;
    processes[0].priority = 0;
    processes[0].uid = 0;
    processes[0].gid = 0;
    process_count = 1;
    next_pid = 1;
}

// Create new process
pid_t process_create_new(const char* name, entry_func_t entry, uint32_t priority) {
    if (process_count >= MAX_PROCESSES) return -1;
    
    process_internal_t* proc = &processes[process_count];
    
    proc->pid = next_pid++;
    strcpy(proc->name, name);
    proc->state = PROC_STATE_READY;
    proc->priority = priority;
    proc->time_slice = 100;
    proc->cpu_time = 0;
    proc->uid = 1000;
    proc->gid = 1000;
    
    // Set up stack
    if (entry) {
        proc->stack[1023] = (uint32_t)entry;
        proc->esp = (uint32_t)&proc->stack[1022];
    }
    
    process_count++;
    return proc->pid;
}

// Exit process
void process_exit_new(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROC_STATE_ZOMBIE;
            process_count--;
            
            // Shift array
            for (int j = i; j < process_count; j++) {
                processes[j] = processes[j + 1];
            }
            break;
        }
    }
}

// Get process by PID
process_internal_t* process_get_by_pid(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            return &processes[i];
        }
    }
    return NULL;
}

// Get process count
int process_get_count(void) {
    return process_count;
}

// Get all processes
process_internal_t* process_get_all(void) {
    return processes;
}
