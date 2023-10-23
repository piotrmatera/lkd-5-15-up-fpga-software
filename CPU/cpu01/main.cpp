
/*
 *  ======== main.c ========
 */
#include <string.h>
#include "stdafx.h"

#include "State_background.h"
#include "State_master.h"
#include "State_slave.h"

#include "Init.h"
#include "HWIs.h"

#include "device_check.h"


#include "testing-eeprom.h"
#include "i2c_transactions.h"

extern i2c_transactions_t i2c_bus;

void test_eeprom_page128(void){
    struct msg_buffer msg;
    memset(&msg, 0xde, sizeof(msg));

    msg.len = 30;
    //for(Uint16 i = 0; i<msg.len; i++)
    //    msg.data[i] = i+10;

    msg.ready = 0;

    DELAY_US(20000);

    i2c_bus.i2c.set_slave_address( 0x50 );
    //while( i2c_bus.i2c.write(&msg, 0) != status_ok );

    struct msg_buffer msg_write_addr;
    msg_write_addr.data[0] = 10;
    msg_write_addr.data[1] = 11;
    msg_write_addr.len = 2;
    msg_write_addr.ready = 0;

    while( i2c_bus.i2c.write_nostop(&msg_write_addr, 0) != status_ok );
    while( msg_write_addr.ready == 0);

    msg.len = 30;
    msg.ready = 0;

    while( i2c_bus.i2c.read(&msg, 0) != status_ok );
    while( msg.ready == 0);

    while(1);
}



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
#if FW_FOR_CALIBRATION
    if( is_unprogrammed_device_id() ){
            programm_device_id();

            if( !is_correct() ){

                GPIO_Setup(LED1_CM);
                GPIO_WRITE( LED1_CM, 1 );
                while(1){};
            }

     }
#endif
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

    test_eeprom_page128();
    while(1)
    {
#if TESTING_EEPROM_NV
        testing_eeprom_nv();
#endif

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
