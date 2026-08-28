// WeeK OS - Terminal Applications
// WeeK Terminal (Bash) + WeeK Shell (PowerShell-like)

#define MAX_PATH 260
#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Terminal State
// ============================================
#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25
#define INPUT_BUFFER_SIZE 256
#define HISTORY_SIZE 100
#define MAX_ARGS 16

typedef struct {
    char     buffer[TERMINAL_HEIGHT][TERMINAL_WIDTH];
    int      cursor_x;
    int      cursor_y;
    uint8_t  fg_color;
    uint8_t  bg_color;
    char     input_buffer[INPUT_BUFFER_SIZE];
    int      input_pos;
    char     history[HISTORY_SIZE][INPUT_BUFFER_SIZE];
    int      history_count;
    int      history_index;
    char     current_dir[MAX_PATH];
    char     username[32];
    char     hostname[32];
    bool     is_root;
    bool     is_bash;  // true=bash, false=powershell
} terminal_t;

static terminal_t term;

// ============================================
// Terminal Colors
// ============================================
#define TERM_BLACK    0
#define TERM_RED      1
#define TERM_GREEN    2
#define TERM_YELLOW   3
#define TERM_BLUE     4
#define TERM_MAGENTA  5
#define TERM_CYAN     6
#define TERM_WHITE    7
#define TERM_BRIGHT_BLACK  8
#define TERM_BRIGHT_RED    9
#define TERM_BRIGHT_GREEN  10
#define TERM_BRIGHT_YELLOW 11
#define TERM_BRIGHT_BLUE   12
#define TERM_BRIGHT_MAGENTA 13
#define TERM_BRIGHT_CYAN   14
#define TERM_BRIGHT_WHITE  15

// ============================================
// Terminal Output
// ============================================
void term_putchar(char c) {
    if (c == '\n') {
        term.cursor_x = 0;
        term.cursor_y++;
        if (term.cursor_y >= TERMINAL_HEIGHT) {
            // Scroll up
            for (int i = 0; i < TERMINAL_HEIGHT - 1; i++) {
                strcpy(term.buffer[i], term.buffer[i + 1]);
            }
            term.cursor_y = TERMINAL_HEIGHT - 1;
            memset(term.buffer[term.cursor_y], 0, TERMINAL_WIDTH);
        }
        return;
    }
    
    if (c == '\r') {
        term.cursor_x = 0;
        return;
    }
    
    if (c == '\t') {
        term.cursor_x = (term.cursor_x + 8) & ~7;
        return;
    }
    
    if (c == '\b') {
        if (term.cursor_x > 0) {
            term.cursor_x--;
            term.buffer[term.cursor_y][term.cursor_x] = ' ';
        }
        return;
    }
    
    term.buffer[term.cursor_y][term.cursor_x] = c;
    term.cursor_x++;
    
    if (term.cursor_x >= TERMINAL_WIDTH) {
        term.cursor_x = 0;
        term.cursor_y++;
        if (term.cursor_y >= TERMINAL_HEIGHT) {
            for (int i = 0; i < TERMINAL_HEIGHT - 1; i++) {
                strcpy(term.buffer[i], term.buffer[i + 1]);
            }
            term.cursor_y = TERMINAL_HEIGHT - 1;
            memset(term.buffer[term.cursor_y], 0, TERMINAL_WIDTH);
        }
    }
}

void term_puts(const char* str) {
    while (*str) {
        term_putchar(*str++);
    }
}

void term_puts_colored(const char* str, uint8_t fg, uint8_t bg) {
    uint8_t old_fg = term.fg_color;
    uint8_t old_bg = term.bg_color;
    term.fg_color = fg;
    term.bg_color = bg;
    term_puts(str);
    term.fg_color = old_fg;
    term.bg_color = old_bg;
}

// ============================================
// Terminal Prompt
// ============================================
void term_print_prompt(void) {
    if (term.is_bash) {
        // Bash prompt: user@host:~$
        term_puts_colored(term.username, TERM_GREEN, TERM_BLACK);
        term_putchar('@');
        term_puts_colored(term.hostname, TERM_GREEN, TERM_BLACK);
        term_putchar(':');
        term_puts_colored(term.current_dir, TERM_BLUE, TERM_BLACK);
        term_puts("$ ");
    } else {
        // PowerShell prompt: PS C:\Users\user>
        term_puts_colored("PS ", TERM_YELLOW, TERM_BLACK);
        term_puts_colored(term.current_dir, TERM_BLUE, TERM_BLACK);
        term_puts("> ");
    }
}

