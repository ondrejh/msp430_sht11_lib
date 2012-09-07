/*
 * timer.c
 *
 *  Created on: 22.8.2012
 *      Author: ohejda
 *
 *  Description: timer module using interrupts
 *               free running pwm
 *
 *  Functions:
 *  	timer_init(void) .. timer initialization (timer 0A)
 *
 *      pwm_init(void) .. pwm initialization (timer 1A)
 *      pwm_set(uint8_t val) .. set pwm value
 *
 *  Interrupt routines:
 *  	Timer A0 interrupt service routine .. set new timeout and exit sleep mode
 *
 *  edit 5.9.2012: pwm module added (no interrupt)
 *
 */

// include section
#include <msp430g2553.h>
#include "uart.h"
// self
#include "timer.h"

// timer init (timer 0A)
void timer_init(void)
{
	CCTL0 = CCIE;				// CCR0 interrupt enabled
	CCR0 = TIMER_INTERVAL;
	TACTL = TASSEL_2 + MC_2 + ID_3;	// SMCLK, contmode, fosc/8
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    CCR0 += TIMER_INTERVAL;	// Add Offset to CCR0
	__bic_SR_register_on_exit(CPUOFF); // Clear CPUOFF bit from 0(SR)
}

// init pwm timer (timer 1A)
void pwm_init(void)
{
    TA1CCR0  = PWM_PERIOD-1; // pwm period
    TA1CCTL1 = OUTMOD_7; // T1ACCR1 reset/set
    TA1CTL   = TASSEL_2 + MC_1; // SMCLK, upmode
    TA1CCR1  = 0; // pwm initial value
    P2DIR   |= BIT2; // P2.2 as output
    P2SEL   |= BIT2; // alternate function on
}

// set pwm value (val should be less than PWM_PERIOD)
void pwm_set(uint8_t val)
{
    TA1CCR1 = val;
    set_debug_value((get_debug_value(1)&0xFF80)|val,1);
}
