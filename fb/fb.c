#include "fb.h"

void fb_init()
{
    PUT32(0x40040000, 640); /* #0 Physical Width */
    PUT32(0x40040004, 480); /* #4 Physical Height */
    PUT32(0x40040008, 640); /* #8 Virtual Width */
    PUT32(0x4004000C, 480); /* #12 Virtual Height */
    PUT32(0x40040010, 0);   /* #16 GPU - Pitch */
    PUT32(0x40040014, 16);   /* #20 Bit Depth */ /* changed from 32 */
    PUT32(0x40040018, 0);   /* #24 X */
    PUT32(0x4004001C, 0);   /* #28 Y */
    PUT32(0x40040020, 0);   /* #32 GPU - Pointer */
    PUT32(0x40040024, 0);   /* #36 GPU - Size */

    MailboxWrite(0x40040000,1);
    MailboxRead(1);

    // fb_info.virt_height = 480;
    // fb_info.virt_width = 640;
    // fb_info.bit_depth = 16;
    // fb_info.fb_addr = GET32(0x40040020);
}

// Also, observe @fb_base. This is a pointer, but PUT32() takes a non-pointer.
void drawChar(uint32_t drawAt_x, uint32_t drawAt_y, char c, uint32_t fb_base)
{
    const uint8_t charWidth = 8;
    const uint8_t charHeight = 16;

    // const uint8_t bpp = fb_info.bit_depth / 8;

    uint32_t x, y;
    drawAt_x *= charWidth;
    drawAt_y *= charHeight;
    for (y = 0; y < charHeight; y++) {
        for (x = 0; x < charWidth; x++) {
            uint32_t put_addr = fb_base +
                                ((drawAt_x + x) * 2) +
                                (((drawAt_y + y) * 2) * 640);

            PUT16(put_addr, default_font[(uint8_t)c][x+y*8]);
        }
    }
}

void puts(uint32_t xIndex, uint32_t yIndex, const char* s, uint32_t fb_base)
{
    // TODO: Do not increment yIndex. When do we increment it, though?
    // Use some algorithm related to screen size.
    while (*s != '\0') {
        drawChar(xIndex++, yIndex,
                 *s++, fb_base);
    }
}
