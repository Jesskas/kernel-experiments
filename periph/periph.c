#include "periph.h"

// Note, 
// https://www.raspberrypi.org/documentation/hardware/
//     raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf
// is a nice resource and all, but contains typos; errata in
// https://elinux.org/BCM2835_datasheet_errata#BCM_2835_datasheet_errata

// ARM Timer Registers
// Base address in BCM2835: 0x7E00B000
#define ARM_TIMER_LOAD  0x2000B400 // Load
#define ARM_TIMER_VALUE 0x2000B404 // Value             (Read Only)
#define ARM_TIMER_CTL   0x2000B408 // Control
#define ARM_TIMER_ICA   0x2000B40C // IRQ Clear/Ack     (Write Only)
#define ARM_TIMER_IRAW  0x2000B410 // RAW IRQ           (Read Only) 
#define ARM_TIMER_IMASK 0x2000B414 // Masked IRQ        (Read Only)
#define ARM_TIMER_RLOAD 0x2000B418 // Reload
#define ARM_TIMER_PRDV  0x2000B41C // Pre-divider (Not in real 804!)
#define ARM_TIMER_CNT   0x2000B420 // Free running counter (Not in real 804!)

// GPIO Registers
// Base address in BCM2835: 0x7E200000
// Todo: Complete list
#define GPFSEL1         0x20200004 // GPIO Function Select 1
#define GPSET0          0x2020001C // GPIO Pin Output Set 0
#define GPCLR0          0x20200028 // GPIO Pin Output Clear 0
#define GPPUD           0x20200094 // GPIO Pin Pull-up/down Enable
#define GPPUDCLK0       0x20200098 // GPIO Pin Pull-up/down Enable Clock 0

// Auxiliary Peripheral Registers
// Base address in BCM2835: 0x7E215000
// Todo: Complete list, missing two SPI masters
#define AUX_IRQ         0x20215000 // Auxiliary Interrupt status
#define AUX_ENABLES     0x20215004 // Auxiliary enables
#define AUX_MU_IO_REG   0x20215040 // Mini UART I/O Data
#define AUX_MU_IER_REG  0x20215044 // Mini UART Interrupt Enable
#define AUX_MU_IIR_REG  0x20215048 // Mini UART Interrupt Identify
#define AUX_MU_LCR_REG  0x2021504C // Mini UART Line Control
#define AUX_MU_MCR_REG  0x20215050 // Mini UART Modem Control
#define AUX_MU_LSR_REG  0x20215054 // Mini UART Line Status
#define AUX_MU_MSR_REG  0x20215058 // Mini UART Modem Status
#define AUX_MU_SCRATCH  0x2021505C // Mini UART Scratch
#define AUX_MU_CNTL_REG 0x20215060 // Mini UART Extra Control
#define AUX_MU_STAT_REG 0x20215064 // Mini UART Extra Status
#define AUX_MU_BAUD_REG 0x20215068 // Mini UART Baudrate
// Todo: The PL011 UART; above is the "Mini UART"
// PL011 UART is the one with registers like "UART_DR"

// Memory-Mapped I/O output
static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

// Memory-Mapped I/O input
static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");
}

void uart_init()
{
    // Disable [mini] UART RX and TX
    mmio_write(AUX_MU_CNTL_REG, 0);

    // Setup the GPIO pin 14 && 15 and alt5.
    unsigned int ra = mmio_read(GPFSEL1);
    ra &= ~(7 << 12);     // gpio14
    ra |= 2 << 12;        // alt5
    ra &= ~(7 << 15);     // gpio15
    ra |= 2 << 15;        // alt5
    mmio_write(GPFSEL1, ra);
    
    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    mmio_write(GPPUD, 0x00000000);
    // delay(150);
    for (ra = 0; ra < 150; ra++) dummy(ra);   // delay for 150 cycles
 
    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    // delay(150);
    for (ra = 0; ra < 150; ra++) dummy(ra);   // delay for 150 cycles

    // Write 0 to GPPUDCLK0 to make it take effect.
    mmio_write(GPPUDCLK0, 0x00000000);


    // Enable the mini UART
    mmio_write(AUX_ENABLES, 1);

    // Disable interrupts
    mmio_write(AUX_MU_IER_REG, 0);
    
    // Properly (see errata) set UART to 8-bit mode in LCR register
    mmio_write(AUX_MU_LCR_REG, 3); 

    // Clear RTS bit to set RTS line to high
    mmio_write(AUX_MU_MCR_REG,0);

    // Disable interrupts (again) 
    mmio_write(AUX_MU_IER_REG, 0);

    // Enable and clear receive+transmit interrupt FIFOs
    mmio_write(AUX_MU_IIR_REG, 0xC6);

    // for AUX_UART_CLOCK=250000000, BAUD=115200,
    // floor((AUX_UART_CLOCK/(BAUD*8))-1) = 270
    mmio_write(AUX_MU_BAUD_REG, 270);

    // Enable [mini] UART RX and TX
    mmio_write(AUX_MU_CNTL_REG, 3);
}

