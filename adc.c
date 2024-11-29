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

#define OFFSET_VOLTAGE 0.016
#define SCALE_VOLTAGE 39.96

#define OFFSET_CURRENT 0.100
#define SCALE_CURRENT 7.032

static const float real_table_v[] =
{
    0.3,
    0.4,
    0.5,
    1.0,
    2.0,
    3.0,
    5.0,
    10.0,
    20.0,
    40.0,
    80.0,
    100.0,
};

static const float adc_table_v[] =
{
    0.011,
    0.02,
    0.05,
    0.274,
    1.188,
    2.214,
    4.235,
    9.11,
    19.14,
    39.31,
    79.83,
    100.14,
};

static const float real_table_i[] =
{
    0.05,
    0.1,
    0.2,
    0.3,
    0.5,
    1.0,
    1.5,
    2.0,
    4.0,
    8.0,
    10.0,
};

static const float adc_table_i[] =
{
    0.057,
    0.106,
    0.207,
    0.311,
    0.527,
    1.054,
    1.55,
    2.046,
    4.01,
    8.03,
    10.1,
};

/***********************************************************************
 ***********************************************************************/
float interpolate(const float* real_table,
                  const float* adc_table,
                  unsigned int size,
                  float adc_value)
{
    unsigned int i;
    for (i = 0; i < (size - 1); i++)
    {
        if ((adc_table[i] <= adc_value) &&
             adc_value < adc_table[i + 1])
        {
            break;
        }
    }

    float adc_low = adc_table[i];
    float adc_high = adc_table[i + 1];
    float real_low = real_table[i];
    float real_high = real_table[i + 1];

    float real_value = real_low + ((adc_value - adc_low) / (adc_high - adc_low)) * (real_high - real_low);
    return real_value;
}

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

/* #define _CALIBRATE_ */

/***********************************************************************
 ***********************************************************************/
float adc_read_voltage(void)
{
    float value = ravg_average(&ravg_voltage) * adc_vdd / 4095.0;
    value = (value - OFFSET_VOLTAGE) * SCALE_VOLTAGE;

#ifndef _CALIBRATE_
    value = interpolate(real_table_v,
                        adc_table_v,
                        sizeof(real_table_v) / sizeof(real_table_v[0]),
                        value);
#endif

    return value;
}

/***********************************************************************
 ***********************************************************************/
float adc_read_current(void)
{
    float value = ravg_average(&ravg_current) * adc_vdd / 4095.0;
    value = (value - OFFSET_CURRENT) * SCALE_CURRENT;

#ifndef _CALIBRATE_
    value = interpolate(real_table_i,
                        adc_table_i,
                        sizeof(real_table_i) / sizeof(real_table_i[0]),
                        value);
#endif


    return value;
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
