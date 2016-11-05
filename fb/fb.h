#include <stdint.h>
#include "../mailbox/mailbox.h"
#include "../global.h"
//#include "font8x8_basic.h"
#include "char_data.h"

struct fb_info_t {
    uint32_t phys_width;
    uint32_t phys_height;
    uint32_t virt_width;
    uint32_t virt_height;
    uint32_t pitch;
    uint32_t bit_depth;
    uint32_t x;
    uint32_t y;
    uint32_t fb_addr;
    uint32_t fb_size;
};

// Only one framebuffer for now
// static struct fb_info_t fb_info = { 0 };

void fb_init();

void drawChar(uint32_t drawAt_x, uint32_t drawAt_y, char c, uint32_t fb_base);
void puts(uint32_t xIndex, uint32_t yIndex, const char* c, uint32_t fb_base);
