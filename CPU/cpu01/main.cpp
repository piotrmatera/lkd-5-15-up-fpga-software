
/*
 *  ======== main.c ========
 */

#include "stdafx.h"

#include "State_background.h"
#include "State_master.h"
#include "State_slave.h"

#include "Init.h"
#include "HWIs.h"

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
    PieVectTable.XINT1_INT = &SD_AVG_INT;
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

    InitTempSensor(3.3f);

    Init.ADC();

    Background.init();

    while(1)
    {
        Machine_slave.Main();
        Machine_master.Main();
        Background.Main();

        static volatile int32 SD_phase = -600;
        int32 max_period = (int32)EMIF_mem.read.cycle_period - 1L;
        int32 SD_phase_temp = SD_phase;
        if(SD_phase < 0) SD_phase_temp = max_period + SD_phase;
        if(SD_phase_temp < 0) SD_phase_temp = 0;
        if(SD_phase_temp > max_period) SD_phase_temp = max_period;
        EMIF_mem.write.SD_sync_val = SD_phase_temp;
        EMIF_mem.write.local_counter_phase_shift = Conv.PWM_phase_shift * (float)EMIF_mem.read.cycle_period;

        static Uint64 benchmark_timer;
        static volatile float benchmark;
        benchmark = (float)(ReadIpcTimer() - benchmark_timer)*(1.0f/200000000.0f);
        benchmark_timer = ReadIpcTimer();
    }
}
