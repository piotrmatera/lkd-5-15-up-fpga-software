/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <math.h>

#include "stdafx.h"
#include "HWIs.h"

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void SD_FAST_INT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

    register Uint32 *src;
    register Uint32 *dest;

    src = (Uint32 *)&EMIF_mem.read.U_grid_a;
    dest = (Uint32 *)&EMIF_CLA.U_grid_a;

    Cla1ForceTask1();

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    src = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.id_ref;
    dest = (Uint32 *)&Conv.id_ref;

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    Conv.MR_ref.a = (float)EMIF_mem.read.Resonant[0].series[0].sum * Conv.div_range_modifier;
    Conv.MR_ref.b = (float)EMIF_mem.read.Resonant[0].series[1].sum * Conv.div_range_modifier;
    Conv.MR_ref.c = (float)EMIF_mem.read.Resonant[0].series[2].sum * Conv.div_range_modifier;
    Conv.MR_ref.a += (float)EMIF_mem.read.Resonant[1].series[0].sum * Conv.div_range_modifier;
    Conv.MR_ref.b += (float)EMIF_mem.read.Resonant[1].series[1].sum * Conv.div_range_modifier;
    Conv.MR_ref.c += (float)EMIF_mem.read.Resonant[1].series[2].sum * Conv.div_range_modifier;

    Conv.cycle_period = EMIF_mem.read.cycle_period;

    Timer_PWM.CPU_MEAS = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    Conv.zero_error = 1.0f;
    if (fmaxf(fmaxf(fabsf(Conv.duty[0]), fabsf(Conv.duty[1])), fabsf(Conv.duty[2])) > Conv.cycle_period)
        Conv.zero_error = 0;

    static volatile Uint32 Resonant_WIP;
    Resonant_WIP = EMIF_mem.read.flags.Resonant1_WIP;

    register float modifier = Conv.range_modifier * Conv.zero_error;
    EMIF_mem.write.Resonant[1].series[0].error =
    EMIF_mem.write.Resonant[0].series[0].error = Conv.I_err.a * modifier;
    EMIF_mem.write.Resonant[1].series[1].error =
    EMIF_mem.write.Resonant[0].series[1].error = Conv.I_err.b * modifier;
    EMIF_mem.write.Resonant[1].series[2].error =
    EMIF_mem.write.Resonant[0].series[2].error = Conv.I_err.c * modifier;

    EMIF_mem.write.DSP_start = 3UL;

    Timer_PWM.CPU_MR_START = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx2);
    PieCtrlRegs.PIEIFR11.bit.INTx2 = 0;

    *(Uint32 *)&EMIF_mem.write.duty[0] = *(Uint32 *)&Conv.duty[0];
    *(Uint32 *)&EMIF_mem.write.duty[2] = *(Uint32 *)&Conv.duty[2];

    Timer_PWM.CPU_PWM = TIMESTAMP_PWM;

    static Uint16 decimator_cpu2 = 0;

    if(decimator_cpu2++)
    {
        decimator_cpu2 = 0;

        CPU2toCPU1.CLA2toCLA1.w_filter = PLL.w_filter;

        IpcRegs.IPCSET.bit.IPC3 = 1;
        IpcRegs.IPCCLR.bit.IPC3 = 1;
    }

    Timer_PWM.CPU_SD_end = TIMESTAMP_PWM;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
