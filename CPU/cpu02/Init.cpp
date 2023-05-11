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

void Init_class::CIC1_adaptive_filter(struct CIC1_adaptive_struct *CIC, float max_value, float OSR)
{
    CIC->range_modifier = (float)(1UL<<31) / OSR / max_value;
    CIC->div_range_modifier = 1.0f / CIC->range_modifier;
}

void Init_class::Variables()
{
    memset(&Grid, 0, sizeof(Grid));
    memset(&Grid_filter, 0, sizeof(Grid_filter));
    memset(&Meas_master, 0, sizeof(Meas_master));
    memset(&Kalman_I_grid, 0, sizeof(Kalman_I_grid));
    memset(&Kalman_U_grid, 0, sizeof(Kalman_U_grid));
    memset(&CIC1_adaptive_global__50Hz, 0, sizeof(CIC1_adaptive_global__50Hz));
    memset(&CPU2toCPU1, 0, sizeof(CPU2toCPU1));

    CLA1toCLA2.w_filter = 50.0f * MATH_2PI;

    float CT_SD_max_value[3];
    CT_SD_max_value[0] = CPU1toCPU2.CT_ratio[0] * 5.0f;
    CT_SD_max_value[1] = CPU1toCPU2.CT_ratio[1] * 5.0f;
    CT_SD_max_value[2] = CPU1toCPU2.CT_ratio[2] * 5.0f;

    float decimation = 625.0f;
    float OSR = 50.0f;

    static const float U_grid_max = 230.0f;
    static const float I_conv_max = 128.0f;
    static const float additional_range = 2.0f;

    ///////////////////////////////////////////////////////////////////
    CIC1_filter(&Grid_filter.CIC1_P_conv_1h[0], additional_range * U_grid_max * I_conv_max, OSR, decimation);
    CIC1_filter(&Grid_filter.CIC1_P_conv_1h[1], additional_range * U_grid_max * I_conv_max, OSR, decimation);
    CIC1_filter(&Grid_filter.CIC1_P_conv_1h[2], additional_range * U_grid_max * I_conv_max, OSR, decimation);
    Grid_filter.CIC1_Q_conv_1h[0] = Grid_filter.CIC1_P_conv_1h[0];
    Grid_filter.CIC1_Q_conv_1h[1] = Grid_filter.CIC1_P_conv_1h[1];
    Grid_filter.CIC1_Q_conv_1h[2] = Grid_filter.CIC1_P_conv_1h[2];

    CIC1_filter(&Grid_filter.CIC1_P_grid_1h[0], additional_range * U_grid_max * CT_SD_max_value[0], OSR, decimation);
    CIC1_filter(&Grid_filter.CIC1_P_grid_1h[1], additional_range * U_grid_max * CT_SD_max_value[1], OSR, decimation);
    CIC1_filter(&Grid_filter.CIC1_P_grid_1h[2], additional_range * U_grid_max * CT_SD_max_value[2], OSR, decimation);
    Grid_filter.CIC1_Q_grid_1h[0] = Grid_filter.CIC1_P_grid_1h[0];
    Grid_filter.CIC1_Q_grid_1h[1] = Grid_filter.CIC1_P_grid_1h[1];
    Grid_filter.CIC1_Q_grid_1h[2] = Grid_filter.CIC1_P_grid_1h[2];

    ///////////////////////////////////////////////////////////////////

    CIC1_filter(&Grid_filter.CIC1_U_grid_1h[0], additional_range * U_grid_max, OSR, decimation);
    Grid_filter.CIC1_U_grid_1h[0] =
    Grid_filter.CIC1_U_grid_1h[1] =
    Grid_filter.CIC1_U_grid_1h[2] = Grid_filter.CIC1_U_grid_1h[0];

    CIC1_filter(&Grid_filter.CIC1_I_grid_1h[0], additional_range * I_conv_max, OSR, decimation);
    Grid_filter.CIC1_I_grid_1h[0] =
    Grid_filter.CIC1_I_grid_1h[1] =
    Grid_filter.CIC1_I_grid_1h[2] = Grid_filter.CIC1_I_grid_1h[0];

    ///////////////////////////////////////////////////////////////////

    CIC1_filter(&Grid_filter.CIC1_U_grid[0], additional_range * U_grid_max, OSR, decimation);
    Grid_filter.CIC1_U_grid[0] =
    Grid_filter.CIC1_U_grid[1] =
    Grid_filter.CIC1_U_grid[2] = Grid_filter.CIC1_U_grid[0];

    CIC1_filter(&Grid_filter.CIC1_I_conv[0], additional_range * I_conv_max, OSR, decimation);
    Grid_filter.CIC1_I_conv[0] =
    Grid_filter.CIC1_I_conv[1] =
    Grid_filter.CIC1_I_conv[2] =
    Grid_filter.CIC1_I_conv[3] = Grid_filter.CIC1_I_conv[0];

    CIC1_filter(&Grid_filter.CIC1_I_grid[0], additional_range * CT_SD_max_value[0], OSR, decimation);
    CIC1_filter(&Grid_filter.CIC1_I_grid[1], additional_range * CT_SD_max_value[1], OSR, decimation);
    CIC1_filter(&Grid_filter.CIC1_I_grid[2], additional_range * CT_SD_max_value[2], OSR, decimation);

    ///////////////////////////////////////////////////////////////////

    CIC1_filter(&Grid_filter.CIC1_THD_U_grid[0], 1000.0f, OSR, decimation);
    Grid_filter.CIC1_THD_U_grid[0] =
    Grid_filter.CIC1_THD_U_grid[1] =
    Grid_filter.CIC1_THD_U_grid[2] =
    Grid_filter.CIC1_THD_I_grid[0] =
    Grid_filter.CIC1_THD_I_grid[1] =
    Grid_filter.CIC1_THD_I_grid[2] = Grid_filter.CIC1_THD_U_grid[0];

    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    float OSR2 = 625.0f;
    CIC1_adaptive_filter(&Grid.CIC1_P_conv_1h[0], additional_range * U_grid_max * I_conv_max, OSR2);
    CIC1_adaptive_filter(&Grid.CIC1_P_conv_1h[1], additional_range * U_grid_max * I_conv_max, OSR2);
    CIC1_adaptive_filter(&Grid.CIC1_P_conv_1h[2], additional_range * U_grid_max * I_conv_max, OSR2);
    Grid.CIC1_Q_conv_1h[0] = Grid.CIC1_P_conv_1h[0];
    Grid.CIC1_Q_conv_1h[1] = Grid.CIC1_P_conv_1h[1];
    Grid.CIC1_Q_conv_1h[2] = Grid.CIC1_P_conv_1h[2];

    CIC1_adaptive_filter(&Grid.CIC1_P_grid_1h[0], additional_range * U_grid_max * CT_SD_max_value[0], OSR2);
    CIC1_adaptive_filter(&Grid.CIC1_P_grid_1h[1], additional_range * U_grid_max * CT_SD_max_value[1], OSR2);
    CIC1_adaptive_filter(&Grid.CIC1_P_grid_1h[2], additional_range * U_grid_max * CT_SD_max_value[2], OSR2);
    Grid.CIC1_Q_grid_1h[0] = Grid.CIC1_P_grid_1h[0];
    Grid.CIC1_Q_grid_1h[1] = Grid.CIC1_P_grid_1h[1];
    Grid.CIC1_Q_grid_1h[2] = Grid.CIC1_P_grid_1h[2];

    ///////////////////////////////////////////////////////////////////

    CIC1_adaptive_filter(&Grid.CIC1_U_grid_1h[0], additional_range * U_grid_max, OSR2);
    Grid.CIC1_U_grid_1h[0] =
    Grid.CIC1_U_grid_1h[1] =
    Grid.CIC1_U_grid_1h[2] = Grid.CIC1_U_grid_1h[0];

    CIC1_adaptive_filter(&Grid.CIC1_I_grid_1h[0], additional_range * I_conv_max, OSR2);
    Grid.CIC1_I_grid_1h[0] =
    Grid.CIC1_I_grid_1h[1] =
    Grid.CIC1_I_grid_1h[2] = Grid.CIC1_I_grid_1h[0];

    ///////////////////////////////////////////////////////////////////

    CIC1_adaptive_filter(&Grid.CIC1_U_grid[0], powf(additional_range * U_grid_max, 2.0f), OSR2);
    Grid.CIC1_U_grid[0] =
    Grid.CIC1_U_grid[1] =
    Grid.CIC1_U_grid[2] = Grid.CIC1_U_grid[0];

    CIC1_adaptive_filter(&Grid.CIC1_I_conv[0], powf(additional_range * I_conv_max, 2.0f), OSR2);
    Grid.CIC1_I_conv[0] =
    Grid.CIC1_I_conv[1] =
    Grid.CIC1_I_conv[2] =
    Grid.CIC1_I_conv[3] = Grid.CIC1_I_conv[0];

    CIC1_adaptive_filter(&Grid.CIC1_I_grid[0], powf(additional_range * CT_SD_max_value[0], 2.0f), OSR2);
    CIC1_adaptive_filter(&Grid.CIC1_I_grid[1], powf(additional_range * CT_SD_max_value[1], 2.0f), OSR2);
    CIC1_adaptive_filter(&Grid.CIC1_I_grid[2], powf(additional_range * CT_SD_max_value[2], 2.0f), OSR2);

    ///////////////////////////////////////////////////////////////////

    Grid.Resonant_U_grid[0].trigonometric.ptr =
    Grid.Resonant_U_grid[1].trigonometric.ptr =
    Grid.Resonant_U_grid[2].trigonometric.ptr =
    Grid.Resonant_I_grid[0].trigonometric.ptr =
    Grid.Resonant_I_grid[1].trigonometric.ptr =
    Grid.Resonant_I_grid[2].trigonometric.ptr =
    Grid.Resonant_I_conv[0].trigonometric.ptr =
    Grid.Resonant_I_conv[1].trigonometric.ptr =
    Grid.Resonant_I_conv[2].trigonometric.ptr = &sincos_table[0];

    Grid.Resonant_U_grid[0].trigonometric_comp.ptr =
    Grid.Resonant_U_grid[1].trigonometric_comp.ptr =
    Grid.Resonant_U_grid[2].trigonometric_comp.ptr =
    Grid.Resonant_I_conv[0].trigonometric_comp.ptr =
    Grid.Resonant_I_conv[1].trigonometric_comp.ptr =
    Grid.Resonant_I_conv[2].trigonometric_comp.ptr = &Grid.zero_rot;

    Grid.Ts = 32e-6;
    register float rotation = 0.0f * MATH_2PI * 50.0f * Grid.Ts;
    Grid.zero_rot.sine = sinf(rotation);
    Grid.zero_rot.cosine = cosf(rotation);

    Grid.I_grid_rot[0].sine =
    Grid.I_grid_rot[1].sine =
    Grid.I_grid_rot[2].sine = 0.0f;
    Grid.I_grid_rot[0].cosine =
    Grid.I_grid_rot[1].cosine =
    Grid.I_grid_rot[2].cosine = 1.0f;

    Grid.Resonant_I_grid[0].trigonometric_comp.ptr = &Grid.I_grid_rot[0];
    Grid.Resonant_I_grid[1].trigonometric_comp.ptr = &Grid.I_grid_rot[1];
    Grid.Resonant_I_grid[2].trigonometric_comp.ptr = &Grid.I_grid_rot[2];

    Grid.Resonant_U_grid[0].gain =
    Grid.Resonant_U_grid[1].gain =
    Grid.Resonant_U_grid[2].gain =
    Grid.Resonant_I_grid[0].gain =
    Grid.Resonant_I_grid[1].gain =
    Grid.Resonant_I_grid[2].gain =
    Grid.Resonant_I_conv[0].gain =
    Grid.Resonant_I_conv[1].gain =
    Grid.Resonant_I_conv[2].gain = 2.0f / (MATH_2PI * 50.0f) / (MATH_1_E * 0.02f);

    Grid.Accumulator_gain = ((float)0x80000000 * 2.0f / 3600.0f) * Grid.Ts;

    ///////////////////////////////////////////////////////////////////

    Kalman_U_grid[0].gain.ptr =
    Kalman_U_grid[1].gain.ptr =
    Kalman_U_grid[2].gain.ptr =
    Kalman_I_grid[0].gain.ptr =
    Kalman_I_grid[1].gain.ptr =
    Kalman_I_grid[2].gain.ptr = (float *)&Kalman_gain;

    ///////////////////////////////////////////////////////////////////

    CIC1_adaptive_global__50Hz.Ts = Grid.Ts;

    SINCOS_kalman_calc_CPUasm(sincos_kalman_table, CLA1toCLA2.w_filter * Grid.Ts);
    SINCOS_calc_CPUasm(sincos_table, CLA1toCLA2.w_filter * Grid.Ts);
}

void Init_class::PWM_timestamp(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;

    EPwmReg->TBPRD = 3199;                   // PWM frequency = 1/(TBPRD+1)
    EPwmReg->CMPA.bit.CMPA = 20;
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

    EDIS;
}

void Init_class::PWMs()
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;
    EDIS;

    PWM_timestamp(&EPwm10Regs);
}
