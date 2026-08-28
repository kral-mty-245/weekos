// WeeK OS - Process Scheduler
// Round-robin scheduling with priority

#include "../../include/kernel.h"

#define MAX_PROCESSES 256
#define STACK_SIZE    4096

// Process states
enum proc_state {
    PROC_UNUSED = 0,
    PROC_RUNNING,
    PROC_READY,
    PROC_BLOCKED,
    PROC_ZOMBIE
};

// Process Control Block
typedef struct {
    pid_t       pid;
    char        name[64];
    enum proc_state state;
    uint32_t    priority;     // 0=highest, 255=lowest
    uint32_t    stack[STACK_SIZE / 4];
    uint32_t    esp;          // Stack pointer
    uint32_t    eip;          // Instruction pointer
    uint32_t    cr3;          // Page directory
    uint32_t    time_slice;
    uint32_t    total_time;
    uid_t       uid;
    gid_t       gid;
} process_t;

static process_t processes[MAX_PROCESSES];
static process_t* current_process = NULL;
static pid_t next_pid = 1;

// Initialize scheduler
void scheduler_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROC_UNUSED;
    }
    
    // Create kernel idle process
    process_create("idle", NULL, 255);
}

// Create a new process
pid_t process_create(const char* name, entry_func_t entry, uint32_t priority) {
    // Find empty slot
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROC_UNUSED) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) return -1;
    
    process_t* proc = &processes[slot];
    
    // Initialize process
    proc->pid = next_pid++;
    strcpy(proc->name, name);
    proc->state = PROC_READY;
    proc->priority = priority;
    proc->time_slice = 100;
    proc->total_time = 0;
    proc->uid = 0;
    proc->gid = 0;
    
    // Set up stack
    if (entry) {
        proc->stack[STACK_SIZE / 4 - 1] = (uint32_t)entry;
        proc->esp = (uint32_t)&proc->stack[STACK_SIZE / 4 - 2];
    }
    
    return proc->pid;
}

// Exit a process
void process_exit(pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROC_ZOMBIE;
            return;
        }
    }
}

// Schedule next process
static void schedule(void) {
    // Find next ready process
    int next = -1;
    uint32_t best_prio = 0xFFFFFFFF;
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROC_READY && 
            processes[i].priority < best_prio) {
            best_prio = processes[i].priority;
            next = i;
        }
    }
    
    if (next == -1) {
        // No ready process, run idle
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].state == PROC_READY) {
                next = i;
                break;
            }
        }
    }
    
    if (next != -1) {
        current_process = &processes[next];
        current_process->state = PROC_RUNNING;
        current_process->time_slice = 100;
    }
}

// Timer interrupt handler (called by IRQ0)
void scheduler_tick(void) {
    if (!current_process) {
        schedule();
        return;
    }
    
    current_process->total_time++;
    
    if (current_process->time_slice > 0) {
        current_process->time_slice--;
    } else {
        // Time slice expired, reschedule
        current_process->state = PROC_READY;
        schedule();
    }
}

// Start the scheduler
void scheduler_start(void) {
    schedule();
}

// Get current process
process_t* scheduler_get_current(void) {
    return current_process;
}

// Get process by PID
process_t* scheduler_get_process(pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            return &processes[i];
        }
    }
    return NULL;
}

// Get process count
int scheduler_get_process_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROC_UNUSED) {
            count++;
        }
    }
    return count;
}