// ============================================
// Command Processing
// ============================================
void term_process_command(const char* cmd) {
    // Add to history
    if (term.history_count < HISTORY_SIZE) {
        strcpy(term.history[term.history_count], cmd);
        term.history_count++;
    }
    
    // Parse command and arguments
    char* args[MAX_ARGS];
    int argc = 0;
    char cmd_copy[INPUT_BUFFER_SIZE];
    strcpy(cmd_copy, cmd);
    
    char* token = strtok(cmd_copy, " ");
    while (token && argc < MAX_ARGS) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (argc == 0) return;
    
    // Built-in commands
    if (strcmp(args[0], "help") == 0) {
        term_puts("WeeK OS Terminal Komutlari:\n");
        term_puts("  help     - Bu yardimi gosterir\n");
        term_puts("  clear    - Ekrani temizler\n");
        term_puts("  ls       - Dosyalari listeler\n");
        term_puts("  cd       - Dizin degistirir\n");
        term_puts("  pwd      - Mevcut dizini gosterir\n");
        term_puts("  cat      - Dosya icerigini gosterir\n");
        term_puts("  mkdir    - Dizin olusturur\n");
        term_puts("  rm       - Dosya siler\n");
        term_puts("  cp       - Dosya kopyalar\n");
        term_puts("  mv       - Dosya tasiir\n");
        term_puts("  echo     - Metin yazdirir\n");
        term_puts("  whoami   - Kullanici adini gosterir\n");
        term_puts("  date     - Tarih ve saati gosterir\n");
        term_puts("  uptime   - Calisma suresini gosterir\n");
        term_puts("  ps       - Surecleri gosterir\n");
        term_puts("  top      - Surecler yonetici\n");
        term_puts("  df       - Disk bilgisi\n");
        term_puts("  free     - Bellek bilgisi\n");
        term_puts("  uname    - Sistem bilgisi\n");
        term_puts("  neofetch - Sistem bilgisi (gorsel)\n");
        term_puts("  exit     - Terminali kapatir\n");
    }
    else if (strcmp(args[0], "clear") == 0 || strcmp(args[0], "cls") == 0) {
        memset(term.buffer, 0, sizeof(term.buffer));
        term.cursor_x = 0;
        term.cursor_y = 0;
    }
    else if (strcmp(args[0], "pwd") == 0) {
        term_puts(term.current_dir);
        term_putchar('\n');
    }
    else if (strcmp(args[0], "whoami") == 0) {
        term_puts(term.username);
        term_putchar('\n');
    }
    else if (strcmp(args[0], "uname") == 0) {
        if (argc > 1 && strcmp(args[1], "-a") == 0) {
            term_puts("WeeK OS 1.0.0 WeeK-KERNEL x86_64 GNU/Linux\n");
        } else {
            term_puts("WeeK OS\n");
        }
    }
    else if (strcmp(args[0], "date") == 0) {
        term_puts("2026-08-28 15:43:00 UTC\n");
    }
    else if (strcmp(args[0], "uptime") == 0) {
        term_puts(" 15:43:00 up 0 days, 0:00, 1 user\n");
    }
    else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; i < argc; i++) {
            term_puts(args[i]);
            if (i < argc - 1) term_putchar(' ');
        }
        term_putchar('\n');
    }
    else if (strcmp(args[0], "neofetch") == 0) {
        term_puts_colored("        .--.        \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("       |o_o |       \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("       |:_/ |       \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("      //   \\ \\      \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("     (|     | )     \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("    /'\\_   _/`\\     \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("    \\___)=(___/     \n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  WeeK OS", TERM_CYAN, TERM_BLACK);
        term_puts_colored(" @ ", TERM_WHITE, TERM_BLACK);
        term_puts_colored(term.hostname, TERM_CYAN, TERM_BLACK);
        term_puts_colored("\n  ----------------\n", TERM_WHITE, TERM_BLACK);
        term_puts_colored("  OS: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("WeeK OS 1.0.0\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  Kernel: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("WeeK-KERNEL 1.0.0\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  Shell: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored(term.is_bash ? "week-bash" : "week-shell", TERM_GREEN, TERM_BLACK);
        term_puts_colored("\n  DE: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("Week Desktop\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  WM: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("Week Compositor\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  Theme: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("Blue/White [Light]\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  Terminal: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("Week Terminal\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  CPU: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("x86_64 @ 3.2GHz\n", TERM_GREEN, TERM_BLACK);
        term_puts_colored("  Memory: ", TERM_WHITE, TERM_BLACK);
        term_puts_colored("2048MiB / 8192MiB\n", TERM_GREEN, TERM_BLACK);
        term_putchar('\n');
        // Color blocks
        for (int i = 0; i < 8; i++) {
            term_puts_colored("   ", i, i);
        }
        term_putchar('\n');
        for (int i = 8; i < 16; i++) {
            term_puts_colored("   ", i, i);
        }
        term_putchar('\n');
    }
    else if (strcmp(args[0], "exit") == 0) {
        term_puts("Kapatiliyor...\n");
        // Close terminal window
    }
    else {
        term_puts_colored("week: komut bulunamadi: ", TERM_RED, TERM_BLACK);
        term_puts(args[0]);
        term_putchar('\n');
    }
}

// ============================================
// Terminal Input Handling
// ============================================
void term_handle_input(keyboard_event_t* event) {
    if (!event->pressed) return;
    
    // Enter - execute command
    if (event->key == 0x1C) {  // Enter
        term_putchar('\n');
        if (term.input_pos > 0) {
            term_process_command(term.input_buffer);
        }
        term.input_pos = 0;
        memset(term.input_buffer, 0, INPUT_BUFFER_SIZE);
        term_print_prompt();
        return;
    }
    
    // Backspace
    if (event->key == 0x0E) {  // Backspace
        if (term.input_pos > 0) {
            term.input_pos--;
            term.input_buffer[term.input_pos] = '\0';
            term_putchar('\b');
        }
        return;
    }
    
    // Tab completion
    if (event->key == 0x0F) {  // Tab
        // Simple tab completion
        return;
    }
    
    // Arrow keys (history)
    if (event->key == 0x48) {  // Up arrow
        if (term.history_index > 0) {
            term.history_index--;
            strcpy(term.input_buffer, term.history[term.history_index]);
            term.input_pos = strlen(term.input_buffer);
        }
        return;
    }
    if (event->key == 0x50) {  // Down arrow
        if (term.history_index < term.history_count - 1) {
            term.history_index++;
            strcpy(term.input_buffer, term.history[term.history_index]);
            term.input_pos = strlen(term.input_buffer);
        }
        return;
    }
    
    // Regular character
    if (event->key < 128) {
        char c = event->key;
        if (event->shift) {
            // Shift mapping
            if (c >= 'a' && c <= 'z') c -= 32;
            else if (c == '1') c = '!';
            else if (c == '2') c = '@';
            else if (c == '3') c = '#';
            else if (c == '4') c = '$';
            else if (c == '5') c = '%';
            else if (c == '6') c = '^';
            else if (c == '7') c = '&';
            else if (c == '8') c = '*';
            else if (c == '9') c = '(';
            else if (c == '0') c = ')';
        }
        
        if (term.input_pos < INPUT_BUFFER_SIZE - 1) {
            term.input_buffer[term.input_pos++] = c;
            term.input_buffer[term.input_pos] = '\0';
            term_putchar(c);
        }
    }
}

// ============================================
// Terminal Initialization
// ============================================
void term_init(bool bash_mode) {
    memset(&term, 0, sizeof(terminal_t));
    
    term.fg_color = TERM_WHITE;
    term.bg_color = TERM_BLACK;
    term.is_bash = bash_mode;
    term.is_root = false;
    
    strcpy(term.current_dir, "/home/user");
    strcpy(term.username, "user");
    strcpy(term.hostname, "WeeK-PC");
    
    // Clear screen
    memset(term.buffer, 0, sizeof(term.buffer));
    
    // Print welcome message
    if (bash_mode) {
        term_puts_colored("WeeK Terminal (Bash)\n", TERM_CYAN, TERM_BLACK);
        term_puts_colored("WeeK OS 1.0.0 - Liquid Glass Desktop\n", TERM_WHITE, TERM_BLACK);
        term_puts_colored("Yardim icin 'help' yazin.\n\n", TERM_YELLOW, TERM_BLACK);
    } else {
        term_puts_colored("WeeK Shell (PowerShell-like)\n", TERM_BLUE, TERM_BLACK);
        term_puts_colored("WeeK OS 1.0.0 - Liquid Glass Desktop\n", TERM_WHITE, TERM_BLACK);
        term_puts_colored("Yardim icin 'help' yazin.\n\n", TERM_YELLOW, TERM_BLACK);
    }
    
    term_print_prompt();
}

// ============================================
// Terminal Draw
// ============================================
void term_draw(framebuffer_t* fb) {
    int char_width = 8;
    int char_height = 16;
    int margin_x = 10;
    int margin_y = 10;
    
    // Terminal background
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0C0C0C);
    
    // Draw buffer
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            char c = term.buffer[y][x];
            if (c) {
                fb_fill_rect(fb, margin_x + x * char_width, margin_y + y * char_height,
                            char_width, char_height, 0x0C0C0C);
                // Character placeholder
                fb_fill_rect(fb, margin_x + x * char_width + 1, margin_y + y * char_height + 1,
                            char_width - 2, char_height - 2, term.fg_color == TERM_WHITE ? 0xCCCCCC : 0xFFFFFF);
            }
        }
    }
    
    // Cursor
    fb_fill_rect(fb, margin_x + term.cursor_x * char_width, 
                margin_y + term.cursor_y * char_height,
                char_width, char_height, 0xFFFFFF);
}
