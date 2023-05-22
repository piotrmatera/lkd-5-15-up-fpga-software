
/*
 *  ======== main.c ========
 */
#include <string.h>
#include "stdafx.h"
#include "Init.h"
#include "HWIs.h"
#include "State.h"
#include "ff.h"
#include "Rtc.h"
#include "diskio.h"
#include "Software/driver_mosfet/MosfetDriver.h"
#include "MosfetCtrlApp.h"
#include "math.h"

Rtc rtc;
FATFS fs;           /* Filesystem object */
MosfetCtrlApp mosfet_ctrl_app;
int32 SD_phase = 1500;

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void NMI_INT()
{
    ESTOP0;
}

void main()
{
    EALLOW;
    *(Uint32 *)0xD00 = 0x0B5A;
    EDIS;

    DINT;

    while(!GPIO_READ(FPGA_DONE));

    InitFlash();
    InitSysPll(XTAL_OSC, IMULT_20, FMULT_0, PLLCLK_BY_2);

    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

//    EALLOW;
//    InputXbarRegs.INPUT6SELECT = EM1A2;
//    OutputXbarRegs.OUTPUT4MUX0TO15CFG.bit.MUX11 = 1;
//    OutputXbarRegs.OUTPUT4MUXENABLE.bit.MUX11 = 1;
//    EDIS;

    EALLOW;
    InputXbarRegs.INPUT4SELECT = SD_NEW_CM;
    XintRegs.XINT1CR.bit.POLARITY = 0;
    XintRegs.XINT1CR.bit.ENABLE = 1;
    PieVectTable.IPC3_INT = &SD_INT;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx16 = 1;
    IER |= M_INT1;


    EALLOW;
    PieVectTable.NMI_INT = &NMI_INT;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

//    static volatile Uint16 go_on1 = 0;
//    while(!go_on1);

    Init.CPUS();

    Init.GPIO();

    Init.CLA();

    Init.PWMs();

    Init.EMIF();

    Init.ADC();

    rtc.init();
    rtc.process_event(Rtc::event_init);
    RTC_new_time.second = 0;
    RTC_new_time.second10 = 0;
    RTC_new_time.minute = 9;
    RTC_new_time.minute10 = 4;
    RTC_new_time.hour = 1;
    RTC_new_time.hour10 = 1;
    RTC_new_time.day = 8;
    RTC_new_time.day10 = 0;
    RTC_new_time.month = 3;
    RTC_new_time.month10 = 0;
    RTC_new_time.year = 1;
    RTC_new_time.year10 = 2;

    FatFS_time.second_2 = 5;
    FatFS_time.minute = 10;
    FatFS_time.hour = 10;
    FatFS_time.day = 10;
    FatFS_time.month = 10;
    FatFS_time.year = 10 + 20;

    f_mount(&fs, "", 1);

    mosfet_ctrl_app.init();

    float omega = MATH_2PI * 50.0f * 32e-6;
    for(Uint16 i = 0; i < FPGA_KALMAN_STATES; i++)
    {
        int32 temp;
        temp = cosf(omega * (float)(2 * i - 1)) * (float)(1UL<<31);
        EMIF_mem.write.Kalman[0].harmonic[i].cosine = temp;
        temp = sinf(omega * (float)(2 * i - 1)) * (float)(1UL<<31);
        EMIF_mem.write.Kalman[0].harmonic[i].sine = temp;
        temp = Kalman_gain[2 * i] * (float)(1UL<<31);
        EMIF_mem.write.Kalman[0].harmonic[i].Kalman_gain_1 = temp;
        temp = Kalman_gain[2 * i + 1] * (float)(1UL<<31);
        EMIF_mem.write.Kalman[0].harmonic[i].Kalman_gain_2 = temp;
    }

    Machine.state = Machine_class::state_init;
    while(1)
    {
        Machine.Main();
        Machine.Background();

        int32 max_period = 1999;//(int32)EMIF_mem.read.cycle_period - 1L;
        int32 SD_phase_temp = SD_phase;
        if(SD_phase < 0) SD_phase_temp = max_period + SD_phase;
        if(SD_phase_temp < 0) SD_phase_temp = 0;
        if(SD_phase_temp > max_period) SD_phase_temp = max_period;
        *(Uint32 *)&EMIF_mem.write.SD_sync_val = *(Uint32 *)&SD_phase_temp;

        static Uint64 benchmark_timer;
        static volatile float benchmark;
        benchmark = (float)(ReadIpcTimer() - benchmark_timer)*(1.0f/200000000.0f);
        benchmark_timer = ReadIpcTimer();
    }
}
