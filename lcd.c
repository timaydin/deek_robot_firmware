/***********************************************************************
 ***********************************************************************/
#include "numicro_8051.h"
#include "debug_log.h"
#include "util.h"

#define HT1621B_DATA P00
#define HT1621B_CS   P01
#define HT1621B_WR   P10


/***********************************************************************
 ***********************************************************************/
static void ht1621b_write_data(unsigned int data, unsigned int bitcount)
{
    unsigned int mask = 1 << bitcount;

    for (unsigned int i = 0; i < bitcount; ++i)
    {
        mask >>= 1;
        HT1621B_DATA = !!(data & mask);

        HT1621B_WR = 1;
        busy_loop_delay(5);
        HT1621B_WR = 0;
    }
}

/***********************************************************************
 ***********************************************************************/
void lcd_set_voltage(float voltage)
{
    /* TODO: implement */
    if (voltage > 0)
    {
        ;
    }
}

/***********************************************************************
 ***********************************************************************/
void lcd_set_current(float current)
{
    /* TODO: implement */
    if (current > 0)
    {
        ;
    }
}

/***********************************************************************
 ***********************************************************************/
void lcd_init(void)
{
    debug_log(1, "enter\n");

    P00_PUSHPULL_MODE;
    P01_PUSHPULL_MODE;
    P10_PUSHPULL_MODE;

    while (1)
    {
        HT1621B_CS = 0;
        unsigned int lcd_on = 0b100000000110;
        ht1621b_write_data(lcd_on, 12);
        HT1621B_CS = 1;

        busy_loop_delay(1000);
    }
}
