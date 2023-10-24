/*
 * testing-eeprom.cpp
 *
 *  Created on: 17 paü 2023
 *      Author: Piotr
 */
#include <string.h>
#include "Software/driver_eeprom/eeprom_i2c.h"
#include "i2c_transactions.h"
#include "nonvolatile.h"
#include "Rtc.h"
#include "testing-eeprom.h"

extern eeprom_i2c eeprom;
extern i2c_transactions_t i2c_bus;

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



#if TESTING_NONVOLATILE
uint16_t test_eeprom_buffer[ 64 ]; //do testow - obraz eepromu
uint16_t test_nv_shadow_buffer[ 128 ];  //jako bufor tymczasowy do dzialania nonvolatile


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

const region_memories_t test_nv_regions[] = {
  // REGION index = 0 L_grid_test
  {  /*ext*/{ .address.ptr_u16 = (Uint16 *)&Lgrid.L,      .size = 28 },//musi byc wielokrotnosc 2 - kopiowanie ext<->int slowami u16
     /*int*/{ .address.ptr_u16 = &test_nv_shadow_buffer[16/2], .size = 30 },
     /*eep*/{ .address.u16 = 16, .size = 32 /*NU?*/  }}
};


const class nonvolatile_t test_nonvolatile = //korzysta z globalnego obiektu eeprom
{
     .regions_count  = 1,
     .regions = test_nv_regions
};

#define LGRID_REGION 0



void _test_nv(){
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
        test_nonvolatile.save(LGRID_REGION, 0 /*ms*/); //test timeoutu

        cnt = 3;

        break;
    }
    case 3: //odczyt regionu - blokujaco
        Lgrid_clear();
        test_nonvolatile.retrieve(LGRID_REGION, 0 /*ms*/);
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

void test_nv(){
    while(1){
        _test_nv();
        i2c_bus.process(); // wywolywanie cykliczne konieczne do dzialania i2c


    }
}

#endif
