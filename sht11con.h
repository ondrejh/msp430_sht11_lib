/*
 * sht11con.h
 *
 *  Created on: 4.9.2012
 *      Author: ohejda
 */

#ifndef __SHT11CON_H__
#define __SHT11CON_H__

#include <inttypes.h>

/// sht registers to int conversion
void sht2int(uint16_t tR, uint16_t hR, int16_t *T, int16_t *H);
/// function converting int (-7999 .. 7999) to bcd with sign (msb)
uint16_t int2bcd(int16_t w);

#endif
