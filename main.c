//******************************************************************************
// Launchpad test application. PWM DC motor drive.
//
// author: Ondrej Hejda
// date:   5.9.2012
//
// used TI demos:
//   MSP430G2xx3 Demo - USCI_A0, 9600 UART Echo ISR, DCO SMCLK
//   MSP430G2xx3 Demo - P1 Interrupt from LPM4 with Internal Pull-up
//   MSP430G2xx3 Demo - Timer_A, Toggle P1.0, CCR0 Cont. Mode ISR, DCO SMCLK
//   ...
//
// hardware: MSP430G2553 (launchpad)
//
//                MSP4302553
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |           P1.1,2|--> debug UART (debug output 9.6kBaud)
//            |                 |
//            |             P1.0|--> launchpad on board RED LED (active high)
//            |             P1.6|--> launchpad on board GREEN LED (active high)
//            |                 |
//            |                 |                                -------
//            |             P2.2|--> PWM OUT -----------------> | MOTOR |
//            |                 |                                -------
//            |             P2.3|--> LED (active high)
//            |             P2.4|--> BTN1 (pullup, active low)
//            |             P2.5|--> BTN2 (pullup, active low)
//            |                 |
//
//******************************************************************************

#define DEBUG

// include section
#include <msp430g2553.h>
#include "timer.h"
#include "board.h"

#ifdef DEBUG
#include "uart.h"
#endif

// hw depended init
void board_init(void)
{
	// oscillator
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO
	DCOCTL = CALDCO_1MHZ;

	LED_INIT(); // launchpad onboard leds
	PLED_INIT(); // external led
	BUTTONS_INIT(); // external buttons
}



// main program body
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop WDT

	board_init(); 	// init oscilator and leds
	timer_init(); 	// init timer
	pwm_init();     // init pwm

	#ifdef DEBUG
	uart_init(); // init debug interface
	set_debug_value(0x0,0);	// store value for debug interface
	set_debug_value(0x0,1);
	#endif

	pwm_set(25); // test

	while(1)
	{
	    if (BTN1_DOWN) PLED_ON();
	    if (BTN2_DOWN) PLED_OFF();
		__bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on timer interrupt)
	}

	return -1;
}
