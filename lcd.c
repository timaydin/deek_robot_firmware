/***********************************************************************
 ***********************************************************************/
#include "numicro_8051.h"
#include "debug_log.h"
#include "util.h"

#define HT1621B_DATA P00
#define HT1621B_CS   P01
#define HT1621B_WR   P10

#define LCD_BUILD_CMD(id, w1, w2, s) \
    ((0b##id << 9) | (0b##w1 << 5) | (0b##w2 << 1) | 0b##s)

#define SYS_EN           LCD_BUILD_CMD(100, 0000, 0001, 0)
#define BIAS_12_2COMMONS LCD_BUILD_CMD(100, 0010, 0000, 0)
#define BIAS_12_3COMMONS LCD_BUILD_CMD(100, 0010, 0100, 0)
#define BIAS_12_4COMMONS LCD_BUILD_CMD(100, 0010, 1000, 0)
#define BIAS_13_2COMMONS LCD_BUILD_CMD(100, 0010, 0001, 0)
#define BIAS_13_3COMMONS LCD_BUILD_CMD(100, 0010, 0101, 0)
#define BIAS_13_4COMMONS LCD_BUILD_CMD(100, 0010, 1001, 0)

/***********************************************************************
 ***********************************************************************/
static void ht1621b_command(unsigned int data)
{
    debug_log(1, "data = 0x%04X\n", data);

    HT1621B_CS = 0;

    unsigned int bitcount = 12;
    unsigned int mask = 1 << bitcount;
    for (unsigned int i = 0; i < bitcount; ++i)
    {
        mask >>= 1;
        HT1621B_DATA = !!(data & mask);

        HT1621B_WR = 1;
        busy_loop_delay(5);
        HT1621B_WR = 0;
    }

    HT1621B_CS = 1;
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
    P00_PUSHPULL_MODE;
    P01_PUSHPULL_MODE;
    P10_PUSHPULL_MODE;

    ht1621b_command(SYS_EN);
    ht1621b_command(BIAS_12_4COMMONS);

    while (1)
    {
        ;
    }
}
