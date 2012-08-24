/*
 * timer.h
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

#ifndef __TIMER_H__
#define __TIMER_H__

// timer counter multiplier (10 * 0.5s = 5s)
#define TIMER_MULTIPLIER 10

// timer interval (0.5s / 1MHz osc / fosc/8)
#define TIMER_INTERVAL 62500

void timer_init(void);

#endif