void uart_putc(unsigned char c)
{
    // Wait for UART to become ready to transmit
    while (!(mmio_read(AUX_MU_LSR_REG) & (1 << 5))) {}
    mmio_write(AUX_MU_IO_REG, c);
}

// Uncertain how well this works...
unsigned char uart_getc()
{
    // Wait for UART to have received something.
    while (!(mmio_read(AUX_MU_LSR_REG) & (1 << 4))) {}
    return mmio_read(AUX_MU_IO_REG);
}

void uart_puts(const char* str)
{
    for (uint32_t i = 0; str[i] != '\0'; i++)
        uart_putc((unsigned char)str[i]);
}

void uart_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    //while (*fmt != '\0')
    //{
    //    if (*fmt == 'd') {
    //        int i = va_arg(args, int);
    //        uart_puts("##some_number##");
    //    } else if (*fmt == 'c') {
    //        // note automatic conversion to integral type?
    //        // int c = va_arg(args, int);
    //        // printf("%c\n", c);
    //    } else {
    //        // not implemented x:
    //    }
    //    ++fmt;
    //}
    va_end(args);

    uart_puts(fmt);
}


//--------------------------------------
// Above is from OSDev pages
// Below is old material and copyrighted
//--------------------------------------


// GPIO14  TXD0 and TXD1
// GPIO15  RXD0 and RXD1
uint32_t uart_lcr()
{
    return GET32(AUX_MU_LSR_REG);
}

// uint32_t uart_recv()
// {
//     while (1)
//     {
//         if (GET32(AUX_MU_LSR_REG)&0x01)
//             break;
//     }
//     return GET32(AUX_MU_IO_REG) & 0xFF;
// }

uint32_t uart_check()
{
    if (GET32(AUX_MU_LSR_REG) & 0x01) return 1;
    return 0;
}

// Deprecated
void uart_send(uint32_t c)
{
    while (1)
    {
        if (GET32(AUX_MU_LSR_REG) & 0x20)
            break;
    }
    PUT32(AUX_MU_IO_REG, c);
}

void uart_flush()
{
    while (1)
    {
        if ((GET32(AUX_MU_LSR_REG) & 0x100) == 0)
            break;
    }
}

void hexstrings(uint32_t d)
{
    uint32_t rb;
    uint32_t rc;

    rb = 32; // 0x20
    while (1)
    {
        rb -= 4;
        rc = (d >> rb)&0xF;  // 0b1111
        if (rc > 9)
            rc += 0x37;
        else
            rc += 0x30;
        uart_send(rc);
        if (rb == 0)
            break; // basically, repeat 8 times
    }
    uart_send(0x20);
}

void hexstring(uint32_t d)
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}

void timer_init()
{
    //0xF9+1 = 250
    //250MHz/250 = 1MHz
    PUT32(ARM_TIMER_CTL, 0x00F90000);
    PUT32(ARM_TIMER_CTL, 0x00F90200);
}

uint32_t timer_tick()
{
    return GET32(ARM_TIMER_CNT);
}

//-------------------------------------------------------------------------
// Copyright (c) 2012 David Welch dwelch@dwelch.com
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//-------------------------------------------------------------------------
