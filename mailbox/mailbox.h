#include <stdint.h>
#include "../global.h"
#include "../periph/periph.h"

uint32_t MailboxWrite (uint32_t fbinfo_addr, uint32_t channel);
uint32_t MailboxRead (uint32_t channel);
