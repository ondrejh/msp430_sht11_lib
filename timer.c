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
#include "board.h"
// self
#include "timer.h"

// button status interface variable
uint8_t button_status = 0;

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

    // button filtering registers
    static bool fbutton[2]={false,false};
    static uint8_t btnbuf[2]={0,0};

    // button filtering
    uint16_t i;
    btnbuf[0]<<=1;
    btnbuf[1]<<=1;
    if (BTN1_DOWN) btnbuf[0]|=0x01;
    if (BTN2_DOWN) btnbuf[1]|=0x01;
    for (i=0;i<2;i++)
    {
        btnbuf[i]<<=1;
        if BTN_DOWN(i) btnbuf[i]|=0x01;
        if ((btnbuf[i]&0x07)==0x07) fbutton[i]=true;
        if ((btnbuf[i]&0x07)==0x00) fbutton[i]=false;
    }
    // debug mirror of filtered buttons
    i = get_debug_value(1);
    i &= 0xFCFF;
    if (fbutton[0]) i|=0x0100;
    if (fbutton[1]) i|=0x0200;
    set_debug_value(i,1);

    // buttons sequential
    static uint8_t btn_seqv[2] = {0,0};
    static uint8_t btn_timer[2] = {0,0};
    button_status = 0;
    for (i=0;i<2;i++)
    {
        switch (btn_seqv[i])
        {
            case 0: // (awaiting) button press
                if (fbutton[i])
                {
                    // reset timer and goto next state
                    btn_timer[i]=0;
                    btn_seqv[i]++;
                }
                break;
            case 1: // button leave (short press) or holding timeout
                if (!fbutton[i])
                {
                    // set button pressed flag
                    button_status |= (i==0) ? BTN1_PRESSED : BTN2_PRESSED;
                    // back to waiting buttonpress
                    btn_seqv[i]=0;
                }
                btn_timer[i]++;
                if (btn_timer[i]>=BTN_SHORTPRESS_LIMIT)
                {
                    // set button hold flag
                    button_status |= (i==0) ? BTN1_HOLD : BTN2_HOLD;
                    // reset timer
                    btn_timer[i]= 0;
                    // goto button holding state
                    btn_seqv[i]++;
                }
                break;
            case 2: // holding end (first time)
                if (!fbutton[i]) btn_seqv[i]=0; // back to waiting button press
                btn_timer[i]++;
                if (btn_timer[i]>=BTN_LONGPRESS_1ST_PERIOD)
                {
                    // set button hold flag
                    button_status |= (i==0) ? BTN1_HOLD : BTN2_HOLD;
                    // clear timer for next period
                    btn_timer[i]=0;
                    // goto repeated holding
                    btn_seqv[i]++;
                }
                break;
            case 3: // holding end (repeated)
                if (!fbutton[i]) btn_seqv[i]=0; // back to waiting button press
                btn_timer[i]++;
                if (btn_timer[i]>=BTN_LONGPRESS_REPEATED_PERIOD)
                {
                    // set button hold flag
                    button_status |= (i==0) ? BTN1_HOLD : BTN2_HOLD;
                    // clear timer for next period
                    btn_timer[i]=0;
                }
                break;
            default: // should never happen
                btn_seqv[i]=0;
                break;
        }
    }

    // if something to do in main .. wake up the mcu
    if (button_status!=0) __bic_SR_register_on_exit(CPUOFF); // Clear CPUOFF bit from 0(SR)
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
