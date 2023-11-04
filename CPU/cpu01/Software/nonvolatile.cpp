/*
 * nonvolatile.cpp
 *
 *  Created on: 13 kwi 2023
 *      Author: Piotr
 */
#include <string.h>
#include "nonvolatile.h"
#include "stats.h"

extern eeprom_i2c eeprom;
extern i2c_transactions_t i2c_bus;//magistrala i2c na ktorej jest RTC i EEPROM

#define NONVOLATILE_INVALID_CRC 0xDEAD

stats_t stats;

//#include "crc8.h"

static const Uint16 CRC_TABLE[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

/* oblicza sume crc8
 * @param[in] data dane do wyznaczenia crc
 * @param[in] size rozmiar danych w [bajtach], gdy nieparzyste to pobiera mlodsza czesc z ostatniego slowa*/
uint16_t nonvolatile_t::crc8(const Uint16 * data, size_t size) const{
    return crc8_continue(data, size, 0);
}


/* oblicza sume crc8 wersja przystosowana do obliczania strumieniowo (po kawalku)
 * @param[in] data dane do wyznaczenia crc
 * @param[in] size rozmiar danych w [bajtach], gdy nieparzyste to pobiera mlodsza czesc z ostatniego slowa
 * @param[in] crc_init poprzednia wartosc */
uint16_t nonvolatile_t::crc8_continue(const Uint16 * data, size_t size, Uint16 crc_init) const{
    Uint16 ix;
    Uint16 val = crc_init;

    // funkcjonalnie taki sam kod w pythonie w narzedziach do eepromu
    // ponizej wprowadzona poprawka dla nieparzystych dlugosci
    Uint16 ix_max = size/2;
    if( 2*(size/2) < size ) //dla nieparzytej dlugosci trzeba o jedno slowo wiecej przeliczyc
        ix_max = size/2+1;   // tam jest jeszcze jeden koncowy, nieparzysty bajt w mlodszej czeci slowa

    for(ix = 0; ix<ix_max; ix++){
        val = CRC_TABLE[ (val ^ data[ix]) & 0xFF ];
        if( 2*ix+1 < size ) //to sie nie wykona dla ostatniego bajtu, gdy size jest nieparzysta
            val = CRC_TABLE[ (val ^ data[ix]>>8) & 0xFF ];
    }

    return val;
}


//-------------------------- API na zewnatrz ---------------------------

Uint16 nonvolatile_t::save( Uint16 region_index, Uint64 timeout, callback_copy_t cb) const{
    Uint64 _timeout = timeout==0? 0ULL : ((Uint64)timeout)*200ULL*1000ULL + ReadIpcTimer();
    //TODO sprawdzic czy moze dojsc do przewiniecia

    if( region_index >= this->regions_count )
        return NONVOLATILE_INVALID;

    const region_memories_t *reg = &regions[ region_index ];

    Uint16 retc = find_last_correct_copy(region_index,_timeout);
    if( (retc == NONVOLATILE_TIMEOUT) || (retc == NONVOLATILE_INVALID) )
        return retc;

    Uint16 last_copy = retc;
    //okreslenie w ktory slot kopiowac nowe dane
    Uint16 new_offset = NONVOL_COPY_OFFSET;
    if( (last_copy == NONVOLATILE_COPY_1) || (last_copy == NONVOLATILE_NO_VALID_COPY) ){
        new_offset = 0;
    }

    //skopiowanie danych do lokalnego bufora
    if( cb == NULL ){
        if( reg->data_ext.address.ptr_u16 != NULL )
            memcpy( &reg->data_int.address.ptr_u16[REGION_DATA_OFFSET/2], reg->data_ext.address.ptr_u16, reg->data_ext.size/2);
        else //niedozwolone wywolanie bez podania callbacka gdy ext=NULL
            return 1;
    }else{
        if( cb( &reg->data_int.address.ptr_u16[REGION_DATA_OFFSET/2], (reg->data_int.size-REGION_DATA_OFFSET)/2 ) != status_ok )
            return 1;
    }

    reg->data_int.address.ptr_u16[0] = crc8( &reg->data_int.address.ptr_u16[REGION_DATA_OFFSET/2], reg->data_ext.size );

    retc = this->write( region_index, new_offset, _timeout);
    if( (retc == NONVOLATILE_TIMEOUT) || (retc == NONVOLATILE_INVALID) )
         return retc;

    //poprawny zapis tresci sam sie zatwierdza, bo jest zapis tresci razem z crc (chyba nie jest wazna kolejnsoc zapisu tych elem.)

    //uniewaznienie drugiego (innego) slotu(kopii)
    retc = this->invalidate( region_index, last_copy == NONVOLATILE_COPY_0? 0:NONVOL_COPY_OFFSET, _timeout );
    if( (retc == NONVOLATILE_TIMEOUT) || (retc == NONVOLATILE_INVALID) )
          return retc;

    return NONVOLATILE_OK;
}

Uint16 nonvolatile_t::retrieve(Uint16 region_index, Uint64 timeout) const{
    Uint16 retc;
    Uint64 _timeout = timeout==0? 0ULL :((Uint64)timeout)*200ULL*1000ULL + ReadIpcTimer();
    if( region_index >= this->regions_count )
         return 1;

    const region_memories_t *reg = &regions[ region_index ];


    retc = this->find_last_correct_copy(region_index, _timeout );
    if( retc == NONVOLATILE_NO_VALID_COPY || retc == NONVOLATILE_TIMEOUT)
        return retc;

    //skopiowanie danych z lokalnego bufora
    if( reg->data_ext.address.ptr_u16!=NULL)
        memcpy( reg->data_ext.address.ptr_u16, &reg->data_int.address.ptr_u16[REGION_DATA_OFFSET/2], reg->data_ext.size/2);

    return 0;
}

/** zapis czesci informacyjnych*/
Uint16 nonvolatile_t::write_info( const struct region_info_t* info, const struct region_info_ext_t * info_ext, Uint64 timeout ) const{
    Uint64 _timeout = timeout==0? 0ULL :((Uint64)timeout)*200ULL*1000ULL + ReadIpcTimer();
    struct region_info_t info_wr = *info;
    info_wr.crc = crc8( (uint16_t*)&info_wr.data, sizeof(info_wr.data));

    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = NONVOLATILE_INFO_ADDRESS;
    xdata.total_len = NONVOLATILE_INFO_SIZE;
    xdata.data = (uint16_t*)&info_wr;

    Uint16 retc = blocking_wait_for_finished(eeprom_i2c::event_write_region, &xdata, _timeout );

    return retc;
}

/** odczyt czesci informacyjnych*/
Uint16 nonvolatile_t::read_info( struct region_info_t* info, struct region_info_ext_t * info_ext, Uint64 timeout) const{
    Uint64 _timeout = timeout==0? 0ULL :((Uint64)timeout)*200ULL*1000ULL + ReadIpcTimer();
    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = NONVOLATILE_INFO_ADDRESS;
    xdata.total_len = NONVOLATILE_INFO_SIZE;
    xdata.data = (uint16_t*)info;

    Uint16 retc = blocking_wait_for_finished(eeprom_i2c::event_read_region,  &xdata, _timeout );

    if( xdata.status != eeprom_i2c::event_region_xdata::done_ok )
        return retc;

    Uint16 crc = crc8( (uint16_t*)&info->data, sizeof(info->data));
    return ( crc != info->crc )? NONVOLATILE_INVALID : NONVOLATILE_OK;
}

//-----------------------------------------------------------------------------------------------

Uint16 nonvolatile_t::find_last_correct_copy( Uint16 region_index, Uint64 timeout )const{
    Uint16 retc;
    retc = is_copy_correct( region_index, 0, timeout );
    if( retc == NONVOLATILE_TIMEOUT )
        return retc;
    if( retc == NONVOLATILE_OK )
        return NONVOLATILE_COPY_0;

    retc = is_copy_correct( region_index, NONVOL_COPY_OFFSET, timeout );
    if( retc == NONVOLATILE_TIMEOUT )
        return retc;
    if( retc == NONVOLATILE_OK )
        return NONVOLATILE_COPY_1;

    stats.increment(stats_t::nv_no_valid_copy)
    return NONVOLATILE_NO_VALID_COPY;
}

//Uint16 nonvolatile_t::find_incorrect_copy( Uint16 region_index ){
//    if( fast_is_copy_incorrect( region_index, 0 ) ) return 0;
//    if( fast_is_copy_incorrect( region_index, NONVOL_COPY_OFFSET ) ) return 1;
//
//    if( is_copy_correct( region_index, 0 ) ) return 1;
//    if( is_copy_correct( region_index, NONVOL_COPY_OFFSET ) ) return 1;
//  UWAGA! to nie jest takie proste, trzeba brac pod uwage kolejnosc testow i wszystkie przyapdki
//}


Uint16 nonvolatile_t::is_copy_correct( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const{
    Uint16 retc;
    //tylko odczyt crc z regionu i sprawdzanie czy Hbyte!=0 albo oznaczone jako niewazne
    // aby szybko wylapac uniewazniona kopie

    retc = this->fast_is_copy_incorrect(region_index, copy_offset, timeout);
    if( retc == NONVOLATILE_INVALID )
        return retc;

    const region_memories_t *reg = &regions[ region_index ];

    retc = this->read( region_index, copy_offset, timeout);
    if( retc != NONVOLATILE_OK)
        return retc;

    Uint16 crc_calc = crc8( &reg->data_int.address.ptr_u16[REGION_DATA_OFFSET/2], reg->data_ext.size );
    Uint16 crc_in_mem = reg->data_int.address.ptr_u16[0];

    if( crc_calc == crc_in_mem ) return NONVOLATILE_OK;
    return NONVOLATILE_INVALID;
}

// ---------------- dostep do eepromu - zapis/odczyt stronami regionu
Uint16 nonvolatile_t::read( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const{
    if( region_index >= this->regions_count )
        return NONVOLATILE_INVALID;

    const region_memories_t *reg = &regions[ region_index ];

    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = (reg->data_eeprom.address.u16 + copy_offset)*EEPROM_VIRT_SCALING;
    xdata.total_len = reg->data_int.size;
    xdata.data = reg->data_int.address.ptr_u16;

    Uint16 retc = blocking_wait_for_finished( eeprom_i2c::event_read_region, &xdata, timeout );
    return retc;
}

Uint16 nonvolatile_t::write( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const{
    if( region_index >= this->regions_count )
        return NONVOLATILE_INVALID;

    const region_memories_t *reg = &regions[ region_index ];

    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = (reg->data_eeprom.address.u16 + copy_offset)*EEPROM_VIRT_SCALING;
    xdata.total_len = reg->data_int.size;
    xdata.data = reg->data_int.address.ptr_u16;

    Uint16 retc = blocking_wait_for_finished(eeprom_i2c::event_write_region, &xdata, timeout );
    return retc;
}

Uint16 nonvolatile_t::invalidate( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const{
    if( region_index >= this->regions_count )
        return 1;

    const region_memories_t *reg = &regions[ region_index ];
    uint16_t invalid_crc = NONVOLATILE_INVALID_CRC;

    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = (reg->data_eeprom.address.u16 + copy_offset)*EEPROM_VIRT_SCALING;
    xdata.total_len = 2;
    xdata.data = &invalid_crc;

    Uint16 retc = blocking_wait_for_finished(eeprom_i2c::event_write_region, &xdata, timeout );
    return retc;
}


Uint16 nonvolatile_t::fast_is_copy_incorrect( Uint16 region_index, Uint16 copy_offset, Uint64 timeout )const{
    //TODO odczytac starszy bajt (ten ktory nie zawiera CRC ale info o uniewaznieniu) CRC i sprawdzic czy jest = 0

    if( region_index >= this->regions_count )
        return NONVOLATILE_INVALID;

    const region_memories_t *reg = &regions[ region_index ];

    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = (reg->data_eeprom.address.u16 + copy_offset)*EEPROM_VIRT_SCALING;
    xdata.total_len = REGION_CRC_SIZE; //2
    xdata.data = reg->data_int.address.ptr_u16;

    Uint16 retc = blocking_wait_for_finished( eeprom_i2c::event_read_region, &xdata, timeout );
    if( retc != NONVOLATILE_OK)
        return retc;
//UWAGA! xdata.data jest modyfikowane w eeprom_i2c podczas odczytow
    Uint16 * ptr_crc_word = reg->data_int.address.ptr_u16;
 //   if( crc_word == NONVOLATILE_INVALID_CRC )
 //       return NONVOLATILE_INVALID; //gdy jest oznaczona jako niepoprawna
 // gdy = 0xDEAD to starszy bajt != 0  - obsluzy to nastepny warunek

    if( ((*ptr_crc_word) & 0xFF00 ) != 0U )
        return NONVOLATILE_INVALID; //gdy jest nieoczekiwana wartosc CRC (sprawzany bajt powinein byc = 0)

    return NONVOLATILE_UNKNOWN;//nie wiadomo czy jest poprawna
}


//ogolna funkcja blokujaca, czeka na zakonczenie transakcji
Uint16 nonvolatile_t::blocking_wait_for_finished( eeprom_i2c::event_t event, struct eeprom_i2c::event_region_xdata *xdata, Uint64 timeout) const{
    status_code_t retc;

    //etap 1. rozpoczecie operacji (moze sie nie udawac z powodu zajetosci i2c_bus)
    while(1){
        retc = eeprom.process_event(event, xdata);
        if( retc == status_ok )
            break;

        if( (timeout == 0) && (retc == err_busy) )
            continue;

        if( (timeout == 0) && (retc == err_invalid) )
            return NONVOLATILE_INVALID;

        if( timeout )
            if( ReadIpcTimer()>timeout )
                return NONVOLATILE_TIMEOUT;
    }

    //etap 2. oczekiwanie na zakonczenie operacji
    while(1){
        i2c_bus.process(); // wywolywanie cykliczne konieczne do dzialania i2c
//TODO sprawdzic czy wystarczy wywolywac powyzsze
//czy nie trzeba jeszcze czegos wywolywac od RTC albo DriverowTranzystorow

        if( xdata->status == eeprom_i2c::event_region_xdata::done_ok )
            return NONVOLATILE_OK;

        if( timeout )
              if( ReadIpcTimer()>timeout ){
                  eeprom.process_event(eeprom_i2c::event_abort);
                  return NONVOLATILE_TIMEOUT;
              }

        if( (timeout == 0) && (xdata->status == eeprom_i2c::event_region_xdata::done_error))
            return NONVOLATILE_INVALID;
    }

}
