//******************************************************************************
// Launchpad test application. SHT11 sensor module
//
// author: Ondrej Hejda
// date:   22.8.2012
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
//            |           P1.1,2|--> UART (debug output 9.6kBaud)
//            |                 |
//            |             P1.0|--> RED LED (active high)
//            |             P1.6|--> GREEN LED (active high)
//            |                 |                                     -------
//            |             P1.5|<-> SHT DATA <-----------pullup---->| SHT11 |
//            |             P1.4|--> SHT SCK -------------pullup---->|       |
//            |                 |                                     -------
//            |                 |
//            |                 |      -----------    +5V
//            |            P2.0 |---->| D4 |       |   |
//            |            P2.1 |---->| D5 |       |---
//            |            P2.2 |---->| D6 |  LCD  |
//            |            P2.3 |---->| D7 |  16x2 |
//            |            P2.4 |---->| EN |       |---
//            |            P2.5 |---->| RS |       |   |
//            |                 |      ------------   ---
//            |                 |

//******************************************************************************

#define DEBUG

// include section
#include <msp430g2553.h>
#include "timer.h"
#include "sht11.h"
#include "sht11con.h"
#include "lcd.h"

#ifdef DEBUG
#include "uart.h"
#endif

// board (leds, button)
#define LED_INIT() {P1DIR|=0x41;P1OUT&=~0x41;}
#define LED_RED_ON() {P1OUT|=0x01;}
#define LED_RED_OFF() {P1OUT&=~0x01;}
#define LED_RED_SWAP() {P1OUT^=0x01;}
#define LED_GREEN_ON() {P1OUT|=0x40;}
#define LED_GREEN_OFF() {P1OUT&=~0x40;}
#define LED_GREEN_SWAP() {P1OUT^=0x40;}

// hw depended init
void board_init(void)
{
	// oscillator
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO
	DCOCTL = CALDCO_1MHZ;

	LED_INIT(); // leds
}

void print_val(int val,char *s)
{
    int uV = val;
    s[0]=' ';
    if (val<0)
    {
        s[0]='-';
        uV=-val;
    }
    else if (uV>=1000) s[0]=1;
    if (uV>=100) s[1]=(uV%1000/100)+'0';
    else {s[1]=s[0]; s[0]=' ';};
    s[2]=(uV%100/10)+'0';
    s[3]='.';
    s[4]=(uV%10)+'0';
    s[5]='\0';
}

// main program body
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop WDT

	board_init(); 	// init oscilator and leds
	lcm_init();     // display
	timer_init(); 	// init timer
	sht11_init(); 	// init sht sensor

	#ifdef DEBUG
	uart_init(); // init debug interface
	set_debug_value(0x0,0);	// store value for debug interface
	set_debug_value(0x0,1);
	#endif

	lcm_clearscr(); // clear screen
	lcm_goto(0,0);
	lcm_prints("Temp.:   xxx.x C");
	lcm_goto(1,0);
	lcm_prints("Humi.:   xxx.x %");

	while(1)
	{
		unsigned int Tval,Hval;
		int TvalC,HvalC;
		char str[8];
		LED_GREEN_ON();
		if ((sht_measure_check(&Tval,TEMP)==0) && (sht_measure_check(&Hval,HUMI)==0))
		{
			sht2int(Tval,Hval,&TvalC,&HvalC);
			#ifdef DEBUG
			set_debug_value(int2bcd(TvalC),0);
			set_debug_value(int2bcd(HvalC),1);
			#endif
			print_val(TvalC,str);
			lcm_goto(0,9);
			lcm_prints(str);
			print_val(HvalC,str);
			lcm_goto(1,9);
			lcm_prints(str);
		}
	    LED_GREEN_OFF();
		__bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on timer interrupt)
	}

	return -1;
}
