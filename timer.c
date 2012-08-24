/*
 * timer.c
 *
 *  Created on: 22.8.2012
 *      Author: ohejda
 *
 *  Description: timer module using interrupts
 *
 *  Functions:
 *  	timer_init(void) .. timer initialization
 *
 *  Interrupt routines:
 *  	Timer A0 interrupt service routine .. set new timeout and exit sleep mode
 *
 */

// include section
#include <msp430g2553.h>
// self
#include "timer.h"

// timer init
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
	static unsigned int cnt = 0;
	CCR0 += TIMER_INTERVAL;				// Add Offset to CCR0
	cnt++;
	if (cnt>=TIMER_MULTIPLIER)
	{
		cnt=0;
		__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
	}
}
