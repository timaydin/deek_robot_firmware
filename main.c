/***********************************************************************
 ***********************************************************************/
#include "numicro_8051.h"
#include "debug_log.h"
#include "console.h"
#include "adc.h"
#include "lcd.h"
#include "util.h"

/***********************************************************************
 ***********************************************************************/
static void gpio_init(void)
{
    P15_PUSHPULL_MODE;
}

/***********************************************************************
 ***********************************************************************/
void main(void)
{
    gpio_init();

    console_init(1000000);

    lcd_init();

    adc_init();

    unsigned int count = 0;
    while (1)
    {
        adc_task();

        if (count > 0)
        {
            --count;
        }
        else
        {
            count = 500;

            float voltage = adc_read_voltage();
            debug_log(1, "voltage = %f\n", voltage);
            lcd_set_voltage(voltage);

            float current = adc_read_current();
            debug_log(1, "current = %f\n", current);
            lcd_set_current(current);

            lcd_set_power(voltage * current);
        }
    }
}
