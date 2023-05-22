
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

const float Kalman_gain[2 * FPGA_KALMAN_STATES] =
{
 0.000448842521485309 ,
 0                    ,
 0.000634103002618609 ,
 -2.88548133061653e-05,
 0.000634669298262515 ,
 -1.06817608881194e-05,
 0.000634712723653466 ,
 -7.67962359421781e-06,
 0.000634722176840594 ,
 -6.85393361040006e-06,
 0.000634723172838953 ,
 -6.76106923904479e-06,
 0.000634720478185073 ,
 -7.00947417941968e-06,
 0.000634715452715322 ,
 -7.45065653159282e-06,
 0.000634708546694967 ,
 -8.01741640013077e-06,
 0.000634699881257989 ,
 -8.67634288898916e-06,
 0.000634689421983918 ,
 -9.41040974319991e-06,
 0.000634677036476851 ,
 -1.02116369173533e-05,
 0.000634662510249319 ,
 -1.10777356511361e-05,
 0.000634645542307851 ,
 -1.20105692603990e-05,
 0.000634625726622169 ,
 -1.30155801054626e-05,
 0.000634602517973840 ,
 -1.41018557348941e-05,
 0.000634575177028539 ,
 -1.52827659806226e-05,
 0.000634542679006945 ,
 -1.65772948895905e-05,
 0.000634503562165124 ,
 -1.80124327442099e-05,
 0.000634455657977260 ,
 -1.96274362666893e-05,
 0.000634395580292669 ,
 -2.14817563008641e-05,
 0.000634317667910526 ,
 -2.36709832563470e-05,
 0.000634211495935667 ,
 -2.63627931284139e-05,
 0.000634054915348005 ,
 -2.98928506874042e-05,
 0.000633787960896638 ,
 -3.51004164430066e-05,
 0.000633137770845265 ,
 -4.53407246376031e-05,
};

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
    PieVectTable.XINT1_INT = &SD_INT;
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
