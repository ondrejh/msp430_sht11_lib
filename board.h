/*
 * board.h .. leds, buttons etc.
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#define LED_INIT() {P1DIR|=0x41;P1OUT&=~0x41;}
#define LED_RED_ON() {P1OUT|=0x01;}
#define LED_RED_OFF() {P1OUT&=~0x01;}
#define LED_RED_SWAP() {P1OUT^=0x01;}
#define LED_GREEN_ON() {P1OUT|=0x40;}
#define LED_GREEN_OFF() {P1OUT&=~0x40;}
#define LED_GREEN_SWAP() {P1OUT^=0x40;}

#define PLED_INIT() {P2DIR|=0x08;P2OUT&=~0x08;}
#define PLED_ON() {P2OUT|=0x08;}
#define PLED_OFF() {P2OUT&=~0x08;}
#define PLED_SWAP() {P2OUT^=0x08;}
#define BUTTONS_INIT() {P2DIR&=~0x30;P2OUT&=~0x30;}
#define BTN1_DOWN ((P2IN&0x10)==0)
#define BTN2_DOWN ((P2IN&0x20)==0)
#define BTN_DOWN(x) ((x==0)?BTN1_DOWN:BTN2_DOWN)

#endif
