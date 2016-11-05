#include <stdint.h>
#include "../global.h"

extern void uart_init();
extern uint32_t uart_lcr();
extern void uart_flush();
extern void uart_send(uint32_t);
extern uint32_t uart_recv();
extern uint32_t uart_check();
extern void hexstring(uint32_t);
extern void hexstrings(uint32_t);
extern void timer_init();
extern uint32_t timer_tick();
