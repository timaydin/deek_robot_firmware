/***********************************************************************
 ***********************************************************************/
#include "numicro_8051.h"
#include "debug_log.h"
#include "console.h"
#include "adc.h"

/***********************************************************************
 ***********************************************************************/
static void gpio_init(void)
{
    P15_PUSHPULL_MODE;
}

/***********************************************************************
 ***********************************************************************/
static void busy_loop_delay(unsigned int count)
{
    unsigned int i;
    for (i = 0; i < count; ++i)
    {
        ;
    }
}

/***********************************************************************
 ***********************************************************************/
void main(void)
{
    gpio_init();

    console_init(1000000);

    adc_init();

    unsigned long i;
    for (i = 0; i < 100000lu; ++i)
    {
        adc_task();

        float voltage = adc_read_voltage();

        debug_log(1, "voltage = %f\n", voltage);

        busy_loop_delay(30000);
    }

    while (1)
    {
        ;
    }
}