#include <stdint.h>

#include "global.h"
//#include "mailbox/mailbox.h"
#include "periph/periph.h"
//#include "fb/fb.h"

int kernel_main()
{
    uart_init();
    timer_init();

    //fb_init();
    //uint32_t fb_addr = GET32(0x40040020);
    //puts(0, 0, "Hello, world!", fb_addr);
    //puts(0, 1, "well...", fb_addr);

    uart_printf("Hello, kernel World!\r\n");
    uart_printf("str=%s\n\rchr=%c\n\rnum1=%d\n\rnum2=%d\n\rhex1=0x%x\n\rhex2=0x%x\n\r", 
        "hoi i'm temmie", 
        '>', 
        2147483648,
        2147483647,
        0xa0b1c2d4,
        0xa0b1c2d3);

    return 0;
}
