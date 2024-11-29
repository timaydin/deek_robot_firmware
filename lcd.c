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

#define SYS_DIS          LCD_BUILD_CMD(100, 0000, 0000, 0)
#define SYS_EN           LCD_BUILD_CMD(100, 0000, 0001, 0)
#define LCD_OFF          LCD_BUILD_CMD(100, 0000, 0010, 0)
#define LCD_ON           LCD_BUILD_CMD(100, 0000, 0011, 0)
#define BIAS_12_2COMMONS LCD_BUILD_CMD(100, 0010, 0000, 0)
#define BIAS_12_3COMMONS LCD_BUILD_CMD(100, 0010, 0100, 0)
#define BIAS_12_4COMMONS LCD_BUILD_CMD(100, 0010, 1000, 0)
#define BIAS_13_2COMMONS LCD_BUILD_CMD(100, 0010, 0001, 0)
#define BIAS_13_3COMMONS LCD_BUILD_CMD(100, 0010, 0101, 0)
#define BIAS_13_4COMMONS LCD_BUILD_CMD(100, 0010, 1001, 0)

static unsigned char disp_buffer[19];

static const unsigned char segments[] =
{
    0x21,
    0x11,
    0x14,
    0x28,
    0x24,
    0x22,
    0x12,
};

static const unsigned char digits[][7] =
{
    {1, 1, 1, 1, 1, 1, 0}, /* 0 */
    {0, 1, 1, 0, 0, 0, 0}, /* 1 */
    {1, 1, 0, 1, 1, 0, 1}, /* 2 */
    {1, 1, 1, 1, 0, 0, 1}, /* 3 */
    {0, 1, 1, 0, 0, 1, 1}, /* 4 */
    {1, 0, 1, 1, 0, 1, 1}, /* 5 */
    {1, 0, 1, 1, 1, 1, 1}, /* 6 */
    {1, 1, 1, 0, 0, 0, 0}, /* 7 */
    {1, 1, 1, 1, 1, 1, 1}, /* 8 */
    {1, 1, 1, 1, 0, 1, 1}, /* 9 */
};

/***********************************************************************
 ***********************************************************************/
static void _2us_delay(void)
{
    __asm__(
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
    );
}

/***********************************************************************
 ***********************************************************************/
static void ht1621b_write_bits(unsigned int data, unsigned int bitcount)
{
    unsigned int mask = 1 << bitcount;
    for (unsigned int i = 0; i < bitcount; ++i)
    {
        mask >>= 1;
        HT1621B_DATA = !!(data & mask);

        _2us_delay();

        HT1621B_WR = 0;

        _2us_delay();

        HT1621B_WR = 1;
    }
}

/***********************************************************************
 ***********************************************************************/
static void ht1621b_command(unsigned int data, unsigned int bitcount)
{
    HT1621B_CS = 0;

    ht1621b_write_bits(data, bitcount);

    HT1621B_CS = 1;
}

/***********************************************************************
 ***********************************************************************/
static void ht1621b_write_disp_buffer(const unsigned char* buffer)
{
    HT1621B_CS = 0;

    unsigned int cmd = 0b101 << 6;
    ht1621b_write_bits(cmd, 9);

    for (unsigned int i = 0; i < sizeof(disp_buffer); ++i)
    {
        ht1621b_write_bits(buffer[i], 4);
    }

    HT1621B_CS = 1;
}

/***********************************************************************
 ***********************************************************************/
static void update_disp_buffer(unsigned char digit,
                               unsigned char segment,
                               unsigned char status)
{
    unsigned char reladdr = (segment >> 4) & 0x03;
    unsigned char mask = segment & 0x0f;
    unsigned char offset = 2 * digit + reladdr;

    if (status)
    {
        disp_buffer[offset] |= mask;
    }
    else
    {
        disp_buffer[offset] &= ~mask;
    }
}

/***********************************************************************
 ***********************************************************************/
static void lcd_set_digit(unsigned char digit,
                          unsigned char value)
{
    for (unsigned char i = 0; i < sizeof(segments); ++i)
    {
        update_disp_buffer(digit, segments[i], digits[value][i]);
    }
}

/***********************************************************************
 ***********************************************************************/
static void lcd_set_sym(unsigned char digit,
                        unsigned char sym)
{
    update_disp_buffer(digit, 0x18, sym);
}

/***********************************************************************
 ***********************************************************************/
static void lcd_set_celsius(unsigned char celsius)
{
    update_disp_buffer(0, 0x01, celsius);
}

/***********************************************************************
 ***********************************************************************/
static void lcd_display_value(unsigned char line, float fvalue)
{
    unsigned char digit = line * 3;

    unsigned long lvalue;
    if (fvalue < 10)
    {
        lvalue = fvalue * 100 + 0.5;
        lcd_set_sym(digit + 1, 0);
        lcd_set_sym(digit + 2, 1);
    }
    else if (fvalue < 100)
    {
        lvalue = fvalue * 10 + 0.5;
        lcd_set_sym(digit + 1, 1);
        lcd_set_sym(digit + 2, 0);
    }
    else
    {
        lvalue = fvalue + 0.5;
        lcd_set_sym(digit + 1, 0);
        lcd_set_sym(digit + 2, 0);
    }

    lcd_set_digit(digit, lvalue % 10);
    lvalue /= 10;
    lcd_set_digit(digit + 1, lvalue % 10);
    lvalue /= 10;
    lcd_set_digit(digit + 2, lvalue % 10);

    ht1621b_write_disp_buffer(disp_buffer);
}

/***********************************************************************
 ***********************************************************************/
void lcd_set_voltage(float voltage)
{
    lcd_display_value(2, voltage);
}

/***********************************************************************
 ***********************************************************************/
void lcd_set_current(float current)
{
    lcd_display_value(1, current);
}

/***********************************************************************
 ***********************************************************************/
void lcd_set_power(float power)
{
    lcd_display_value(0, power);
}

/***********************************************************************
 ***********************************************************************/
void lcd_init(void)
{
    P00_PUSHPULL_MODE;
    P01_PUSHPULL_MODE;
    P10_PUSHPULL_MODE;

    HT1621B_CS = 1;
    HT1621B_WR = 1;

    ht1621b_command(SYS_EN, 12);
    ht1621b_command(LCD_ON, 12);
    ht1621b_command(BIAS_13_4COMMONS, 12);

    memset(disp_buffer, 0x00, sizeof(disp_buffer));

    lcd_set_sym(0, 1);
    lcd_set_sym(3, 1);
    lcd_set_sym(6, 1);

    ht1621b_write_disp_buffer(disp_buffer);
}
