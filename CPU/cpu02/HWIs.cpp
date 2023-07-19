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
    Conv.MR_ref.a = (float)EMIF_mem.read.Resonant[0].series[0].SUM * modifier1;
    Conv.MR_ref.b = (float)EMIF_mem.read.Resonant[1].series[0].SUM * modifier1;
    Conv.MR_ref.c = (float)EMIF_mem.read.Resonant[2].series[0].SUM * modifier1;
    Conv.MR_ref.a += (float)EMIF_mem.read.Resonant[3].series[0].SUM * modifier1;
    Conv.MR_ref.b += (float)EMIF_mem.read.Resonant[4].series[0].SUM * modifier1;
    Conv.MR_ref.c += (float)EMIF_mem.read.Resonant[5].series[0].SUM * modifier1;

    modifier1 = Conv.div_range_modifier_Kalman_values;
    Conv.Kalman_U_grid.a = (float)EMIF_mem.read.Kalman.series[0].estimate * modifier1;
    Conv.Kalman_U_grid.b = (float)EMIF_mem.read.Kalman.series[1].estimate * modifier1;
    Conv.Kalman_U_grid.c = (float)EMIF_mem.read.Kalman.series[2].estimate * modifier1;

    Conv.cycle_period = EMIF_mem.read.cycle_period;
    Conv.Kp_I = CPU1toCPU2.CLA1toCLA2.Kp_I;
    Conv.L_conv = CPU1toCPU2.CLA1toCLA2.L_conv;
    Conv.enable = CPU1toCPU2.CLA1toCLA2.enable;
    Conv.select_modulation = CPU1toCPU2.CLA1toCLA2.select_modulation;

    Timer_PWM.CPU_MEAS = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    Timer_PWM.CPU_IERR = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    Conv.zero_error = 1.0f;
    if (fmaxf(fmaxf(fabsf(Conv.duty[0]), fabsf(Conv.duty[1])), fabsf(Conv.duty[2])) > Conv.cycle_period)
        Conv.zero_error = 0;

    if(!Conv.enable)
    {
        Conv.zero_error = 1.0f;
        Conv.I_err.a = Meas_ACDC.U_grid.a - Conv.MR_ref.a;
        Conv.I_err.b = Meas_ACDC.U_grid.b - Conv.MR_ref.b;
        Conv.I_err.c = Meas_ACDC.U_grid.c - Conv.MR_ref.c;
    }

    register float modifier2 = Conv.range_modifier_Resonant_values;
    CPU2toCPU1.Resonant_error[0] = Conv.I_err.a * modifier2;
    CPU2toCPU1.Resonant_error[2] = Conv.I_err.b * modifier2;
    CPU2toCPU1.Resonant_error[4] = Conv.I_err.c * modifier2;
    CPU2toCPU1.Resonant_error[1] = Meas_ACDC.I_grid.a * modifier2;
    CPU2toCPU1.Resonant_error[3] = Meas_ACDC.I_grid.b * modifier2;
    CPU2toCPU1.Resonant_error[5] = Meas_ACDC.I_grid.c * modifier2;
    CPU2toCPU1.ZR = Conv.zero_error * Conv.range_modifier_Resonant_coefficients;

    static volatile Uint32 Resonant_WIP;
    Resonant_WIP = EMIF_mem.read.flags.Resonant1_WIP;

    GPIO_SET(CPU2_DMA_CM);

    Timer_PWM.CPU_MR_START = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    while(!PieCtrlRegs.PIEIFR11.bit.INTx2);
    PieCtrlRegs.PIEIFR11.bit.INTx2 = 0;

    *(Uint32 *)&CPU2toCPU1.duty[0] = *(Uint32 *)&Conv.duty[0];
    *(Uint32 *)&CPU2toCPU1.duty[2] = *(Uint32 *)&Conv.duty[2];

    GPIO_CLEAR(CPU2_DMA_CM);

    Timer_PWM.CPU_PWM = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    CPU2toCPU1.Resonant_start = 0b111111;
    CPU2toCPU1.w_filter = PLL.w_filter;
    CPU2toCPU1.f_filter = PLL.f_filter;
    CPU2toCPU1.sign = PLL.sign;
    CPU2toCPU1.PLL_RDY = PLL.RDY;
    CPU2toCPU1.sag = Conv.sag;

    if(EPwm5Regs.TZFLG.bit.OST) CPU2toCPU1.alarm_master.bit.TZ_CPU2 = 1;
    if(EPwm5Regs.TZOSTFLG.bit.OST5) CPU2toCPU1.alarm_master.bit.TZ_CLOCKFAIL_CPU2 = 1;
    if(EPwm5Regs.TZOSTFLG.bit.OST6) CPU2toCPU1.alarm_master.bit.TZ_EMUSTOP_CPU2 = 1;

    Timer_PWM.CPU_SD_END = TIMESTAMP_PWM;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}
