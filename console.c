/***********************************************************************
 ***********************************************************************/
#include "numicro_8051.h"

/***********************************************************************
 ***********************************************************************/
void console_write(const char* buffer, unsigned int len)
{
    for (unsigned int i = 0; i < len; ++i)
    {
        while (TI == 0)
        {
            ;
        }
        TI = 0;

        SBUF = buffer[i];
    }

}

/***********************************************************************
 ***********************************************************************/
void console_init(unsigned long baudrate)
{
    /* UART0 TX */
    P06_PUSHPULL_MODE;

    /* UART0 RX */
    P07_INPUT_MODE;

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
