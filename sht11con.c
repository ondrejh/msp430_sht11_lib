/*
 * sht11con.c
 *
 *  Created on: 4.9.2012
 *      Author: ohejda
 *
 *  SHT11 data conversion module for MSP430
 *
 *  interface functions:
 *
 *      sht2int(regT,regH,*T,*H) .. converting register values into sensful inteters
 *      int2bcd(w) .. converting int to bcd value (with sign) - easier displaying
 *
 */

#include "sht11con.h" // self

/** interface section */

/// sht registers to int conversion
void sht2int(uint16_t tR, uint16_t hR, int16_t *T, int16_t *H)
{
    float tRd = (float) tR;
    float hRd = (float) hR;

    // linear RH
    float dRH = C1 + C2*hRd + C3*hRd*hRd;
    // temperature
    float dT = D1 + D2*tRd;
    // temp. compensated RH
    float dRHc = (dT - 25.0) * (T1 - T2*tRd) + dRH;
    if (dRHc>100.0) dRHc=100.0;
    if (dRHc<0) dRHc=0;

    // convert to int
    int16_t iRH = (int16_t)((float)(dRHc*10.0));
    int16_t iT = (int16_t)((float)(dT*10.0));

    // return values
    *T = iT;
    *H = iRH;
}

/// function converting int (-7999 .. 7999) to bcd with sign (msb)
/// examples 1) -158 -> 0x8158 2) 1234 -> 0x1234
uint16_t int2bcd(int16_t w)
{
    uint16_t buf;
    // test for limits
    if (w>7999) return 0x7999;
    if (w<-7999) return 0xF999; // (0x7999|0x8000)
    // test for <0 values
    if ((w&0x8000)!=0) buf=-w; else buf=w;
    // convert
    uint16_t bcd = buf%10;
    uint16_t mul = 1;
    buf /= 10;
    while (buf!=0)
    {
        mul <<= 4;
        bcd |= buf%10*mul;
        buf /= 10;
    }
    // add sign
    bcd|=(w&0x8000);
    // return value
    return bcd;
}
