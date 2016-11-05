#include "mailbox.h"

uint32_t MailboxWrite(uint32_t fbinfo_addr, uint32_t channel)
{
    uint32_t mailbox;

    mailbox = 0x2000B880;
    while (1)
    {
        if ((GET32(mailbox + 0x18) & 0x80000000) == 0)
            break;
    }
    PUT32(mailbox + 0x20, fbinfo_addr + channel);
    return 0;
}

uint32_t MailboxRead(uint32_t channel)
{
    uint32_t ra;
    uint32_t mailbox;

    mailbox = 0x2000B880;
    while (1)
    {
        while (1)
        {
            ra = GET32(mailbox + 0x18);
            if ((ra & 0x40000000) == 0)
                break;
        }
        hexstrings(ra);
        ra = GET32(mailbox + 0x00);
        hexstring(ra);
        if ((ra & 0xF) == channel)
            break;
    }
    return ra;
}
