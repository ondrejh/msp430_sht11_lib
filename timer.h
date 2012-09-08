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
#include <stdbool.h>

// button timing
#define BTN_SHORTPRESS_LIMIT 30
#define BTN_LONGPRESS_PERIOD 50
// button status interface variable
#define BTN1_PRESSED 0x01
#define BTN1_HOLD 0x02
#define BTN2_PRESSED 0x10
#define BTN2_HOLD 0x20
uint8_t button_status;

// timer interval (10ms / 1MHz osc / fosc/8)
#define TIMER_INTERVAL 1250
void timer_init(void);

// pwm period
#define PWM_PERIOD 100

void pwm_init(void);
void pwm_set(uint8_t val);

#endif
