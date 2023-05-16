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

    Init.EPwm_TZclear(&EPwm4Regs);

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

    DevCfgRegs.CPUSEL0.bit.EPWM10 = 1;

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
    MemCfgRegs.GSxMSEL.bit.MSEL_GS13 = 1;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS14 = 1;
    MemCfgRegs.GSxMSEL.bit.MSEL_GS15 = 1;

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
    Conv.Ts = (float)EMIF_mem.read.cycle_period * 8e-9 * (2.0f - (float)EMIF_mem.read.oversample);
    Conv.compensation2 = 2.0f;

    Meas_alarm_L.U_grid_rms = 5.0f;
    Meas_alarm_H.U_grid_abs = 380.0f;

    Meas_alarm_H.Temp = 95.0f;
    Meas_alarm_L.Temp = 0.0f;

    Meas_alarm_H.U_dc = 750.0f;
    Meas_alarm_L.U_dc = -5.0f;
    Meas_alarm_H.U_dc_balance = 30.0f;

    Meas_alarm_H.I_conv_rms = Conv.I_lim_nominal * 1.1f;
    float min_Current = fabsf(Meas_master_gain.def_osr * Meas_master_gain.def_osr / Meas_master_gain.sd_shift * fminf(Meas_master_gain.I_conv.a, fminf(Meas_master_gain.I_conv.b, fminf(Meas_master_gain.I_conv.c, Meas_master_gain.I_conv.n))));
    Meas_alarm_H.I_conv = fminf(min_Current - 10.0f, Conv.I_lim_nominal * 2.0f);
    Meas_alarm_L.I_conv = -Meas_alarm_H.I_conv;

    ///////////////////////////////////////////////////////////////////

    PLL.Ts = Conv.Ts;
    PLL.PI.Kp = 92.0f;
    PLL.PI.Ts_Ti = PLL.Ts / 0.087f;
    PLL.PI.lim_H = 400.0f;
    PLL.PI.lim_L = -400.0f;

    float decimation_PLL = 16.0f;
    float OSR_PLL = (Uint16)(0.02f / (Conv.Ts * decimation_PLL) + 0.5f);
    CIC2_filter(&PLL.CIC_w, 410.0f, OSR_PLL, decimation_PLL);

    PLL.state = PLL_omega_init;
    PLL.state_last = PLL_active;

    ///////////////////////////////////////////////////////////////////

    CIC2_calibration.decimation_ratio = 10.0f;
    CIC2_calibration.decimation_counter = 4.0f;
    CIC2_calibration.OSR = 125;
    CIC2_calibration.div_OSR = 1.0f / CIC2_calibration.OSR;
    CIC2_calibration.range_modifier = 2500.0f;
    CIC2_calibration.div_range_modifier = 1.0f / CIC2_calibration.range_modifier;
    CIC2_calibration_input.ptr = &Meas_master.I_grid_avg.b;

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
}

void Init_class::PWM_TZ_timestamp(volatile struct EPWM_REGS *EPwmReg)
{
    EALLOW;

    EPwmReg->TBPRD = 1599;                   // PWM frequency = 1/(TBPRD+1)
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

    GPIO_Setup(TZ_EN_CM);
}

