#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include "../../sht11con.h"

/// double based conversion function - used as muster value
void sht2int_double(uint16_t tR, uint16_t hR, int16_t *T, int16_t *H)
{
    double tRd = (double) tR;
    double hRd = (double) hR;

    // linear RH
    double dRH = C1 + C2*hRd + C3*hRd*hRd;
    // temperature
    double dT = D1 + D2*tRd;
    // temp. compensated RH
    double dRHc = (dT - 25.0) * (T1 - T2*tRd) + dRH;
    if (dRHc>100.0) dRHc=100.0;
    if (dRHc<0) dRHc=0;

    // convert to int
    int16_t iRH = (int16_t)((double)(dRHc*10.0));
    int16_t iT = (int16_t)((double)(dT*10.0));

    // return values
    *T = iT;
    *H = iRH;
}

/// test body
int main(int argc, char *argv[])
{
    uint16_t tReg = 0;
    uint16_t hReg = 0;

    int16_t tVal = 0;
    int16_t hVal = 0;

    if (argc==3) // if reg values given in args - just compute one result
    {
        // get values from input args
        if (argv[2][strlen(argv[2])-1]=='h') sscanf(argv[2],"%X",(unsigned int*)&hReg);
        else sscanf(argv[2],"%d",(unsigned int*)&hReg);
        if (argv[1][strlen(argv[1])-1]=='h') sscanf(argv[1],"%X",(unsigned int*)&tReg);
        else sscanf(argv[1],"%d",(unsigned int*)&tReg);
        // show it
        printf("temp.reg.: 0x%04X; humi.reg.: 0x%04X\n",tReg,hReg);

        // convert
        sht2int_double(tReg,hReg,&tVal,&hVal);
        // show result
        printf("\nTemp(int): %d; Hum(int): %d\n",tVal,hVal);
        // test int2bcd function
        printf("Temp(bcd): %X; Hum(bcd): %X\n",int2bcd(tVal),int2bcd(hVal));
        return 0;
    }

    int16_t tValF, hValF;
    uint16_t eTmax=0, eHmax=0;
    double eTavg=0, eHavg=0;
    double cnt=0.0;

    printf("Converting all 2^26 combinations ... ");
    clock_t start = clock();
    while(1)
    {
        cnt+=1.0;
        // convert
        sht2int_double(tReg,hReg,&tVal,&hVal);
        // convert (other way)
        sht2int(tReg,hReg,&tValF,&hValF);
        int e = abs(tValF-tVal);
        eTavg+=e;
        if (eTmax<e) eTmax=e;
        e = abs(hValF-hVal);
        eHavg+=e;
        if (eHmax<e) eHmax=e;

        // values for next conversion
        hReg ++;
        if (hReg==4096) {hReg=0;tReg++;}
        if (tReg==16384) break;
    }
    eTavg/=cnt;
    eHavg/=cnt;

    printf ( "%.2fs\n", ( (double)clock() - start ) / CLOCKS_PER_SEC );
    printf("Max errors T: %d, H: %d\n",eTmax,eHmax);
    printf("Avg error (%.0f samples) T: %f, H: %f",cnt,eTavg,eHavg);
    return 0;
}
