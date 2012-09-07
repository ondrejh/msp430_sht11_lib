/*
 * adc module .. reading internal temprerature sensor
 *
 */

// include section
#include <msp430g2553.h>

#include "uart.h"
#include "adc.h"

#define AVG_BUFFLEN 16
int8_t bufptr = 0;//-1;
uint16_t buf[AVG_BUFFLEN];
uint16_t adc_avg;

// init adc
void adc_init(void)
{
	// init temperature measurement adc
	ADC10CTL1 = ADC10DIV_3 + INCH_10 + CONSEQ_2; // A10
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REF2_5V + ADC10IE + REFON + ADC10ON; // ref 2.5V
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

    // restart conversion
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 |= ENC + ADC10SC;
}
