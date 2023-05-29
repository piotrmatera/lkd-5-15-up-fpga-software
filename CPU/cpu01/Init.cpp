/*
 * Hardware.c
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <string.h>
#include <math.h>
#include "HWIs.h"
#include "stdafx.h"
#include "F2837xD_Ipc_drivers.h"
#include "Init.h"
#include "State.h"
#include "SD_card.h"
#include "version.h"

class Init_class Init;

struct FAN_struct
{
    float on_temp;
    float full_temp;
    float on_duty;
    float slope;
}FAN;

void Init_class::IPCBootCPU2_flash()
{
    if(( (IpcRegs.IPCBOOTSTS & 0x0000000F) == C2_BOOTROM_BOOTSTS_SYSTEM_READY)
        && ((IpcRegs.IPCFLG.all & (IPC_FLAG0 | IPC_FLAG31)) == 0))
    {
        //
        //CPU01 to CPU02 IPC Boot Mode Register
        //
        IpcRegs.IPCBOOTMODE = C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH;
        //
        // CPU01 To CPU02 IPC Command Register
        //
        IpcRegs.IPCSENDCOM  = BROM_IPC_EXECUTE_BOOTMODE_CMD;
        //
        // CPU01 to CPU02 IPC flag register
        //
        IpcRegs.IPCSET.all = (IPC_FLAG0 | IPC_FLAG31);
    }
    else
    {
        IpcRegs.IPCCLR.all = (IPC_FLAG0 | IPC_FLAG31);
    }
}

void Init_class::clear_alarms()
{
    GPIO_SET(RST_CM);

    EMIF_mem.write.PWM_control = 0xFF00;

    DELAY_US(10000);

    EMIF_mem.write.PWM_control = 0x0000;

    DELAY_US(10000);

    GPIO_CLEAR(RST_CM);

    CPU1toCPU2.clear_alarms = 1.0f;
    Init.EPwm_TZclear(&EPwm4Regs);

    DELAY_US(1000);

    CPU1toCPU2.clear_alarms = 0.0f;

    DELAY_US(1000);

    if(alarm_master.bit.Not_enough_data_master)
    {
        alarm_master_snapshot.all[0] =
        alarm_master_snapshot.all[1] =
        alarm_master_snapshot.all[2] =
        alarm_master.all[0] =
        alarm_master.all[1] =
        alarm_master.all[2] = 0;
        alarm_master.bit.Not_enough_data_master = 1;
    }
    else
    {
        alarm_master_snapshot.all[0] =
        alarm_master_snapshot.all[1] =
        alarm_master_snapshot.all[2] =
        alarm_master.all[0] =
        alarm_master.all[1] =
        alarm_master.all[2] = 0;
    }
}

void Init_class::ADC()
{
    Uint16 acqps = 100;//14;

    EALLOW;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 1;

    //
    // Check if device is trimmed
    //
    if(*((Uint16 *)0x5D1B6) == 0x0000){
        //
        // Device is not trimmed--apply static calibration values
        //
        AnalogSubsysRegs.ANAREFTRIMA.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMB.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMC.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMD.all = 31709;
    }

    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    EDIS;

    EALLOW;
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
//    (*Device_cal)();
    EDIS;

    EALLOW;
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    EDIS;
//
//delay for 1ms to allow ADC time to power up
//
    DELAY_US(1000);

    EALLOW;

    AdcdRegs.ADCSOC0CTL.bit.CHSEL  = 0;
    AdcdRegs.ADCSOC0CTL.bit.ACQPS  = acqps;
    AdcdRegs.ADCSOC0CTL.bit.TRIGSEL  = 11;   //ePWM4 SOCA
    AdcdRegs.ADCSOC1CTL.all = AdcdRegs.ADCSOC0CTL.all;

    AdcdRegs.ADCSOC2CTL.bit.CHSEL  = 1;
    AdcdRegs.ADCSOC2CTL.bit.ACQPS  = acqps;
    AdcdRegs.ADCSOC2CTL.bit.TRIGSEL  = 11;   //ePWM4 SOCA
    AdcdRegs.ADCSOC3CTL.all = AdcdRegs.ADCSOC2CTL.all;

    AdcdRegs.ADCSOC4CTL.bit.CHSEL  = 2;
    AdcdRegs.ADCSOC4CTL.bit.ACQPS  = acqps;
    AdcdRegs.ADCSOC4CTL.bit.TRIGSEL  = 11;   //ePWM4 SOCA
    AdcdRegs.ADCSOC5CTL.all = AdcdRegs.ADCSOC4CTL.all;

    AdcdRegs.ADCSOC6CTL.bit.CHSEL  = 3;
    AdcdRegs.ADCSOC6CTL.bit.ACQPS  = acqps;
    AdcdRegs.ADCSOC6CTL.bit.TRIGSEL  = 11;   //ePWM4 SOCA
    AdcdRegs.ADCSOC7CTL.all = AdcdRegs.ADCSOC6CTL.all;

    AdcdRegs.ADCSOC8CTL.bit.CHSEL  = 4;
    AdcdRegs.ADCSOC8CTL.bit.ACQPS  = acqps;
    AdcdRegs.ADCSOC8CTL.bit.TRIGSEL  = 11;   //ePWM4 SOCA
    AdcdRegs.ADCSOC9CTL.all = AdcdRegs.ADCSOC8CTL.all;

    EDIS;
}

void Init_class::CPUS()
{
    EALLOW;

    DevCfgRegs.CPUSEL0.bit.EPWM5 = 1;

    MemCfgRegs.GSxMSEL.bit.MSEL_GS0  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS1  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS2  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS3  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS4  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS5  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS6  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS7  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS8  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS9  = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS10 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS11 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS12 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS13 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS14 = 0;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS15 = 0;

   EDIS;
}

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

    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 1;

    //
    // Next configure LS0RAM and LS1RAM as data spaces for the CLA
    // First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    //
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
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK2 = 0;  //soft trigger

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

void Init_class::CIC1_filter(struct CIC1_struct *CIC, float max_value, Uint16 OSR, Uint16 decimation_ratio)
{
    CIC->decimation_ratio = decimation_ratio;
    CIC->OSR = OSR;
    CIC->div_OSR = 1.0f / (float)OSR;
    CIC->range_modifier = (float)(1UL << 31) / (float)OSR / max_value;
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

void Init_class::CIC1_adaptive_filter(struct CIC1_adaptive_struct *CIC, float max_value, Uint16 OSR)
{
    CIC->range_modifier = (float)(1UL<<31) / (float)OSR / max_value;
    CIC->div_range_modifier = 1.0f / CIC->range_modifier;
}

void Init_class::Variables()
{
    Conv.Ts_rate = EMIF_mem.read.control_rate;
    Conv.Ts = Conv.Ts_rate * (float)EMIF_mem.read.cycle_period * 8e-9;
    Conv.w_filter = MATH_2PI * 50.0f;
    Conv.f_filter = 50.0f;
    float full_OSR = (Uint16)(1.0f / Conv.f_filter / Conv.Ts + 0.5f);

    Meas_alarm_L.U_grid_rms = 5.0f;
    Meas_alarm_H.U_grid_abs = 380.0f;

    Meas_alarm_H.Temp = 95.0f;
    Meas_alarm_L.Temp = 0.0f;

    Meas_alarm_H.U_dc = 760.0f;
    Meas_alarm_L.U_dc = -5.0f;
    Meas_alarm_H.U_dc_balance = 30.0f;

    Meas_alarm_H.I_conv_rms = Conv.I_lim_nominal * 1.1f;
    float min_Current = fabsf(Meas_master_gain.def_osr * Meas_master_gain.def_osr / Meas_master_gain.sd_shift * fminf(Meas_master_gain.I_conv.a, fminf(Meas_master_gain.I_conv.b, fminf(Meas_master_gain.I_conv.c, Meas_master_gain.I_conv.n))));
    Meas_alarm_H.I_conv = fminf(min_Current - 10.0f, Conv.I_lim_nominal * 2.0f);
    Meas_alarm_L.I_conv = -Meas_alarm_H.I_conv;

    ///////////////////////////////////////////////////////////////////

    float decimation = 1.0f;
    while (full_OSR / decimation > 100.0f) decimation++;
    Uint16 OSR_calib = full_OSR / decimation + 0.5f;
    CIC2_filter(&CIC2_calibration, 35.0f, OSR_calib, decimation);
    CIC2_calibration_input.ptr = &Meas_master.I_grid.b;

    CIC1_adaptive_global__50Hz.Ts = Conv.Ts;

    ///////////////////////////////////////////////////////////////////

    FAN.on_duty = 0.5f;
    FAN.on_temp = Meas_alarm_H.Temp - 35.0f;
    FAN.full_temp = Meas_alarm_H.Temp - 5.0f;
    FAN.slope = (1.0f - FAN.on_duty)/(FAN.full_temp - FAN.on_temp);

    ///////////////////////////////////////////////////////////////////

    Therm.Divider_supply = 3.3f;
    Therm.R_divider = 4700.0f;
    Therm.B = 3984.0f;
    Therm.T_0 = 273.15f;
    Therm.R25 = 4700.0f;
    Therm.DIV_Rinf = expf(Therm.B/(Therm.T_0+25.0f))/Therm.R25;

    ///////////////////////////////////////////////////////////////////

    Conv.P_conv_filter.Ts_Ti = Conv.Ts / 0.1f;

    Conv.version_Q_comp_local_prefilter.a.Ts_Ti =
    Conv.version_Q_comp_local_prefilter.b.Ts_Ti =
    Conv.version_Q_comp_local_prefilter.c.Ts_Ti =
    Conv.enable_Q_comp_local_prefilter.a.Ts_Ti =
    Conv.enable_Q_comp_local_prefilter.b.Ts_Ti =
    Conv.enable_Q_comp_local_prefilter.c.Ts_Ti =
    Conv.Q_set_local_prefilter.a.Ts_Ti =
    Conv.Q_set_local_prefilter.b.Ts_Ti =
    Conv.Q_set_local_prefilter.c.Ts_Ti =
    Conv.enable_P_sym_local_prefilter.Ts_Ti =
    Conv.version_P_sym_local_prefilter.Ts_Ti =
    Conv.tangens_range_local_prefilter[0].a.Ts_Ti =
    Conv.tangens_range_local_prefilter[0].b.Ts_Ti =
    Conv.tangens_range_local_prefilter[0].c.Ts_Ti =
    Conv.tangens_range_local_prefilter[1].a.Ts_Ti =
    Conv.tangens_range_local_prefilter[1].b.Ts_Ti =
    Conv.tangens_range_local_prefilter[1].c.Ts_Ti = Conv.Ts / 0.02f;

    ///////////////////////////////////////////////////////////////////

    Conv.I_lim = Conv.I_lim_nominal;
    Conv.U_dc_ref = 675.0f;

    CIC1_adaptive_filter(&Conv.CIC1_U_dc, 1000.0f, full_OSR);

    register float alfa2 = 2.0f * MATH_SQRT2;
    register float STC2 = 0.005f;//0.63212 * 0.02f;
    float kp_dc = Conv.C_dc / (alfa2 * STC2);
    float ti_dc = alfa2 * alfa2 * STC2;

    Conv.PI_U_dc.Ts_Ti = Conv.Ts / ti_dc;
    Conv.PI_U_dc.Kp = kp_dc;
    Conv.PI_U_dc.lim_H = Conv.I_lim * 0.2f;
    Conv.PI_U_dc.lim_L = -Conv.I_lim * 0.2f;

    ///////////////////////////////////////////////////////////////////

    Conv.PI_Iq[0].Ts_Ti = Conv.Ts / 0.011f;
    Conv.PI_Iq[0].Kp = 0.5f;
    Conv.PI_Iq[0].lim_H = Conv.I_lim;
    Conv.PI_Iq[0].lim_L = -Conv.I_lim;

    Conv.PI_Iq[1] =
    Conv.PI_Iq[2] = Conv.PI_Iq[0];

    Conv.PI_Id[0].Ts_Ti = Conv.PI_Iq[0].Ts_Ti;
    Conv.PI_Id[0].Kp = Conv.PI_Iq[0].Kp;
    Conv.PI_Id[0].lim_H = Conv.I_lim;
    Conv.PI_Id[0].lim_L = -Conv.I_lim;

    Conv.PI_Id[1] =
    Conv.PI_Id[2] = Conv.PI_Id[0];

    ///////////////////////////////////////////////////////////////////

    Conv.PI_I_harm_ratio[0].Ts_Ti = Conv.Ts / 0.1f;
    Conv.PI_I_harm_ratio[0].Kp = 1.0f;
    Conv.PI_I_harm_ratio[0].lim_H = 1.0f;
    Conv.PI_I_harm_ratio[0].lim_L = 0.0f;

    Conv.PI_I_harm_ratio[1] =
    Conv.PI_I_harm_ratio[2] =
    Conv.PI_I_harm_ratio[3] = Conv.PI_I_harm_ratio[0];

    ///////////////////////////////////////////////////////////////////

    Conv.compensation2 = 2.0f;

    SINCOS_calc_CPUasm(sincos_table, Conv.w_filter * Conv.Ts / Conv.Ts_rate);
    SINCOS_calc_CPUasm(sincos_table_comp, Conv.w_filter * Conv.Ts / Conv.Ts_rate * Conv.compensation2);
    SINCOS_calc_CPUasm(sincos_table_Kalman, Conv.w_filter * Conv.Ts);

    register float p_pr_i = Conv.L_conv / (3.0f * Conv.Ts);
    register float r_pr_i = Conv.L_conv * MATH_PI / Conv.Ts;
    r_pr_i /= MATH_2PI * 50.0f;

    Conv.Kp_I = p_pr_i;
    Conv.Kr_I = r_pr_i;

    Conv.range_modifier_Resonant_coefficients = 1UL << 30;
    Conv.div_range_modifier_Resonant_coefficients = 1.0f / Conv.range_modifier_Resonant_coefficients;
    Conv.range_modifier_Resonant_values = 1UL << 21;
    Conv.div_range_modifier_Resonant_values = 1.0f / Conv.range_modifier_Resonant_values;

    for(Uint16 i = 0; i < FPGA_RESONANT_STATES; i++)
    {
        register float modifier = Conv.range_modifier_Resonant_coefficients;
        EMIF_mem.write.Resonant[1].harmonic[i].cos_A =
        EMIF_mem.write.Resonant[0].harmonic[i].cos_A = modifier * sincos_table[2 * i].cosine;
        EMIF_mem.write.Resonant[1].harmonic[i].sin_A =
        EMIF_mem.write.Resonant[0].harmonic[i].sin_A = modifier * sincos_table[2 * i].sine;
        EMIF_mem.write.Resonant[1].harmonic[i].cos_B =
        EMIF_mem.write.Resonant[0].harmonic[i].cos_B = modifier * (sincos_table[2 * i].cosine - 1.0f) / (float)(2 * i + 1) * Conv.Kr_I;
        EMIF_mem.write.Resonant[1].harmonic[i].sin_B =
        EMIF_mem.write.Resonant[0].harmonic[i].sin_B = modifier * sincos_table[2 * i].sine / (float)(2 * i + 1) * Conv.Kr_I;
        EMIF_mem.write.Resonant[1].harmonic[i].cos_C =
        EMIF_mem.write.Resonant[0].harmonic[i].cos_C = modifier * sincos_table_comp[2 * i].cosine;
        EMIF_mem.write.Resonant[1].harmonic[i].sin_C =
        EMIF_mem.write.Resonant[0].harmonic[i].sin_C = modifier * sincos_table_comp[2 * i].sine;
    }

    Conv.range_modifier_Kalman_coefficients = 1UL << 31;
    Conv.div_range_modifier_Kalman_coefficients = 1.0f / Conv.range_modifier_Kalman_coefficients;
    Conv.range_modifier_Kalman_values = 1UL << 21;
    Conv.div_range_modifier_Kalman_values = 1.0f / Conv.range_modifier_Kalman_values;

    EMIF_mem.write.Kalman[0].harmonic[0].K1 = Kalman_gain[0] * Conv.range_modifier_Kalman_coefficients;
    EMIF_mem.write.Kalman[0].harmonic[0].K2 = Kalman_gain[1] * Conv.range_modifier_Kalman_coefficients;
    EMIF_mem.write.Kalman[1].harmonic[0].K1 = Kalman_gain[0] * Conv.range_modifier_Kalman_coefficients;
    EMIF_mem.write.Kalman[1].harmonic[0].K2 = Kalman_gain[1] * Conv.range_modifier_Kalman_coefficients;
    for(Uint16 i = 1; i < FPGA_KALMAN_STATES; i++)
    {
        register float modifier = Conv.range_modifier_Kalman_coefficients;
        EMIF_mem.write.Kalman[1].harmonic[i].cos_K =
        EMIF_mem.write.Kalman[0].harmonic[i].cos_K = sincos_table_Kalman[2 * (i - 1)].cosine * modifier;
        EMIF_mem.write.Kalman[1].harmonic[i].sin_K =
        EMIF_mem.write.Kalman[0].harmonic[i].sin_K = sincos_table_Kalman[2 * (i - 1)].sine * modifier;
        EMIF_mem.write.Kalman[1].harmonic[i].K1 =
        EMIF_mem.write.Kalman[0].harmonic[i].K1 = Kalman_gain[2 * i] * modifier;
        EMIF_mem.write.Kalman[1].harmonic[i].K2 =
        EMIF_mem.write.Kalman[0].harmonic[i].K2 = Kalman_gain[2 * i + 1] * modifier;
    }

    EMIF_mem.write.Kalman_DC.harmonic[0].K1 = Kalman_gain_dc[0] * Conv.range_modifier_Kalman_coefficients;
    EMIF_mem.write.Kalman_DC.harmonic[0].K2 = Kalman_gain_dc[1] * Conv.range_modifier_Kalman_coefficients;
    for(Uint16 i = 1; i < FPGA_KALMAN_DC_STATES; i++)
    {
        register float modifier = Conv.range_modifier_Kalman_coefficients;
        EMIF_mem.write.Kalman_DC.harmonic[i].cos_K = sincos_table_Kalman[i - 1].cosine * modifier;
        EMIF_mem.write.Kalman_DC.harmonic[i].sin_K = sincos_table_Kalman[i - 1].sine * modifier;
        EMIF_mem.write.Kalman_DC.harmonic[i].K1 = Kalman_gain_dc[2 * i] * modifier;
        EMIF_mem.write.Kalman_DC.harmonic[i].K2 = Kalman_gain_dc[2 * i + 1] * modifier;
    }

    ///////////////////////////////////////////////////////////////////

    Grid_params.Ts = Conv.Ts;
    float CT_SD_max_value[3];
    CT_SD_max_value[0] = CT_char_vars.CT_char.CT_ratio_a[0] * 5.0f;
    CT_SD_max_value[1] = CT_char_vars.CT_char.CT_ratio_b[0] * 5.0f;
    CT_SD_max_value[2] = CT_char_vars.CT_char.CT_ratio_c[0] * 5.0f;

    float decimation_grid = 625.0f;
    float OSR = 50.0f;

    static const float U_grid_max = 230.0f;
    static const float I_conv_max = 128.0f;
    static const float additional_range = 2.0f;

    ///////////////////////////////////////////////////////////////////
    CIC1_filter(&Grid_filter_params.CIC1_P_conv_1h[0], additional_range * U_grid_max * I_conv_max, OSR, decimation_grid);
    CIC1_filter(&Grid_filter_params.CIC1_P_conv_1h[1], additional_range * U_grid_max * I_conv_max, OSR, decimation_grid);
    CIC1_filter(&Grid_filter_params.CIC1_P_conv_1h[2], additional_range * U_grid_max * I_conv_max, OSR, decimation_grid);
    Grid_filter_params.CIC1_Q_conv_1h[0] = Grid_filter_params.CIC1_P_conv_1h[0];
    Grid_filter_params.CIC1_Q_conv_1h[1] = Grid_filter_params.CIC1_P_conv_1h[1];
    Grid_filter_params.CIC1_Q_conv_1h[2] = Grid_filter_params.CIC1_P_conv_1h[2];

    CIC1_filter(&Grid_filter_params.CIC1_P_grid_1h[0], additional_range * U_grid_max * CT_SD_max_value[0], OSR, decimation_grid);
    CIC1_filter(&Grid_filter_params.CIC1_P_grid_1h[1], additional_range * U_grid_max * CT_SD_max_value[1], OSR, decimation_grid);
    CIC1_filter(&Grid_filter_params.CIC1_P_grid_1h[2], additional_range * U_grid_max * CT_SD_max_value[2], OSR, decimation_grid);
    Grid_filter_params.CIC1_Q_grid_1h[0] = Grid_filter_params.CIC1_P_grid_1h[0];
    Grid_filter_params.CIC1_Q_grid_1h[1] = Grid_filter_params.CIC1_P_grid_1h[1];
    Grid_filter_params.CIC1_Q_grid_1h[2] = Grid_filter_params.CIC1_P_grid_1h[2];

    ///////////////////////////////////////////////////////////////////

    CIC1_filter(&Grid_filter_params.CIC1_U_grid_1h[0], additional_range * U_grid_max, OSR, decimation_grid);
    Grid_filter_params.CIC1_U_grid_1h[0] =
    Grid_filter_params.CIC1_U_grid_1h[1] =
    Grid_filter_params.CIC1_U_grid_1h[2] = Grid_filter_params.CIC1_U_grid_1h[0];

    CIC1_filter(&Grid_filter_params.CIC1_I_grid_1h[0], additional_range * I_conv_max, OSR, decimation_grid);
    Grid_filter_params.CIC1_I_grid_1h[0] =
    Grid_filter_params.CIC1_I_grid_1h[1] =
    Grid_filter_params.CIC1_I_grid_1h[2] = Grid_filter_params.CIC1_I_grid_1h[0];

    ///////////////////////////////////////////////////////////////////

    CIC1_filter(&Grid_filter_params.CIC1_U_grid[0], additional_range * U_grid_max, OSR, decimation_grid);
    Grid_filter_params.CIC1_U_grid[0] =
    Grid_filter_params.CIC1_U_grid[1] =
    Grid_filter_params.CIC1_U_grid[2] = Grid_filter_params.CIC1_U_grid[0];

    CIC1_filter(&Grid_filter_params.CIC1_I_conv[0], additional_range * I_conv_max, OSR, decimation_grid);
    Grid_filter_params.CIC1_I_conv[0] =
    Grid_filter_params.CIC1_I_conv[1] =
    Grid_filter_params.CIC1_I_conv[2] =
    Grid_filter_params.CIC1_I_conv[3] = Grid_filter_params.CIC1_I_conv[0];

    CIC1_filter(&Grid_filter_params.CIC1_I_grid[0], additional_range * CT_SD_max_value[0], OSR, decimation_grid);
    CIC1_filter(&Grid_filter_params.CIC1_I_grid[1], additional_range * CT_SD_max_value[1], OSR, decimation_grid);
    CIC1_filter(&Grid_filter_params.CIC1_I_grid[2], additional_range * CT_SD_max_value[2], OSR, decimation_grid);

    ///////////////////////////////////////////////////////////////////

    CIC1_filter(&Grid_filter_params.CIC1_THD_U_grid[0], 1000.0f, OSR, decimation_grid);
    Grid_filter_params.CIC1_THD_U_grid[0] =
    Grid_filter_params.CIC1_THD_U_grid[1] =
    Grid_filter_params.CIC1_THD_U_grid[2] =
    Grid_filter_params.CIC1_THD_I_grid[0] =
    Grid_filter_params.CIC1_THD_I_grid[1] =
    Grid_filter_params.CIC1_THD_I_grid[2] = Grid_filter_params.CIC1_THD_U_grid[0];

    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////

    CIC1_adaptive_filter(&Grid_params.CIC1_P_conv_1h[0], additional_range * U_grid_max * I_conv_max, full_OSR);
    CIC1_adaptive_filter(&Grid_params.CIC1_P_conv_1h[1], additional_range * U_grid_max * I_conv_max, full_OSR);
    CIC1_adaptive_filter(&Grid_params.CIC1_P_conv_1h[2], additional_range * U_grid_max * I_conv_max, full_OSR);
    Grid_params.CIC1_Q_conv_1h[0] = Grid_params.CIC1_P_conv_1h[0];
    Grid_params.CIC1_Q_conv_1h[1] = Grid_params.CIC1_P_conv_1h[1];
    Grid_params.CIC1_Q_conv_1h[2] = Grid_params.CIC1_P_conv_1h[2];

    CIC1_adaptive_filter(&Grid_params.CIC1_P_grid_1h[0], additional_range * U_grid_max * CT_SD_max_value[0], full_OSR);
    CIC1_adaptive_filter(&Grid_params.CIC1_P_grid_1h[1], additional_range * U_grid_max * CT_SD_max_value[1], full_OSR);
    CIC1_adaptive_filter(&Grid_params.CIC1_P_grid_1h[2], additional_range * U_grid_max * CT_SD_max_value[2], full_OSR);
    Grid_params.CIC1_Q_grid_1h[0] = Grid_params.CIC1_P_grid_1h[0];
    Grid_params.CIC1_Q_grid_1h[1] = Grid_params.CIC1_P_grid_1h[1];
    Grid_params.CIC1_Q_grid_1h[2] = Grid_params.CIC1_P_grid_1h[2];

    ///////////////////////////////////////////////////////////////////

    CIC1_adaptive_filter(&Grid_params.CIC1_U_grid_1h[0], additional_range * U_grid_max, full_OSR);
    Grid_params.CIC1_U_grid_1h[0] =
    Grid_params.CIC1_U_grid_1h[1] =
    Grid_params.CIC1_U_grid_1h[2] = Grid_params.CIC1_U_grid_1h[0];

    CIC1_adaptive_filter(&Grid_params.CIC1_I_grid_1h[0], additional_range * I_conv_max, full_OSR);
    Grid_params.CIC1_I_grid_1h[0] =
    Grid_params.CIC1_I_grid_1h[1] =
    Grid_params.CIC1_I_grid_1h[2] = Grid_params.CIC1_I_grid_1h[0];

    ///////////////////////////////////////////////////////////////////

    CIC1_adaptive_filter(&Grid_params.CIC1_U_grid[0], powf(additional_range * U_grid_max, 2.0f), full_OSR);
    Grid_params.CIC1_U_grid[0] =
    Grid_params.CIC1_U_grid[1] =
    Grid_params.CIC1_U_grid[2] = Grid_params.CIC1_U_grid[0];

    CIC1_adaptive_filter(&Grid_params.CIC1_I_conv[0], powf(additional_range * I_conv_max, 2.0f), full_OSR);
    Grid_params.CIC1_I_conv[0] =
    Grid_params.CIC1_I_conv[1] =
    Grid_params.CIC1_I_conv[2] =
    Grid_params.CIC1_I_conv[3] = Grid_params.CIC1_I_conv[0];

    CIC1_adaptive_filter(&Grid_params.CIC1_I_grid[0], powf(additional_range * CT_SD_max_value[0], 2.0f), full_OSR);
    CIC1_adaptive_filter(&Grid_params.CIC1_I_grid[1], powf(additional_range * CT_SD_max_value[1], 2.0f), full_OSR);
    CIC1_adaptive_filter(&Grid_params.CIC1_I_grid[2], powf(additional_range * CT_SD_max_value[2], 2.0f), full_OSR);

    ///////////////////////////////////////////////////////////////////

    Conv.Resonant_U_grid[0].trigonometric.ptr =
    Conv.Resonant_U_grid[1].trigonometric.ptr =
    Conv.Resonant_U_grid[2].trigonometric.ptr =
    Conv.Resonant_I_grid[0].trigonometric.ptr =
    Conv.Resonant_I_grid[1].trigonometric.ptr =
    Conv.Resonant_I_grid[2].trigonometric.ptr =
    Conv.Resonant_I_conv[0].trigonometric.ptr =
    Conv.Resonant_I_conv[1].trigonometric.ptr =
    Conv.Resonant_I_conv[2].trigonometric.ptr = &sincos_table[0];

    Conv.Resonant_U_grid[0].trigonometric_comp.ptr =
    Conv.Resonant_U_grid[1].trigonometric_comp.ptr =
    Conv.Resonant_U_grid[2].trigonometric_comp.ptr =
    Conv.Resonant_I_conv[0].trigonometric_comp.ptr =
    Conv.Resonant_I_conv[1].trigonometric_comp.ptr =
    Conv.Resonant_I_conv[2].trigonometric_comp.ptr = &Conv.zero_rot;

    register float rotation = 0.0f * MATH_2PI * 50.0f * Conv.Ts;
    Conv.zero_rot.sine = sinf(rotation);
    Conv.zero_rot.cosine = cosf(rotation);

    Conv.I_grid_rot[0].sine =
    Conv.I_grid_rot[1].sine =
    Conv.I_grid_rot[2].sine = 0.0f;
    Conv.I_grid_rot[0].cosine =
    Conv.I_grid_rot[1].cosine =
    Conv.I_grid_rot[2].cosine = 1.0f;

    Conv.Resonant_I_grid[0].trigonometric_comp.ptr = &Conv.I_grid_rot[0];
    Conv.Resonant_I_grid[1].trigonometric_comp.ptr = &Conv.I_grid_rot[1];
    Conv.Resonant_I_grid[2].trigonometric_comp.ptr = &Conv.I_grid_rot[2];

    Conv.Resonant_U_grid[0].gain =
    Conv.Resonant_U_grid[1].gain =
    Conv.Resonant_U_grid[2].gain =
    Conv.Resonant_I_grid[0].gain =
    Conv.Resonant_I_grid[1].gain =
    Conv.Resonant_I_grid[2].gain =
    Conv.Resonant_I_conv[0].gain =
    Conv.Resonant_I_conv[1].gain =
    Conv.Resonant_I_conv[2].gain = 2.0f / (MATH_2PI * 50.0f) / (MATH_1_E * 0.02f);

    Grid_params.Accumulator_gain = ((float)0x80000000 * 2.0f / 3600.0f) * Grid_params.Ts;
}

void Init_class::PWM_TZ_timestamp(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;

    EPwmReg->TBPRD = (float)EMIF_mem.read.control_rate * (float)EMIF_mem.read.cycle_period * 0.8f - 1.0f;//1599;                   // PWM frequency = 1/(TBPRD+1)
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

    //Configure ADCSoC pulse
    EPwmReg->ETSEL.bit.SOCASELCMP = 1;
    EPwmReg->ETSEL.bit.SOCASEL = 4; //4 counting up CMPC, 5 counting down
    EPwmReg->ETSEL.bit.SOCAEN = 1;
    EPwmReg->ETPS.bit.SOCAPRD = 1;

    EPwmReg->CMPC = 100;

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
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;
    EDIS;

    EALLOW;
    InputXbarRegs.INPUT5SELECT = SYNC_PWM_CM;
    EDIS;

    EALLOW;
    SyncSocRegs.SYNCSELECT.bit.EPWM4SYNCIN = 5;
    SyncSocRegs.SYNCSELECT.bit.EPWM7SYNCIN = 5;
    SyncSocRegs.SYNCSELECT.bit.EPWM10SYNCIN = 5;
    EDIS;

    PWM_TZ_timestamp(&EPwm4Regs);

    GPIO_Setup(TZ_EN_CPU1_CM);
    GPIO_Setup(TZ_EN_CPU2_CM);
}

void Init_class::Fan_speed()
{
    static volatile float Temp_fan = 0;
    Temp_fan = fmaxf(Meas_master.Temperature1,  fmaxf(Meas_master.Temperature2, Meas_master.Temperature3));
    static volatile float duty_f;
    duty_f = fminf(fmaxf((Temp_fan - FAN.on_temp) * FAN.slope + FAN.on_duty, 0.0f), 1.0f);
    static volatile float duty_f2;
    if(Conv.RDY)
        duty_f2 = fmaxf(Grid_filter.Used_resources.a, fmaxf(Grid_filter.Used_resources.b, fmaxf(Grid_filter.Used_resources.c, Grid_filter.Used_resources.n)));
    else
        duty_f2 = 0.0f;
    float duty_f_max = fmaxf(duty_f, duty_f2);

    static Uint16 fan_decrease_counter = 0;
    static Uint16 fan_state = 0;
    switch(fan_state)
    {
    case 0:
    {
        if(duty_f_max > 0.5f) fan_state++;
        else
        {
            GPIO_CLEAR(FAN_CM);
        }
        break;
    }
    case 1:
    {
        if(duty_f_max < 0.45f)
        {
            if(++fan_decrease_counter > 10) fan_state--, fan_decrease_counter = 0;
        }
        else
        {
            fan_decrease_counter = 0;
            GPIO_SET(FAN_CM);
        }
        break;
    }
    default:
    {
        fan_state = 0;
        break;
    }
    }
}

void Init_class::EMIF()
{
    EALLOW;

    CpuSysRegs.PCLKCR1.bit.EMIF1            = 0x1;
    ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0x0;

    Uint16  ErrCount = 0;

    //
    // Grab EMIF1 For CPU1. EMIF_selectMaster
    //
    Emif1ConfigRegs.EMIF1MSEL.all = 0x93A5CE71; //Writing the value 0x93A5CE7 will allow the writing of the  EMIF1M select bits
    if(Emif1ConfigRegs.EMIF1MSEL.all != 0x1)
    {
        ErrCount++;
    }
    //Disable Access Protection (CPU_FETCH/CPU_WR/DMA_WR)
    Emif1ConfigRegs.EMIF1ACCPROT0.all = 0x0;
    if(Emif1ConfigRegs.EMIF1ACCPROT0.all != 0x0)
    {
        ErrCount++;
    }
    // Commit the configuration related to protection. Till this bit remains set
    // content of EMIF1ACCPROT0 register can't be changed.
    Emif1ConfigRegs.EMIF1COMMIT.all = 0x1;
    if(Emif1ConfigRegs.EMIF1COMMIT.all != 0x1)
    {
       ErrCount++;
    }
    // Lock the configuration so that EMIF1COMMIT register can't be
    // changed any more.
    Emif1ConfigRegs.EMIF1LOCK.all = 0x1;
    if(Emif1ConfigRegs.EMIF1LOCK.all != 1)
    {
        ErrCount++;
    }

    Emif1Regs.ASYNC_CS2_CR.bit.SS       = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.EW       = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.W_SETUP  = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.W_STROBE = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.W_HOLD   = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.R_SETUP  = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.R_STROBE = 4;
    Emif1Regs.ASYNC_CS2_CR.bit.R_HOLD   = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.TA       = 0;
    Emif1Regs.ASYNC_CS2_CR.bit.ASIZE    = 2;

    EDIS;

    GPIO_Setup(EM1D0 );
    GPIO_Setup(EM1D1 );
    GPIO_Setup(EM1D2 );
    GPIO_Setup(EM1D3 );
    GPIO_Setup(EM1D4 );
    GPIO_Setup(EM1D5 );
    GPIO_Setup(EM1D6 );
    GPIO_Setup(EM1D7 );
    GPIO_Setup(EM1D8 );
    GPIO_Setup(EM1D9 );
    GPIO_Setup(EM1D10);
    GPIO_Setup(EM1D11);
    GPIO_Setup(EM1D12);
    GPIO_Setup(EM1D13);
    GPIO_Setup(EM1D14);
    GPIO_Setup(EM1D15);
    GPIO_Setup(EM1D16);
    GPIO_Setup(EM1D17);
    GPIO_Setup(EM1D18);
    GPIO_Setup(EM1D19);
    GPIO_Setup(EM1D20);
    GPIO_Setup(EM1D21);
    GPIO_Setup(EM1D22);
    GPIO_Setup(EM1D23);
    GPIO_Setup(EM1D24);
    GPIO_Setup(EM1D25);
    GPIO_Setup(EM1D26);
    GPIO_Setup(EM1D27);
    GPIO_Setup(EM1D28);
    GPIO_Setup(EM1D29);
    GPIO_Setup(EM1D30);
    GPIO_Setup(EM1D31);

    GPIO_Setup(EM1WE );
    GPIO_Setup(EM1OE );

    GPIO_Setup(EM1A0 );
    GPIO_Setup(EM1A1 );
    GPIO_Setup(EM1A2 );
    GPIO_Setup(EM1A3 );
    GPIO_Setup(EM1A4 );
    GPIO_Setup(EM1A5 );
    GPIO_Setup(EM1A6 );
    GPIO_Setup(EM1A7 );
    GPIO_Setup(EM1A8 );
    GPIO_Setup(EM1A9 );
    GPIO_Setup(EM1A10);
    GPIO_Setup(EM1A11);
}

const struct GPIO_struct GPIOreg[169] =
{
[TRIGGER_CM] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[RST_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[SD_AVG_CM] = {LOW, MUX0, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_NEW_CM] = {LOW, MUX0, CPU1_IO, INPUT, ASYNC | PULLUP},
[SYNC_PWM_CM] = {LOW, MUX0, CPU1_IO, INPUT, ASYNC | PULLUP},
[FAN_CM]  = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[TZ_EN_CPU1_CM] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM_EN_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[TZ_EN_CPU2_CM] = {LOW, MUX1, CPU2_IO, OUTPUT, PUSHPULL},

[LED1_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED2_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED3_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED4_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED5_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[C_SS_RLY_L1_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_L1_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},


[C_SS_RLY_L2_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_L2_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[C_SS_RLY_L3_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_L3_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[C_SS_RLY_N_CM ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_N_CM   ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},

[SD_SPISIMO_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPISOMI_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPICLK_PIN] = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[SD_SPISTE_PIN] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[SS_DClink_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
//[DClink_DSCH_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[ON_OFF_CM] = {LOW, MUX0, CPU1_IO, INPUT, QUAL6 | PULLUP},

[EN_Mod_1_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[EN_Mod_2_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[TX_Mod_1_CM]  = {HIGH, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[RX_Mod_1_CM]  = {HIGH, MUX1, CPU1_IO, INPUT, ASYNC},
[TX_Mod_2_CM]  = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[RX_Mod_2_CM]  = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC},

[I2CA_SDA_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC|PULLUP},
[I2CA_SCL_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC|PULLUP},
[I2CB_SDA_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC|PULLUP},
[I2CB_SCL_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC|PULLUP},

[EM1D0 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D1 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D2 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D3 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D4 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D5 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D6 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D7 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D8 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D9 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D10] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D11] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D12] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D13] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D14] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D15] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D16] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D17] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D18] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D19] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D20] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D21] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D22] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D23] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D24] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D25] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D26] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D27] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D28] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D29] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D30] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1D31] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},

[EM1WE ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1OE ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},

[EM1A0 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A1 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A2 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A3 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A4 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A5 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A6 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A7 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A8 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A9 ] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A10] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
[EM1A11] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
};

void Init_class::GPIO()
{
    GPIO_Setup(TRIGGER_CM);

    GPIO_Setup(RST_CM);
    GPIO_Setup(SD_AVG_CM);
    GPIO_Setup(SD_NEW_CM);
    GPIO_Setup(SYNC_PWM_CM);
    GPIO_Setup(FAN_CM);

    GPIO_Setup(PWM_EN_CM);

    GPIO_Setup(LED1_CM);
    GPIO_Setup(LED2_CM);
    GPIO_Setup(LED3_CM);
    GPIO_Setup(LED4_CM);
    GPIO_Setup(LED5_CM);

//    GPIO_Setup(DClink_DSCH_CM);
    GPIO_Setup(C_SS_RLY_L1_CM);
    GPIO_Setup(GR_RLY_L1_CM  );
    GPIO_Setup(C_SS_RLY_L2_CM);
    GPIO_Setup(GR_RLY_L2_CM  );
    GPIO_Setup(C_SS_RLY_L3_CM);
    GPIO_Setup(GR_RLY_L3_CM  );
    GPIO_Setup(C_SS_RLY_N_CM );
    GPIO_Setup(GR_RLY_N_CM   );
    GPIO_Setup(SS_DClink_CM  );

    GPIO_Setup(ON_OFF_CM);

    GPIO_Setup(I2CA_SDA_PIN);
    GPIO_Setup(I2CA_SCL_PIN);
    GPIO_Setup(I2CB_SDA_PIN);
    GPIO_Setup(I2CB_SCL_PIN);
}
