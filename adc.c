/*
 * adc.c
 *
 * Created on: 7.9.2012
 *     Author: ondrejh.ck@gmail.com
 *
 * Description:
 * After initialization it scans internal temperature sensor (ADC10),
 * computing moving average from defined number of samples and
 * testing if its less than definet threshold (overtemperature detection)
 *
 */

// include section
#include <msp430g2553.h>

#include "uart.h"
#include "adc.h"

// ADC_AVG = (T+278)*1024/423*AVG_BUFFLEN
// max temperature 65
#define ADC_TEMP_THRESHOLD 0x33E5
// restore temperature 60
#define ADC_TEMP_RESTORE 0x3323
bool overtemp = false;

#define AVG_BUFFLEN 16
int8_t bufptr = 0;//-1;
uint16_t buf[AVG_BUFFLEN];
uint16_t adc_avg;

// temperature over limit informative function
// returns internal overtemp bool value
bool TemperatureTooHigh(void)
{
    return overtemp;
}

// init adc
void adc_init(void)
{
	// init temperature measurement adc
	ADC10CTL1 = ADC10DIV_3 + INCH_10;// + CONSEQ_2; // A10
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + /*REF2_5V +*/ ADC10IE + REFON + ADC10ON; // ref 2.5V
	ADC10AE0 |= 0x20; // PA.5 ADC option select
    ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    // compute floating average
    adc_avg-=buf[bufptr];
    buf[bufptr]=ADC10MEM;
    adc_avg+=buf[bufptr];
    bufptr++;
    bufptr%=AVG_BUFFLEN;

    // save debug value
    set_debug_value(adc_avg,0);

    if (overtemp) // awaiting temp restore
    {
        if (adc_avg<ADC_TEMP_RESTORE)
        {
            overtemp=false; // clear flag
            set_debug_value(get_debug_value(1)&~0x8000,1);
        }
    }
    else
    {
        if (adc_avg>ADC_TEMP_THRESHOLD)
        {
            overtemp=true;
            set_debug_value(get_debug_value(1)|0x8000,1);
        }
    }

    // restart conversion
    ADC10CTL0 |= ENC + ADC10SC;
}
