
/*
 *  ======== main.c ========
 */

#include "stdafx.h"

#include "State_background.h"
#include "State_master.h"
#include "State_slave.h"

#include "Init.h"
#include "HWIs.h"
#if MERGE_PART
<<<<<<< HEAD
=======
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

#pragma CODE_SECTION(".TI.ramfunc_unsecure");
//#pragma CODE_SECTION(".TI.ramfunc");
void block_in_ram(){
    while(1){
    asm (" nop ");
    }
}

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void NMI_INT()
{
    ESTOP0;
}
>>>>>>> dev-pr-fw-protected
#endif

void main()
{
    EALLOW;
    *(Uint32 *)0xD00 = 0x0B5A;
    EDIS;

    DINT;

    // ponizsze wlaczane w FLASH.cpp (save fn), dopiero jak bedzie potrzebne
    // aby nie zezwolic wywolania fn prorgamowania z niezabezp. czesci
//    EALLOW;
//    //DCSM_COMMON_REGS.FLSEM (offset=0)
//    // operacje kasowania i programowania dozwolone ze strefy 1 (Security Zone 1)
//    //*(uint32_t*)0x0005F070 = 0xa501;
//    EDIS;

    // block_in_ram();

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
        // block_in_ram();
    }
}
