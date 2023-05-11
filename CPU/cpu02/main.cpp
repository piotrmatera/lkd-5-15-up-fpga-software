
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
    PieVectTable.IPC3_INT = &IPC3_INT;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx16 = 1;
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
        if(IpcRegs.IPCSTS.bit.IPC4)
        {
            IpcRegs.IPCACK.bit.IPC4 = 1;

            float rotation;
            float wTs = CLA1toCLA2.w_filter * Grid.Ts;
            rotation = CPU1toCPU2.CT_phase[0] * wTs;
            Grid.I_grid_rot[0].sine = sinf(rotation);
            Grid.I_grid_rot[0].cosine = cosf(rotation);
            rotation = CPU1toCPU2.CT_phase[1] * wTs;
            Grid.I_grid_rot[1].sine = sinf(rotation);
            Grid.I_grid_rot[1].cosine = cosf(rotation);
            rotation = CPU1toCPU2.CT_phase[2] * wTs;
            Grid.I_grid_rot[2].sine = sinf(rotation);
            Grid.I_grid_rot[2].cosine = cosf(rotation);
        }

        static Uint32 timer_new = 0;
        static Uint32 timer_old = 0;
        timer_new = ReadIpcTimer();
        if(timer_new - timer_old > 20000000UL)
        {
            timer_old = timer_new;

            SINCOS_kalman_calc_CPUasm(sincos_kalman_table, CLA1toCLA2.w_filter * Grid.Ts);
            SINCOS_calc_CPUasm(sincos_table, CLA1toCLA2.w_filter * Grid.Ts);
        }
    }
}