void Init_class::Fan_speed()
{
    static volatile float Temp_fan = 0;
    Temp_fan = fmaxf(Meas_master.Temperature1,  fmaxf(Meas_master.Temperature2, Meas_master.Temperature3));
    static volatile float duty_f;
    duty_f = fminf(fmaxf((Temp_fan - FAN.on_temp) * FAN.slope + FAN.on_duty, 0.0f), 1.0f);
    static volatile float duty_f2;
    if(Conv.RDY)
        duty_f2 = fmaxf(CLA2toCLA1.Grid_filter.Used_resources.a, fmaxf(CLA2toCLA1.Grid_filter.Used_resources.b, fmaxf(CLA2toCLA1.Grid_filter.Used_resources.c, CLA2toCLA1.Grid_filter.Used_resources.n)));
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

    GPIO_Setup(EM1CS0);
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

#define TRIGGER0_CM  0
#define TRIGGER1_CM  1

#define RST_CM  2
#define SD_NEW_CM  3
#define SYNC_PWM_CM  4
#define ON_OFF_CM  5
#define TZ_EN_CM  6
#define PWM_EN_CM  36
#define FAN_CM  84

#define LED1_CM  7
#define LED2_CM  8
#define LED3_CM  9
#define LED4_CM  10
#define LED5_CM  11

#define SS_DClink_CM  15
#define DClink_DSCH_CM  x

#define C_SS_RLY_L1_CM  16
#define GR_RLY_L1_CM  17
#define C_SS_RLY_L2_CM  18
#define GR_RLY_L2_CM  19
#define C_SS_RLY_L3_CM  20
#define GR_RLY_L3_CM  21
#define C_SS_RLY_N_CM  22
#define GR_RLY_N_CM  23

const struct GPIO_struct GPIOreg[169] =
{
[SD_SPISIMO_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPISOMI_PIN] = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC | PULLUP},
[SD_SPICLK_PIN] = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[SD_SPISTE_PIN] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[TRIGGER0_CM] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[TRIGGER1_CM] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

[RST_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[SD_NEW_CM] = {LOW, MUX0, CPU1_IO, INPUT, ASYNC | PULLUP},
[SYNC_PWM_CM] = {LOW, MUX0, CPU1_IO, INPUT, ASYNC | PULLUP},
[ON_OFF_CM] = {LOW, MUX0, CPU1_IO, INPUT, QUAL6 | PULLUP},

[TZ_EN_CM] = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[PWM_EN_CM] = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[FAN_CM]  = {LOW, MUX1, CPU1_IO, OUTPUT, PUSHPULL},

[LED1_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED2_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED3_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED4_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[LED5_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},

//[DClink_DSCH_CM] = {HIGH, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
//[SS_DClink_CM  ] = {LOW, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[SS_DClink_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[C_SS_RLY_L1_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_L1_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[C_SS_RLY_L2_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_L2_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[C_SS_RLY_L3_CM] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_L3_CM  ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[C_SS_RLY_N_CM ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},
[GR_RLY_N_CM   ] = {LOW, MUX0, CPU1CLA_IO, OUTPUT, PUSHPULL},

[EN_Mod_1_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[EN_Mod_2_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[EN_Mod_3_CM]  = {LOW, MUX0, CPU1_IO, OUTPUT, PUSHPULL},
[TX_Mod_1_CM]  = {HIGH, MUX1, CPU1_IO, OUTPUT, PUSHPULL},
[RX_Mod_1_CM]  = {HIGH, MUX1, CPU1_IO, INPUT, ASYNC},
[TX_Mod_2_CM]  = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[RX_Mod_2_CM]  = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC},
[TX_Mod_3_CM]  = {HIGH, MUX6, CPU1_IO, OUTPUT, PUSHPULL},
[RX_Mod_3_CM]  = {HIGH, MUX6, CPU1_IO, INPUT, ASYNC},

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

[EM1CS0] = {HIGH, MUX2, CPU1_IO, INPUT, ASYNC},
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
    GPIO_Setup(RST_CM);
    GPIO_Setup(ON_OFF_CM);
    GPIO_Setup(SYNC_PWM_CM);
    GPIO_Setup(SD_NEW_CM);

    GPIO_Setup(PWM_EN_CM);
    GPIO_Setup(FAN_CM);

    GPIO_Setup(TRIGGER0_CM);
    GPIO_Setup(TRIGGER1_CM);

    GPIO_Setup(LED1_CM);
    GPIO_Setup(LED2_CM);
    GPIO_Setup(LED3_CM);
    GPIO_Setup(LED4_CM);
    GPIO_Setup(LED5_CM);

//    GPIO_Setup(DClink_DSCH_CM);
    GPIO_Setup(SS_DClink_CM  );
//    GPIO_Setup(C_SS_RLY_L1_CM);
//    GPIO_Setup(GR_RLY_L1_CM  );
//    GPIO_Setup(C_SS_RLY_L2_CM);
//    GPIO_Setup(GR_RLY_L2_CM  );
//    GPIO_Setup(C_SS_RLY_L3_CM);
//    GPIO_Setup(GR_RLY_L3_CM  );
//    GPIO_Setup(C_SS_RLY_N_CM );
//    GPIO_Setup(GR_RLY_N_CM   );

    GPIO_Setup(I2CA_SDA_PIN);
    GPIO_Setup(I2CA_SCL_PIN);
    GPIO_Setup(I2CB_SDA_PIN);
    GPIO_Setup(I2CB_SCL_PIN);
}
