/*
 * sht11con.h
 *
 *  Created on: 4.9.2012
 *      Author: ohejda
 */

#ifndef __SHT11CON_H__
#define __SHT11CON_H__

#include <inttypes.h>

/** conversion constants (see sht11 datasheet) */

/// RH 12bit
#define C1 -2.0468
#define C2 0.0367
#define C3 -1.5955e-6

/// T 14bit 3.5V
#define D1 -39.7
#define D2 0.01

/// RH temp. compensation (RH 12bit)
#define T1 0.01
#define T2 0.00008

/// sht registers to int conversion
void sht2int(uint16_t tR, uint16_t hR, int16_t *T, int16_t *H);
/// function converting int (-7999 .. 7999) to bcd with sign (msb)
uint16_t int2bcd(int16_t w);

#endif
