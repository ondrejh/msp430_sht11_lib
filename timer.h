/*
 * timer.h
 *
 *  Created on: 22.8.2012
 *      Author: ohejda
 *
 *  Description: timer module using interrupts
 *               free running pwm
 *
 *  Functions:
 *  	timer_init(void) .. timer initialization
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

#ifndef __TIMER_H__
#define __TIMER_H__

#include <inttypes.h>

// timer counter multiplier (10 * 0.5s = 5s)
#define TIMER_MULTIPLIER 10

// timer interval (0.5s / 1MHz osc / fosc/8)
#define TIMER_INTERVAL 62500

void timer_init(void);

// pwm period
#define PWM_PERIOD 100

void pwm_init(void);
void pwm_set(uint8_t val);

#endif
