
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
int32 SD_phase = -500;

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

    EALLOW;
    InputXbarRegs.INPUT6SELECT = SD_NEW_CM;
    EDIS;

    EALLOW;
    InputXbarRegs.INPUT4SELECT = SD_AVG_CM;
    XintRegs.XINT1CR.bit.POLARITY = 0;
    XintRegs.XINT1CR.bit.ENABLE = 1;
    PieVectTable.XINT1_INT = &SD_AVG_NT;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
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

    Init.DMA();

    Init.EMIF();

    Init.CLA();

    Init.PWMs();

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

    if(EMIF_mem.read.cycle_period != CYCLE_PERIOD) while(1);

    Machine.state = Machine_class::state_init;
    while(1)
    {
        Machine.Main();
        Machine.Background();

        int32 max_period = (int32)EMIF_mem.read.cycle_period - 1L;
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
