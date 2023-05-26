
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
    XintRegs.XINT3CR.bit.POLARITY = 0;
    XintRegs.XINT3CR.bit.ENABLE = 1;
    PieVectTable.XINT3_INT = &SD_NEW_INT;
    EDIS;
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1;
    IER |= M_INT12;

    Init.PWMs();

    Init.CLA();

    Init.Variables();

    Init.EPwm_TZclear(&EPwm5Regs);

    EALLOW;
    Cla1Regs.MIER.bit.INT1 = 1;
    EDIS;

    EINT;

    while(1)
    {
        if(CPU1toCPU2.clear_alarms) Init.EPwm_TZclear(&EPwm5Regs);

        if(IpcRegs.IPCSTS.bit.IPC4)
        {
            IpcRegs.IPCACK.bit.IPC4 = 1;
            Meas_master_gain = CPU1toCPU2.Meas_master_gain;
            Meas_master_offset = CPU1toCPU2.Meas_master_offset;
        }

        static Uint32 timer_new = 0;
        static Uint32 timer_old = 0;
        timer_new = ReadIpcTimer();
        if(timer_new - timer_old > 20000000UL)
        {
            timer_old = timer_new;
        }
    }
}
