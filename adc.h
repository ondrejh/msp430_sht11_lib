/*
 * adc.h .. adc module header file
 *
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <stdbool.h>

// temperature too high informative funciton
bool TemperatureTooHigh(void);

// module initialization
void adc_init(void);

#endif
