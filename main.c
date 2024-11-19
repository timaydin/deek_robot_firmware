#include "numicro_8051.h"

static void gpio_init(void)
{
    /* LED */
    P15_PUSHPULL_MODE;

    /* UART */
    P06_PUSHPULL_MODE;
    P07_INPUT_MODE;
}

void console_init(unsigned long baudrate)
{
    /* UART0 mode 1, don't check stop bit, enable receive */
    SCON = 0x50;

    /* timer 1, mode 2 */
    TMOD = 0x20;

    /* UART0 double baud rate */
    set_PCON_SMOD;

    /* timer 1 is driven directly by the system clock */
    set_CKCON_T1M;

    /* UART0 clock source is timer 1 */
    clr_T3CON_BRCK;

    /* configure baud rate */
    TH1 = 256 - 1000000 / baudrate;

    /* enable timer 1 */
    set_TCON_TR1;

    /* initiate transmit operations */
    set_SCON_TI;
}

void main(void)
{
    gpio_init();

    console_init(10000);

    unsigned int count = 0;
    while (1)
    {
        if (RI)
        {
            char c = SBUF;
            RI = 0;

            printf("received: 0x%02X\r\n", c);
        }
    }
}
