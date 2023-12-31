/*
 * Hardware.c
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <math.h>
#include <string.h>

#include "HWIs.h"
#include "stdafx.h"
#include "Init.h"

class Init_class Init;

void Init_class::CLA()
{
    EALLOW;

    CpuSysRegs.PCLKCR0.bit.CLA1 = 1;

    //
    // Initialize and wait for CLA1ToCPUMsgRAM
    //
    MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};

    //
    // Initialize and wait for CPUToCLA1MsgRAM
    //
    MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};

    //
    // Select LS4RAM and LS5RAM to be the programming space for the CLA
    // First configure the CLA to be the master for LS4 and LS5 and then
    // set the space to be a program block
    //

    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 1;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 1;

    //
    // Next configure LS0RAM and LS1RAM as data spaces for the CLA
    // First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    //

    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 0;
    EDIS;


    EALLOW;
    Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);

    Cla1Regs.MVECT2 =
    Cla1Regs.MVECT3 =
    Cla1Regs.MVECT4 =
    Cla1Regs.MVECT5 =
    Cla1Regs.MVECT6 =
    Cla1Regs.MVECT7 =
    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task1);

//    Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
//    Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
//    Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
//    Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
//    Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
//    Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
//    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 0;  //soft trigger

    Cla1Regs.MCTL.bit.IACKE = 1;

    Cla1Regs.MIER.bit.INT1 = 0;
    Cla1Regs.MIER.bit.INT2 = 0;
    Cla1Regs.MIER.bit.INT3 = 0;
    Cla1Regs.MIER.bit.INT4 = 0;
    Cla1Regs.MIER.bit.INT5 = 0;
    Cla1Regs.MIER.bit.INT6 = 0;
    Cla1Regs.MIER.bit.INT7 = 0;
    Cla1Regs.MIER.bit.INT8 = 0;
    EDIS;
}

void Init_class::CIC1_filter(struct CIC1_struct *CIC, float max_value, float OSR, float decimation_ratio)
{
    CIC->decimation_ratio = decimation_ratio;
    CIC->OSR = OSR;
    CIC->div_OSR = 1.0f / OSR;
    CIC->range_modifier = (float)(1UL<<31) / OSR / max_value;
    CIC->div_range_modifier = 1.0f / CIC->range_modifier;
}

void Init_class::CIC2_filter(struct CIC2_struct *CIC, float max_value, Uint16 OSR, Uint16 decimation_ratio)
{
    CIC->decimation_ratio = decimation_ratio;
    CIC->OSR = OSR;
    CIC->div_OSR = 1.0f / (float)OSR;
    CIC->range_modifier = (float)(1UL << 31) / (float)OSR / (float)OSR / max_value;
    CIC->div_range_modifier = 1.0f / CIC->range_modifier;
}

void Init_class::CIC1_adaptive_filter(struct CIC1_adaptive_struct *CIC, float max_value, float OSR)
{
    CIC->range_modifier = (float)(1UL<<31) / OSR / max_value;
    CIC->div_range_modifier = 1.0f / CIC->range_modifier;
}

void Init_class::Variables()
{
    memset(&Meas_ACDC, 0, sizeof(Meas_ACDC));
    memset(&Conv, 0, sizeof(Conv));
    memset(&PLL, 0, sizeof(PLL));
    memset(&CPU2toCPU1, 0, sizeof(CPU2toCPU1));

    Conv.Ts = CPU1toCPU2.CLA1toCLA2.Ts;
    Conv.range_modifier_Resonant_values = CPU1toCPU2.CLA1toCLA2.range_modifier_Resonant_values;
    Conv.div_range_modifier_Resonant_values = 1.0f / Conv.range_modifier_Resonant_values;
    Conv.range_modifier_Kalman_values = CPU1toCPU2.CLA1toCLA2.range_modifier_Kalman_values;
    Conv.div_range_modifier_Kalman_values = 1.0f / Conv.range_modifier_Kalman_values;
    Conv.range_modifier_Resonant_coefficients = CPU1toCPU2.CLA1toCLA2.range_modifier_Resonant_coefficients;
    Conv.div_range_modifier_Resonant_coefficients = 1.0f / Conv.range_modifier_Resonant_coefficients;
    Conv.range_modifier_Kalman_coefficients = CPU1toCPU2.CLA1toCLA2.range_modifier_Kalman_coefficients;
    Conv.div_range_modifier_Kalman_coefficients = 1.0f / Conv.range_modifier_Kalman_coefficients;

    ///////////////////////////////////////////////////////////////////

    PLL.w_filter = 50.0f * MATH_2PI;
    PLL.Ts = Conv.Ts;
    PLL.PI.Kp = 92.0f;
    PLL.PI.Ts_Ti = PLL.Ts / 0.087f;
    PLL.PI.lim_H = 400.0f;
    PLL.PI.lim_L = -400.0f;

    float full_OSR_PLL = (Uint16)(0.02f / Conv.Ts + 0.5f);
    float decimation_PLL = 1.0f;
    while (full_OSR_PLL / decimation_PLL > 160.0f) decimation_PLL++;
    Uint16 OSR_PLL = full_OSR_PLL / decimation_PLL + 0.5f;
    CIC2_filter(&PLL.CIC_w, 410.0f, OSR_PLL, decimation_PLL);

    PLL.state = PLL_omega_init;
    PLL.state_last = PLL_active;

    ///////////////////////////////////////////////////////////////////

    Conv.cycle_period = EMIF_mem.read.cycle_period;
    Conv.Kp_I = CPU1toCPU2.CLA1toCLA2.Kp_I;
    Conv.L_conv = CPU1toCPU2.CLA1toCLA2.L_conv;
}

void Init_class::PWM_TZ_timestamp(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;

    EPwmReg->TBPRD = (float)EMIF_mem.read.cycle_period * 0.8f - 1.0f;//1599;                   // PWM frequency = 1/(TBPRD+1)
    EPwmReg->TBCTR = 0;                     //clear counter
    EPwmReg->TBPHS.all = 0;

    EPwmReg->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwmReg->TBCTL.bit.PHSEN = TB_ENABLE;
    EPwmReg->TBCTL.bit.PHSDIR = TB_UP;

//Configure modes, clock dividers and action qualifier
    EPwmReg->TBCTL.bit.CTRMODE = TB_COUNT_UP;         // Select up-down count mode
    EPwmReg->TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwmReg->TBCTL.bit.CLKDIV = TB_DIV1;                  // TBCLK = SYSCLKOUT
    EPwmReg->TBCTL.bit.FREE_SOFT = 2;
    EPwmReg->TBCTL.bit.PRDLD = TB_SHADOW;                 // set Shadow load

    EPwmReg->AQCTLA.bit.ZRO = AQ_SET;
    EPwmReg->AQCTLA.bit.PRD = AQ_SET;

    //Configure trip-zone
    EPwmReg->TZCTL.bit.TZA = TZ_FORCE_LO;

//    EPwmReg->TZSEL.bit.OSHT1 = 1;
//    EPwmReg->TZSEL.bit.OSHT3 = 1;
    EPwmReg->TZSEL.bit.OSHT5 = 1;
    EPwmReg->TZSEL.bit.OSHT6 = 1;
    EDIS;
}

void Init_class::EPwm_TZclear(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;
    EPwmReg->TZOSTCLR.all = 0xFF;
    EPwmReg->TZCLR.bit.OST = 1;
    EDIS;
}

void Init_class::PWMs()
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 1;
    EDIS;

    PWM_TZ_timestamp(&EPwm5Regs);
}
