/***********************************************************************
 ***********************************************************************/
#include "numicro_8051.h"
#include "debug_log.h"

#define RAVG_BUFCNT 20

typedef struct
{
    unsigned int buffer[RAVG_BUFCNT];
    unsigned char index;
    unsigned long sum;

} RAVG;

static float adc_vdd;
static RAVG ravg_voltage;
static RAVG ravg_current;

#define OFFSET_VOLTAGE 0.020
#define SCALE_VOLTAGE 1

#define OFFSET_CURRENT 0.100
#define SCALE_CURRENT 1


/***********************************************************************
 ***********************************************************************/
static void ravg_run(RAVG* ravg, unsigned int data)
{
    ravg->sum -= ravg->buffer[ravg->index];
    ravg->sum += data;
    ravg->buffer[ravg->index] = data;
    ravg->index = (ravg->index + 1) % RAVG_BUFCNT;
}

/***********************************************************************
 ***********************************************************************/
static float ravg_average(RAVG* ravg)
{
    return (float)ravg->sum / RAVG_BUFCNT;
}

/***********************************************************************
 ***********************************************************************/
static unsigned int adc_read_raw(void)
{
    clr_ADCCON0_ADCF;
    set_ADCCON0_ADCS;
    while ((ADCCON0 & SET_BIT7) == 0)
    {
        ;
    }

    unsigned int data = (ADCRH << 4) | (ADCRL & 0x0f);
    return data;
}

/***********************************************************************
 ***********************************************************************/
unsigned int adc_read_voltage_raw(void)
{
    ENABLE_ADC_AIN6;
    return adc_read_raw();
}

/***********************************************************************
 ***********************************************************************/
unsigned int adc_read_current_raw(void)
{
    ENABLE_ADC_AIN5;
    return adc_read_raw();
}

/***********************************************************************
 ***********************************************************************/
static unsigned int adc_bandgap_stored(void)
{
    unsigned int data;

    set_IAPEN;
    IAPAL = 0x0C;
    IAPAH = 0x00;
    IAPCN = 0x04;
    set_IAPGO;
    data = IAPFD;
    IAPAL = 0x0d;
    IAPAH = 0x00;
    IAPCN = 0x04;
    set_IAPGO;
    data = (data << 4) | (IAPFD & 0x0f);
    clr_IAPEN;

    debug_log(1, "data = %u\n", data);
    return data;
}

/***********************************************************************
 ***********************************************************************/
static unsigned int adc_bandgap_actual(void)
{
    ENABLE_ADC_BANDGAP;

    adc_read_raw();
    adc_read_raw();
    adc_read_raw();

    unsigned int data = adc_read_raw();

    debug_log(1, "data = %u\n", data);
    return data;
}

/***********************************************************************
 ***********************************************************************/
void adc_task(void)
{
    ravg_run(&ravg_voltage, adc_read_voltage_raw());
    ravg_run(&ravg_current, adc_read_current_raw());
}

/***********************************************************************
 ***********************************************************************/
float adc_read_voltage(void)
{
    float value = ravg_average(&ravg_voltage) * adc_vdd / 4095.0 - OFFSET_VOLTAGE;

    return value * SCALE_VOLTAGE;
}

/***********************************************************************
 ***********************************************************************/
float adc_read_current(void)
{
    float value = ravg_average(&ravg_current) * adc_vdd / 4095.0 - OFFSET_CURRENT;

    return value * SCALE_CURRENT;
}

/***********************************************************************
 ***********************************************************************/
void adc_init(void)
{
    unsigned int bg_stored = adc_bandgap_stored();
    unsigned int bg_actual = adc_bandgap_actual();

    float vbg = 3072.0 * bg_stored / 4096.0;
    adc_vdd = 4095.0 / bg_actual * vbg / 1000.0;

    debug_log(1, "bgs = %u, bga = %u, vbg = %f, vdd = %f\n",
              bg_stored, bg_actual, vbg, adc_vdd);
}
