// WeeK OS - Sticky Notes
// Desktop sticky notes with multiple colors

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Sticky Note
// ============================================
#define MAX_NOTES 32
#define NOTE_MAX_TEXT 1024

typedef struct {
    uint32_t id;
    int32_t  x, y;
    uint32_t w, h;
    char     text[NOTE_MAX_TEXT];
    uint32_t color;
    bool     visible;
    bool     minimized;
    bool     focused;
    uint32_t font_size;
} sticky_note_t;

static sticky_note_t notes[MAX_NOTES];
static int note_count = 0;
static uint32_t next_note_id = 1;

// Note colors
#define NOTE_YELLOW  0xFFF9C4
#define NOTE_PINK    0xF8BBD0
#define NOTE_GREEN   0xC8E6C9
#define NOTE_BLUE    0xBBDEFB
#define NOTE_PURPLE  0xD1C4E9
#define NOTE_ORANGE  0xFFE0B2

static const uint32_t note_colors[] = {
    NOTE_YELLOW, NOTE_PINK, NOTE_GREEN, 
    NOTE_BLUE, NOTE_PURPLE, NOTE_ORANGE
};

// ============================================
// Create Note
// ============================================
uint32_t sticky_create_note(int32_t x, int32_t y, const char* text, uint32_t color) {
    if (note_count >= MAX_NOTES) return 0;
    
    sticky_note_t* note = &notes[note_count];
    note->id = next_note_id++;
    note->x = x;
    note->y = y;
    note->w = 200;
    note->h = 200;
    strcpy(note->text, text);
    note->color = color;
    note->visible = true;
    note->minimized = false;
    note->focused = false;
    note->font_size = 14;
    
    note_count++;
    return note->id;
}

// ============================================
// Delete Note
// ============================================
void sticky_delete_note(uint32_t id) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            // Shift array
            for (int j = i; j < note_count - 1; j++) {
                notes[j] = notes[j + 1];
            }
            note_count--;
            break;
        }
    }
}

// ============================================
// Draw Note
// ============================================
void sticky_draw_note(framebuffer_t* fb, uint32_t id) {
    sticky_note_t* note = NULL;
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            note = &notes[i];
            break;
        }
    }
    
    if (!note || !note->visible || note->minimized) return;
    
    // Note shadow
    fb_fill_rect(fb, note->x + 3, note->y + 3, note->w, note->h, 0x000000);
    
    // Note body
    fb_fill_rect(fb, note->x, note->y, note->w, note->h, note->color);
    
    // Note header (darker shade)
    uint32_t header_color = note->color - 0x101010;
    fb_fill_rect(fb, note->x, note->y, note->w, 24, header_color);
    
    // Close button
    fb_fill_rect(fb, note->x + note->w - 20, note->y + 4, 16, 16, 0xFF0000);
    
    // Minimize button
    fb_fill_rect(fb, note->x + note->w - 40, note->y + 4, 16, 16, 0xFFC107);
    
    // Note content area
    fb_fill_rect(fb, note->x + 8, note->y + 32, note->w - 16, note->h - 40, 
                 note->color - 0x080808);
    
    // Text (placeholder - character rendering)
    int text_y = note->y + 40;
    for (int i = 0; note->text[i] && text_y < note->y + note->h - 10; i++) {
        if (note->text[i] == '\n') {
            text_y += note->font_size + 4;
        } else {
            // Character placeholder
            fb_fill_rect(fb, note->x + 12 + (i % 20) * 8, text_y, 6, note->font_size, 0x1A1A1A);
        }
    }
}

// ============================================
// Draw All Notes
// ============================================
void sticky_draw_all(framebuffer_t* fb) {
    for (int i = 0; i < note_count; i++) {
        sticky_draw_note(fb, notes[i].id);
    }
}

// ============================================
// Note Actions
// ============================================
void sticky_move_note(uint32_t id, int32_t x, int32_t y) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            notes[i].x = x;
            notes[i].y = y;
            break;
        }
    }
}

void sticky_resize_note(uint32_t id, uint32_t w, uint32_t h) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            notes[i].w = w;
            notes[i].h = h;
            break;
        }
    }
}

void sticky_set_text(uint32_t id, const char* text) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            strcpy(notes[i].text, text);
            break;
        }
    }
}

void sticky_set_color(uint32_t id, uint32_t color) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            notes[i].color = color;
            break;
        }
    }
}

void sticky_minimize_note(uint32_t id) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i].id == id) {
            notes[i].minimized = !notes[i].minimized;
            break;
        }
    }
}

// ============================================
// Initialize Default Notes
// ============================================
void sticky_init(void) {
    // Create some default notes
    sticky_create_note(100, 100, "Hosgeldin WeeK OS!\nBu bir yapiskan not.", NOTE_YELLOW);
    sticky_create_note(350, 150, "Yapiskan notlar masaustune\nyerlestirilebilir.", NOTE_BLUE);
    sticky_create_note(600, 100, "Farkli renklerde notlar\nolusturabilirsiniz.", NOTE_GREEN);
}

sticky_note_t* sticky_get_notes(void) {
    return notes;
}

int sticky_get_note_count(void) {
    return note_count;
}
