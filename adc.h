/***********************************************************************
 ***********************************************************************/
#ifndef _H_ADC
#define _H_ADC

unsigned int adc_read_voltage_raw(void);
unsigned int adc_read_current_raw(void);
void adc_task(void);
static float adc_read_voltage(void);
static float adc_read_current(void);
void adc_init(void);

#endif
