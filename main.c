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
//            | ADC10       P2.2|--> PWM OUT -----------------> | MOTOR |
//            | int.            |                                -------
//            | temp.           |
//            | sens.       P2.3|--> LED (active high)
//            |             P2.4|--> BTN1 (pullup, active low)
//            |             P2.5|--> BTN2 (pullup, active low)
//            |                 |
//
//******************************************************************************

#define POWER_STEP 5

// include section
#include <msp430g2553.h>
#include "timer.h"
#include "board.h"
#include "adc.h"
#include "uart.h"

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
	adc_init();

	uart_init(); // init debug interface
	set_debug_value(0x0,0);	// store value for debug interface
	set_debug_value(0x0,1);

	uint8_t power = 25;
	bool motor_on = false;

	while(1)
	{
	    switch (button_status)
	    {
	        case BTN1_PRESSED: // on
                if (!motor_on) motor_on=true;
                else power+=POWER_STEP;
                break;
            case BTN2_PRESSED: // off
                motor_on=false;
                break;
            case BTN1_HOLD: // power up
                power+=POWER_STEP;
                break;
            case BTN2_HOLD: // power down
                power-=POWER_STEP;
                break;
	    }

	    if (power>100) power=100;
	    if (power<=0) {power=POWER_STEP;motor_on=false;}

	    if (motor_on)
	    {
	        pwm_set(power);
	        PLED_ON();
	    }
	    else
	    {
	        pwm_set(0);
	        PLED_OFF();
	    }
		__bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on timer interrupt)
	}

	return -1;
}
