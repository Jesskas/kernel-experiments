#include <stdint.h>

#include "global.h"
#include "mailbox/mailbox.h"
#include "periph/periph.h"
#include "fb/fb.h"

int kernel_main()
{
    uart_init();
    timer_init();
    fb_init();

    uint32_t fb_addr = GET32(0x40040020);

    puts(0, 0, "Hello, world!", fb_addr);
    puts(0, 1, "well...", fb_addr);
    return 0;
}
