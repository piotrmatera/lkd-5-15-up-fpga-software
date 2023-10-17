
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
#include "i2c_transactions.h"
#include "Software/driver_eeprom/eeprom_i2c.h"
#include "i2c_transactions.h"
#include "nonvolatile.h"
#include "Rtc.h"

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


extern eeprom_i2c eeprom;

//#define TESTING_EEPROM_I2C 1

#if TESTING_EEPROM_I2C
uint16_t memory_buffer[256]; //na razie zbior bajtow; TODO zrobic aby byl zapis obu bajtow ze slowa

void test_eeprom(){
    static uint16_t cnt = 1;

    static struct eeprom_i2c::event_region_xdata xdata;

    switch( cnt ){
    case 1: //odczyt

        xdata.total_len = 19;
        xdata.start = 8;
        xdata.data = memory_buffer;

        xdata.status = eeprom_i2c::event_region_xdata::idle;
        eeprom.process_event(eeprom_i2c::event_read_region, &xdata);
        cnt = 2;
        break;

    case 2: //zapis
        if( xdata.status >= eeprom_i2c::event_region_xdata::done_ok )
        {
            uint32_t tm = ReadIpcTimer();
            memory_buffer[0]  = tm >> 16;
            memory_buffer[1]  = tm &0xFFFF;

            tm = ReadIpcTimer();
            memory_buffer[2] =  tm &0xFFFF;

            memory_buffer[3] =  0x1234;
            memory_buffer[4] =  0x5678;

            tm = ReadIpcTimer();
            memory_buffer[5] =  tm & 0xFFFF;
            memory_buffer[6] =  tm >> 16;

            tm = ReadIpcTimer();
            memory_buffer[7] =  tm & 0xFFFF;

            tm = ReadIpcTimer();
            memory_buffer[8] =  tm & 0xFFFF;

            tm = ReadIpcTimer();
            memory_buffer[9] =  tm & 0xFFFF;

            tm = ReadIpcTimer();
            memory_buffer[10] = tm & 0xFFFF;


            xdata.total_len = 19;
            xdata.start = 8;
            xdata.data = memory_buffer;

            xdata.status = eeprom_i2c::event_region_xdata::idle;
            eeprom.process_event(eeprom_i2c::event_write_region, &xdata);
            cnt = 3;
        }
        break;
    case 3: //oczekiwanie na zakonczenie
        if( xdata.status >= eeprom_i2c::event_region_xdata::done_ok ){
            dbg_marker('K');
            xdata.total_len = 19;
            xdata.start = 8;
            xdata.data = memory_buffer;

            xdata.status = eeprom_i2c::event_region_xdata::idle;
            eeprom.process_event(eeprom_i2c::event_read_region, &xdata);
            cnt = 10;
        }
        break;
    case 4: //zapis
        memory_buffer[0] = 0x6261;
        memory_buffer[1] = 0x6463;
        memory_buffer[2] = 0x65;

        xdata.status = eeprom_i2c::event_region_xdata::idle;
        eeprom.process_event(eeprom_i2c::event_write_region, &xdata);
        cnt = 5;
        break;
//    case 5:
//        if( eeprom.x_msg_polling.msg.ready == 1)
//            cnt = 1;
//
//        break;

    }
}
#endif

#define TESTING_NONVOLATILE 1

#if TESTING_NONVOLATILE
uint16_t test_eeprom_buffer[ 64 ]; //do testow - obraz eepromu
uint16_t nv_shadow_buffer[ 128 ];  //jako bufor tymczasowy do dzialania nonvolatile


extern Rtc rtc;


//uint16_t memory_buffer[10]; //na razie zbior bajtow; TODO zrobic aby byl zapis obu bajtow ze slowa

/*
 * obraz eepromu (128 bajtow)
 * strony 8 bajtowe
 *
 * czesc informacyjna
 *   [0] crc-info (crc tylko 1. czesci?)
 *   [2] eeprom-img-version
 *   [4] pcb-version
 *   [6] next info addr
 *
 *   [64] crc-info2?
 *   [66-(64+7)] res
 *
 * Dane zorganizowane w regiony (regiony maja wyrownane poczatki i dlugosci do rozmiaru strony); dwie kopie kazdego regionu
 * TODO sprawdzic czy mozna zapisywac (szybko) w obrebie 1 strony ale niewyrownane dane o rozmiarze innym niz wielkosc strony
 *
 * kopia 1. [8]-[63]
 * kopia 2. kopia1.+64
 *
 * Kazdy region rozpoczyna sie od 16 bitowej struktury CRC:
 *   bajtH=0-valid, bajtL=crc z tresci regionu
 *
 * Potem tresc regionu.
 * Ponizej zdefiniowane regiony, kazdy region posiada 3 definiecje:
 *   pamiec zewnetrzna (ext) - to jest tresc na zewnatrz ktora bedzie zapisywana do eepromu
 *   pamiec wewnetrzna - jest potrzebna do posredniego odczytywania z eepromu, odczytane dane moga byc niewazne a nie powinny zmienic tresci na zewnatrz,
 *      jest poprzedzone u16 crc - rozmiar musi to uwzgledniac
 *      (rozmiar moze byc bez koncowych pol reserved [?]
 *      na razie rozmiar wyrownany do wielkosci strony aby nie bylo starej tresci, ktora pozostaje w eepromie podczas zapisow
 *   pamiec w eepromie - okreslenie gdize jest polozony region w eepromie
 *
 * */


