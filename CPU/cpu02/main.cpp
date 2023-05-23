
/*
 *  ======== main.c ========
 */
#include <string.h>
#include <math.h>

#include "stdafx.h"
#include "Init.h"
#include "HWIs.h"

void main()
{
    InitFlash();

    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    EALLOW;
    PieVectTable.XINT1_INT = &SD_FAST_INT;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    IER |= M_INT1;

    Init.PWMs();

    Init.CLA();

    Init.Variables();

    EALLOW;
    Cla1Regs.MIER.bit.INT1 = 1;
    EDIS;

    EINT;

    while(1)
    {
        static Uint32 timer_new = 0;
        static Uint32 timer_old = 0;
        timer_new = ReadIpcTimer();
        if(timer_new - timer_old > 20000000UL)
        {
            timer_old = timer_new;
        }
    }
}
