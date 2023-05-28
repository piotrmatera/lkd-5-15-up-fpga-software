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
interrupt void SD_NEW_INT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

    register Uint32 *src;
    register Uint32 *dest;

    src = (Uint32 *)&EMIF_mem.read.SD_new;
    dest = (Uint32 *)&EMIF_CLA;

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    Cla1ForceTask1();

    src = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.id_ref;
    dest = (Uint32 *)&Conv.id_ref;

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    register float modifier1 = Conv.div_range_modifier_Resonant_values;
    Conv.MR_ref.a = (float)EMIF_mem.read.Resonant[0].series[0].sum * modifier1;
    Conv.MR_ref.b = (float)EMIF_mem.read.Resonant[0].series[1].sum * modifier1;
    Conv.MR_ref.c = (float)EMIF_mem.read.Resonant[0].series[2].sum * modifier1;
    Conv.MR_ref.a += (float)EMIF_mem.read.Resonant[1].series[0].sum * modifier1;
    Conv.MR_ref.b += (float)EMIF_mem.read.Resonant[1].series[1].sum * modifier1;
    Conv.MR_ref.c += (float)EMIF_mem.read.Resonant[1].series[2].sum * modifier1;

    modifier1 = Conv.div_range_modifier_Kalman_values;
    Conv.Kalman_U_grid.a = (float)EMIF_mem.read.Kalman[0].series[0].estimate * modifier1;
    Conv.Kalman_U_grid.b = (float)EMIF_mem.read.Kalman[0].series[1].estimate * modifier1;
    Conv.Kalman_U_grid.c = (float)EMIF_mem.read.Kalman[0].series[2].estimate * modifier1;

    Conv.cycle_period = EMIF_mem.read.cycle_period;
    Conv.Kp_I = CPU1toCPU2.CLA1toCLA2.Kp_I;
    Conv.L_conv = CPU1toCPU2.CLA1toCLA2.L_conv;
    Conv.enable = CPU1toCPU2.CLA1toCLA2.enable;

    Timer_PWM.CPU_MEAS = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    Timer_PWM.CPU_IERR = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    Conv.zero_error = 1.0f;
    if (fmaxf(fmaxf(fabsf(Conv.duty[0]), fabsf(Conv.duty[1])), fabsf(Conv.duty[2])) > Conv.cycle_period)
        Conv.zero_error = 0;

    static volatile Uint32 Resonant_WIP;
    Resonant_WIP = EMIF_mem.read.flags.Resonant1_WIP;

    if(Conv.enable)
    {
        register float modifier2 = Conv.range_modifier_Resonant_values * Conv.zero_error;
        EMIF_mem.write.Resonant[1].series[0].error =
        EMIF_mem.write.Resonant[0].series[0].error = Conv.I_err.a * modifier2;
        EMIF_mem.write.Resonant[1].series[1].error =
        EMIF_mem.write.Resonant[0].series[1].error = Conv.I_err.b * modifier2;
        EMIF_mem.write.Resonant[1].series[2].error =
        EMIF_mem.write.Resonant[0].series[2].error = Conv.I_err.c * modifier2;
    }
    else
    {
        register float modifier2 = Conv.range_modifier_Resonant_values;
        EMIF_mem.write.Resonant[1].series[0].error =
        EMIF_mem.write.Resonant[0].series[0].error = (Meas_master.U_grid.a - Conv.MR_ref.a) * modifier2;
        EMIF_mem.write.Resonant[1].series[1].error =
        EMIF_mem.write.Resonant[0].series[1].error = (Meas_master.U_grid.b - Conv.MR_ref.b) * modifier2;
        EMIF_mem.write.Resonant[1].series[2].error =
        EMIF_mem.write.Resonant[0].series[2].error = (Meas_master.U_grid.c - Conv.MR_ref.c) * modifier2;
    }

    EMIF_mem.write.DSP_start = 3UL;

    Timer_PWM.CPU_MR_START = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    while(!PieCtrlRegs.PIEIFR11.bit.INTx2);
    PieCtrlRegs.PIEIFR11.bit.INTx2 = 0;

    *(Uint32 *)&EMIF_mem.write.duty[0] = *(Uint32 *)&Conv.duty[0];
    *(Uint32 *)&EMIF_mem.write.duty[2] = *(Uint32 *)&Conv.duty[2];

    Timer_PWM.CPU_PWM = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    CPU2toCPU1.w_filter = PLL.w_filter;
    CPU2toCPU1.f_filter = PLL.f_filter;
    CPU2toCPU1.sign = PLL.sign;
    CPU2toCPU1.PLL_RDY = PLL.RDY;
    CPU2toCPU1.sag = Conv.sag;

    if(EPwm5Regs.TZFLG.bit.OST) CPU2toCPU1.alarm_master.bit.TZ_CPU2 = 1;
    if(EPwm5Regs.TZOSTFLG.bit.OST5) CPU2toCPU1.alarm_master.bit.TZ_CLOCKFAIL_CPU2 = 1;
    if(EPwm5Regs.TZOSTFLG.bit.OST6) CPU2toCPU1.alarm_master.bit.TZ_EMUSTOP_CPU2 = 1;

//    static Uint64 benchmark_timer_HWI;
//    static volatile float benchmark_HWI;
//    benchmark_HWI = (float)(ReadIpcTimer() - benchmark_timer_HWI)*(1.0f/200000000.0f);
//    benchmark_timer_HWI = ReadIpcTimer();

    Timer_PWM.CPU_SD_END = TIMESTAMP_PWM;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