struct _Lgrid{
    Uint16 before[16];
    struct {
        float L[7];
    }L;
    Uint16 after[16];
} Lgrid;

static void Lgrid_init(){
    memset( &Lgrid, 0xDEDE, sizeof(Lgrid) );
    Lgrid.L.L[0]= 0.1001;
    Lgrid.L.L[1]= 0.2001;
    Lgrid.L.L[2]= 0.3001;
    Lgrid.L.L[3]= 0.4001;
    Lgrid.L.L[4]= 0.5001;
    Lgrid.L.L[5]= 0.6001;
    Lgrid.L.L[6]= 0.7001;
}

static void Lgrid_clear(){
    memset( &Lgrid, 0xBCBC, sizeof(Lgrid) );
}

const region_memories_t _nv_regions[] = {
  // REGION index = 0 L_grid_test
  {  /*ext*/{ .address.ptr_u16 = (Uint16 *)&Lgrid.L,      .size = 28 },//musi byc wielokrotnosc 2 - kopiowanie ext<->int slowami u16
     /*int*/{ .address.ptr_u16 = &nv_shadow_buffer[16/2], .size = 30 },
     /*eep*/{ .address.u16 = 16, .size = 32 /*NU?*/  }}
};


const class nonvolatile_t nonvolatile = //korzysta z globalnego obiektu eeprom
{
     .regions_count  = 1,
     .regions = _nv_regions
};

#define LGRID_REGION 0

void test_nv(){
    static uint16_t cnt = 1;

    static struct eeprom_i2c::event_region_xdata xdata;
    static Uint64 delay;

    switch( cnt ){
    case 1: //odczyt calej pamieci eeprom
        memset( test_eeprom_buffer, 0xDEDE, sizeof(test_eeprom_buffer));
        xdata.total_len = 128;
        xdata.start = 0;
        xdata.data = test_eeprom_buffer;

        xdata.status = eeprom_i2c::event_region_xdata::idle;
        eeprom.process_event(eeprom_i2c::event_read_region, &xdata);
        cnt = 11;
        break;

    case 11: //oczekiwanie na zakonczenie odczytu
        if( xdata.status >= eeprom_i2c::event_region_xdata::done_ok ){
            dbg_marker('K');
            delay = ReadIpcTimer() + 100ULL /*ms*/*1000ULL *200ULL;             //opoznienie aby na LA odroznic tresc eepromu od operacji
            cnt = 12;
        }
        break;

    case 12:
        if( ReadIpcTimer() > delay ){
            cnt = 2;
        }
        break;

    case 2: {//zapis regionu - blokujaco
        //rtc.process_event(Rtc::event_read_time);//aby wymusic test z zajetym i2cbus przy wywolaniu nv.save
        static Rtc::datetime_bcd_s new_time;
        new_time.day = 10;
        new_time.month = 3;
        new_time.year = 2024;
        new_time.dayofweek = 1;
        new_time.hour = 12;
        new_time.min = 57;
        new_time.sec = 34;

        rtc.process_event(Rtc::event_setup_time_bcd, &new_time);

        Lgrid_init();
        Lgrid.L.L[0] = *(float*)&delay; //aby byla zmienna tresc
        nonvolatile.save(LGRID_REGION, 100 /*ms*/); //test timeoutu

        cnt = 3;

        break;
    }
    case 3: //odczyt regionu - blokujaco
        Lgrid_clear();
        nonvolatile.retrieve(LGRID_REGION, 100 /*ms*/);
        cnt = 4;
        break;

    case 4: //
        memset( test_eeprom_buffer, 0xDEDE, sizeof(test_eeprom_buffer));
        xdata.total_len = 128;
        xdata.start = 0;
        xdata.data = test_eeprom_buffer;

        xdata.status = eeprom_i2c::event_region_xdata::idle;
        eeprom.process_event(eeprom_i2c::event_read_region, &xdata);
        cnt = 5;
        break;

    case 5:
        if( xdata.status >= eeprom_i2c::event_region_xdata::done_ok ){
            dbg_marker('W');
            cnt = 100;
        }
        break;

    case 100:
        cnt = 100;
        break;

//    case 5:
//        if( eeprom.x_msg_polling.msg.ready == 1)
//            cnt = 1;
//
//        break;

    }
}
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

    if( is_unprogrammed_device_id() ){
            programm_device_id();

            if( !is_correct() ){

                GPIO_Setup(LED1_CM);
                GPIO_WRITE( LED1_CM, 1 );
                while(1){};
            }

     }

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
#if TESTING_EEPROM_I2C
        test_eeprom();
#endif
#if TESTING_NONVOLATILE
        test_nv();
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
