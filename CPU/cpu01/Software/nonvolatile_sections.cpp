/*
 * nonvolatile_sections.cpp
 *
 *  Created on: 17 paü 2023
 *      Author: Piotr
 */

#include <string.h>
#include <stdlib.h>
#include "nonvolatile_sections.h"
#include "State_background.h"
#include "State_master.h"
#include "State_slave.h"
#include "nv_section_types.h"
#include "SD_card.h"



extern struct ONOFF_struct ONOFF;
extern struct L_grid_meas_struct L_grid_meas;
extern class Machine_slave_class Machine_slave;



//on-off switch
#define NV_SECTION_1_EXT_SIZE 2  /*wielkosc uzywanego pola w app (UWAGA! jesli poda sie za duzo bedzie 'mazac' po pamieci w APP)*/
#define NV_SECTION_1_INT_SIZE 8  /*wielkosc bufora (+crc {2B}; musi byc wyrownany do strony - 8 bajtow; zaokraglony w gore) wewnetrznego na kopie tymczasowa*/

//Lgrid_previous
#define NV_SECTION_2_EXT_SIZE 40
#define NV_SECTION_2_INT_SIZE 48

//error_retry
#define NV_SECTION_3_EXT_SIZE 2
#define NV_SECTION_3_INT_SIZE 8


//calibration
#define NV_SECTION_4_EXT_SIZE 112
#define NV_SECTION_4_INT_SIZE 224

//harmonics
#define NV_SECTION_5_EXT_SIZE 27
#define NV_SECTION_5_INT_SIZE 56

//meter
#define NV_SECTION_6_EXT_SIZE 192
#define NV_SECTION_6_INT_SIZE 384

//settings
#define NV_SECTION_7_EXT_SIZE 144
#define NV_SECTION_7_INT_SIZE 288 //UWAGA nonvolatile odczytuje cala sekcje


#define NV_SECTION_1_EPP_ADDR 8 //adres poczatku sekcji w eepromie
#define NV_SECTION_2_EPP_ADDR (NV_SECTION_1_EPP_ADDR+NV_SECTION_1_INT_SIZE)
#define NV_SECTION_3_EPP_ADDR (NV_SECTION_2_EPP_ADDR+NV_SECTION_2_INT_SIZE)
#define NV_SECTION_4_EPP_ADDR (NV_SECTION_3_EPP_ADDR+NV_SECTION_3_INT_SIZE)
#define NV_SECTION_5_EPP_ADDR (NV_SECTION_4_EPP_ADDR+NV_SECTION_4_INT_SIZE)
#define NV_SECTION_6_EPP_ADDR (NV_SECTION_5_EPP_ADDR+NV_SECTION_5_INT_SIZE)
#define NV_SECTION_7_EPP_ADDR (NV_SECTION_6_EPP_ADDR+NV_SECTION_6_INT_SIZE)

#define NV_SECTIONS_SIZE      (NV_SECTION_7_EPP_ADDR+NV_SECTION_7_INT_SIZE)



uint16_t nv_shadow_buffer[ NV_SECTIONS_SIZE ];  //jako bufor tymczasowy do dzialania nonvolatile

/* deklarowanie sekcji w eepromie
 * @param[in] _sw_data_ptr_ wskaznik do danych w FW (moze byc NULL, jesli samodzielnie kopiowane dane FW<->shadow_buffer)
 * @param[in] _sw_data_size_ rozmiar danych w FW, moz ebyc zero gdy kopiowanie samdozielnie
 * @param[in] _shadow_buffer_ wskaznik do wystarczajacej pamieci na odczytanie sekcji z eepromu
 * @param[in] _eeprom_address_ adres poczatku sekcji w eepromie
 * @param[in] _eeprom_sec_size rozmiar sekcji w eepromie (incl. crc, zaokraglone do 8 bajtow)
 * */
#define NV_SEC_DECLARE( _sw_data_ptr_, _sw_data_size_, _shadow_buffer_, _eeprom_address_, _eeprom_sec_size_ )\
        {  /*ext*/{ .address.ptr_u16 = (Uint16 *)_sw_data_ptr_, .size = _sw_data_size_ },\
               /*int*/{ .address.ptr_u16 = _shadow_buffer_, .size = _eeprom_sec_size_ },\
               /*epp*/{ .address.u16 = _eeprom_address_, .size = _eeprom_sec_size_ /*NU?*/  }}
              //TODO upewnic sie ze:
              //  ext.size jest podawany w bajtach i bez crc
              //  int.size calego regionu
              //  eeprom.size jest nieuzywane


const region_memories_t _nv_regions[] = {
// REGION 1. ONOFF switch
    NV_SEC_DECLARE( &ONOFF.ONOFF_FLASH,           NV_SECTION_1_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_1_EPP_ADDR/2], NV_SECTION_1_EPP_ADDR, NV_SECTION_1_INT_SIZE),

// REGION 2. L_grid_previous
    NV_SEC_DECLARE( &L_grid_meas.L_grid_previous, NV_SECTION_2_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_2_EPP_ADDR/2], NV_SECTION_2_EPP_ADDR, NV_SECTION_2_INT_SIZE),


// REGION 3. error_retry
    NV_SEC_DECLARE( &Machine_slave.error_retry,   NV_SECTION_3_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_3_EPP_ADDR/2], NV_SECTION_3_EPP_ADDR, NV_SECTION_3_INT_SIZE),

// REGION 4. calibration
    NV_SEC_DECLARE( NULL,                         NV_SECTION_4_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_4_EPP_ADDR/2], NV_SECTION_4_EPP_ADDR, NV_SECTION_4_INT_SIZE),

// REGION 5. harmoniczne
    NV_SEC_DECLARE( NULL,                         NV_SECTION_5_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_5_EPP_ADDR/2], NV_SECTION_5_EPP_ADDR, NV_SECTION_5_INT_SIZE),

// REGION 6. meter
    NV_SEC_DECLARE( NULL,                         NV_SECTION_6_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_6_EPP_ADDR/2], NV_SECTION_6_EPP_ADDR, NV_SECTION_6_INT_SIZE),

// REGION 7. settings
    NV_SEC_DECLARE( NULL,                         NV_SECTION_7_EXT_SIZE, &nv_shadow_buffer[NV_SECTION_7_EPP_ADDR/2], NV_SECTION_7_EPP_ADDR, NV_SECTION_7_INT_SIZE)


};

const class nonvolatile_t nonvolatile = //korzysta z globalnego obiektu eeprom
{
     .regions_count  = sizeof(_nv_regions)/sizeof(_nv_regions[0]),
     .regions = _nv_regions
};

nv_data_t nv_data;


Uint16 nv_data_t::read( section_type_t section ){
    switch( section ){

    case sec_settings:   return read_settings();
    case sec_H_settings: return read_H_settings();
    case sec_calibration_data: return read_calibration_data();
    case sec_meter_data:      return read_meter_data();
    case sec_CT_characteristic:
                         return read_CT_characteristic();
    default:
        return FR_INVALID_PARAMETER;
    }
}

Uint16 nv_data_t::save( section_type_t section ){
    switch( section ){

    case sec_settings:   return save_settings();
    case sec_H_settings: return save_H_settings();
    case sec_calibration_data: return save_calibration_data();
    case sec_meter_data:      return save_meter_data();

    case sec_CT_characteristic:
    default:
        return FR_INVALID_PARAMETER;
    }
}

//uzywane wewnetrznie, na zewnatrz uzywane section_type_t

#define NV_REGION_CALIB 4 //numeracja regionow od 1.
#define NV_REGION_HARMON 5
#define NV_REGION_METER 6
#define NV_REGION_SETTINGS 7

#define NV_REGION_READ_SETTING_TIMEOUT 0
#define NV_REGION_SAVE_SETTING_TIMEOUT 0
#define NV_REGION_READ_HARMON_TIMEOUT 0
#define NV_REGION_SAVE_HARMON_TIMEOUT 0
#define NV_REGION_READ_METER_TIMEOUT 0
#define NV_REGION_SAVE_METER_TIMEOUT 0
#define NV_REGION_READ_CALIB_TIMEOUT 0
#define NV_REGION_SAVE_CALIB_TIMEOUT 0

#define NV_CT_CHAR_FILE_ADDRESS 0x4000
#define NV_CT_CHAR_FILE_HDR_TIMEOUT 0
#define NV_CT_CHAR_FILE_DATA_TIMEOUT 0

#define NV_CT_CHAR_SIZE (7*60*4) //w bajtach




Uint16 nv_data_t::read_settings(){
    Uint16 retc = nonvolatile.retrieve(NV_REGION_SETTINGS, NV_REGION_READ_SETTING_TIMEOUT);
    if( retc!= 0 )
        return FR_INVALID_PARAMETER;

    //lokalizacja odczytanej kopii, UWAGA pierwsze slowo to CRC
    Uint16 * shadow_buffer = nonvolatile.regions[ NV_REGION_SETTINGS-1 ].data_int.address.ptr_u16;
    Uint16   data_buffer_size = nonvolatile.regions[ NV_REGION_SETTINGS-1 ].data_ext.size/2; // /2 bo w bajtach
    //mozna odcyztywac z data_buffer_size/2 slow



    Uint16 items = data_buffer_size/sizeof(struct settings_item);
    struct settings_item * items_table = (struct settings_item *) &shadow_buffer[1];

    for(int i=0; i<items; i++){
        float value = items_table[i].value;

        switch( items_table[i].type){
        case SETTINGS_STATIC_Q_COMPENSATION_A: SD_card.settings.control.Q_set.a = value; break;
        case SETTINGS_STATIC_Q_COMPENSATION_B: SD_card.settings.control.Q_set.b = value; break;
        case SETTINGS_STATIC_Q_COMPENSATION_C: SD_card.settings.control.Q_set.c = value; break;
        case SETTINGS_ENABLE_Q_COMPENSATION_A: SD_card.settings.control.flags.bit.enable_Q_comp_a = value; break;
        case SETTINGS_ENABLE_Q_COMPENSATION_B: SD_card.settings.control.flags.bit.enable_Q_comp_b = value; break;
        case SETTINGS_ENABLE_Q_COMPENSATION_C: SD_card.settings.control.flags.bit.enable_Q_comp_c = value; break;
        case SETTINGS_ENABLE_P_SYMMETRIZATION: SD_card.settings.control.flags.bit.enable_P_sym = value; break;
        case SETTINGS_ENABLE_H_COMPENSATION:   SD_card.settings.control.flags.bit.enable_H_comp = value; break;
        case SETTINGS_VERSION_P_SYMMETRIZATION: SD_card.settings.control.flags.bit.version_P_sym = value; break;
        case SETTINGS_VERSION_Q_COMPENSATION_A: SD_card.settings.control.flags.bit.version_Q_comp_a = value; break;
        case SETTINGS_VERSION_Q_COMPENSATION_B: SD_card.settings.control.flags.bit.version_Q_comp_b = value; break;
        case SETTINGS_VERSION_Q_COMPENSATION_C: SD_card.settings.control.flags.bit.version_Q_comp_c = value; break;
        case SETTINGS_TANGENS_RANGE_A_HIGH: SD_card.settings.control.tangens_range[0].a = value; break;
        case SETTINGS_TANGENS_RANGE_B_HIGH: SD_card.settings.control.tangens_range[0].b = value; break;
        case SETTINGS_TANGENS_RANGE_C_HIGH: SD_card.settings.control.tangens_range[0].c = value; break;
        case SETTINGS_TANGENS_RANGE_A_LOW: SD_card.settings.control.tangens_range[1].a = value; break;
        case SETTINGS_TANGENS_RANGE_B_LOW: SD_card.settings.control.tangens_range[1].b = value; break;
        case SETTINGS_TANGENS_RANGE_C_LOW: SD_card.settings.control.tangens_range[1].c = value; break;
        case SETTINGS_BAUDRATE: SD_card.settings.Baudrate = value; break;
        case SETTINGS_EXT_SERVER_ID: SD_card.settings.modbus_ext_server_id = value; break;
        case SETTINGS_WIFI_ONOFF: SD_card.settings.wifi_on = value==0? 0 : 1; break;
        case SETTINGS_C_DC: SD_card.settings.C_dc = value; break;
        case SETTINGS_L: SD_card.settings.L_conv = value; break;
        case SETTINGS_C: SD_card.settings.C_conv = value; break;
        case SETTINGS_I_LIM: SD_card.settings.I_lim = value; break;
        case SETTINGS_NUMBER_OF_SLAVES: SD_card.settings.number_of_slaves = value; break;
        case SETTINGS_NO_NEUTRAL: SD_card.settings.no_neutral = value; break;
        default:
            break;
        }
    }

    Uint16 fresult = FR_INVALID_PARAMETER;
    if(SD_card.settings.modbus_ext_server_id <1 || SD_card.settings.modbus_ext_server_id>230 )
            SD_card.settings.modbus_ext_server_id = 1;
    if(SD_card.settings.number_of_slaves < 0.0f || SD_card.settings.number_of_slaves > 4.0f) return fresult;
    if(SD_card.settings.control.tangens_range[0].a < -1.0f || SD_card.settings.control.tangens_range[0].a > 1.0f) return fresult;
    if(SD_card.settings.control.tangens_range[0].b < -1.0f || SD_card.settings.control.tangens_range[0].b > 1.0f) return fresult;
    if(SD_card.settings.control.tangens_range[0].c < -1.0f || SD_card.settings.control.tangens_range[0].c > 1.0f) return fresult;
    if(SD_card.settings.control.tangens_range[1].a < -1.0f || SD_card.settings.control.tangens_range[1].a > 1.0f) return fresult;
    if(SD_card.settings.control.tangens_range[1].b < -1.0f || SD_card.settings.control.tangens_range[1].b > 1.0f) return fresult;
    if(SD_card.settings.control.tangens_range[1].c < -1.0f || SD_card.settings.control.tangens_range[1].c > 1.0f) return fresult;
    if(SD_card.settings.C_dc < 0.25e-3 || SD_card.settings.C_dc > 5e-3) return fresult;
    if(SD_card.settings.L_conv < 100e-6 || SD_card.settings.L_conv > 2.5e-3) return fresult;
    if(SD_card.settings.C_conv < 5e-6 || SD_card.settings.C_conv > 200e-6) return fresult;
    if(SD_card.settings.I_lim < 8.0f || SD_card.settings.I_lim > 40.0f) return fresult;

    SD_card.settings.available = 1;

    return FR_OK;
}

/** kopiowanie ze struktur prorgamu do bufora
 * @param[in] shadow_buffer bufor do ktorego kopiowac, rozpoczac od [1], [0]-rezerwacja na CRC
 * @param[in] buffer_size ilosc slow w buforze do wykorzystania
 */
static Uint16 cb_nv_save_settings( Uint16* shadow_buffer, Uint16 buffer_size ){
    Uint16 items = buffer_size/sizeof(struct nv_data_t::settings_item);
    struct nv_data_t::settings_item * items_table = (struct nv_data_t::settings_item *) &shadow_buffer[1];

    float value = 0;

    for(int i=0; i<nv_data_t::SETTINGS_MAX; i++){
        //gdy brak miejsca w sekcji to przerwac

        if (i >= items) break; //TODO sprawdzic czy dobry warunek, czy nie za lagodny

        switch( i ){
        case nv_data_t::SETTINGS_STATIC_Q_COMPENSATION_A: value = SD_card.settings.control.Q_set.a; break;
        case nv_data_t::SETTINGS_STATIC_Q_COMPENSATION_B: value = SD_card.settings.control.Q_set.b; break;
        case nv_data_t::SETTINGS_STATIC_Q_COMPENSATION_C: value = SD_card.settings.control.Q_set.c; break;
        case nv_data_t::SETTINGS_ENABLE_Q_COMPENSATION_A: value = SD_card.settings.control.flags.bit.enable_Q_comp_a; break;
        case nv_data_t::SETTINGS_ENABLE_Q_COMPENSATION_B: value = SD_card.settings.control.flags.bit.enable_Q_comp_b; break;
        case nv_data_t::SETTINGS_ENABLE_Q_COMPENSATION_C: value = SD_card.settings.control.flags.bit.enable_Q_comp_c; break;
        case nv_data_t::SETTINGS_ENABLE_P_SYMMETRIZATION: value = SD_card.settings.control.flags.bit.enable_P_sym; break;
        case nv_data_t::SETTINGS_ENABLE_H_COMPENSATION:   value = SD_card.settings.control.flags.bit.enable_H_comp; break;
        case nv_data_t::SETTINGS_VERSION_P_SYMMETRIZATION: value = SD_card.settings.control.flags.bit.version_P_sym; break;
        case nv_data_t::SETTINGS_VERSION_Q_COMPENSATION_A: value = SD_card.settings.control.flags.bit.version_Q_comp_a; break;
        case nv_data_t::SETTINGS_VERSION_Q_COMPENSATION_B: value = SD_card.settings.control.flags.bit.version_Q_comp_b; break;
        case nv_data_t::SETTINGS_VERSION_Q_COMPENSATION_C: value = SD_card.settings.control.flags.bit.version_Q_comp_c; break;
        case nv_data_t::SETTINGS_TANGENS_RANGE_A_HIGH: value = SD_card.settings.control.tangens_range[0].a; break;
        case nv_data_t::SETTINGS_TANGENS_RANGE_B_HIGH: value = SD_card.settings.control.tangens_range[0].b; break;
        case nv_data_t::SETTINGS_TANGENS_RANGE_C_HIGH: value = SD_card.settings.control.tangens_range[0].c; break;
        case nv_data_t::SETTINGS_TANGENS_RANGE_A_LOW:  value = SD_card.settings.control.tangens_range[1].a; break;
        case nv_data_t::SETTINGS_TANGENS_RANGE_B_LOW:  value = SD_card.settings.control.tangens_range[1].b; break;
        case nv_data_t::SETTINGS_TANGENS_RANGE_C_LOW:  value = SD_card.settings.control.tangens_range[1].c; break;
        case nv_data_t::SETTINGS_BAUDRATE:             value = SD_card.settings.Baudrate; break;
        case nv_data_t::SETTINGS_EXT_SERVER_ID: value = SD_card.settings.modbus_ext_server_id; break;
        case nv_data_t::SETTINGS_WIFI_ONOFF:   value = SD_card.settings.wifi_on; break;
        case nv_data_t::SETTINGS_C_DC:         value = SD_card.settings.C_dc; break;
        case nv_data_t::SETTINGS_L:            value = SD_card.settings.L_conv; break;
        case nv_data_t::SETTINGS_C:            value = SD_card.settings.C_conv; break;
        case nv_data_t::SETTINGS_I_LIM:        value = SD_card.settings.I_lim; break;
        case nv_data_t::SETTINGS_NUMBER_OF_SLAVES: value = SD_card.settings.number_of_slaves; break;
        case nv_data_t::SETTINGS_NO_NEUTRAL:   value = SD_card.settings.no_neutral; break;
        default:
            break;
        }

        items_table[i].value = value;
        items_table[i].type = i;
    }

    return status_ok;

}

Uint16 nv_data_t::save_settings(){
    Uint16 retc = nonvolatile.save(NV_REGION_SETTINGS, NV_REGION_SAVE_SETTING_TIMEOUT, cb_nv_save_settings);
    return ( retc!= 0 )? FR_INVALID_PARAMETER : FR_OK;
}




Uint16 nv_data_t::read_H_settings(){
   Uint16 retc = nonvolatile.retrieve(NV_REGION_HARMON, NV_REGION_READ_HARMON_TIMEOUT);
   if( retc!= 0 )
       return FR_INVALID_PARAMETER;

   //lokalizacja odczytanej kopii, UWAGA pierwsze slowo to CRC
   Uint16 * shadow_buffer = nonvolatile.regions[ NV_REGION_HARMON-1 ].data_int.address.ptr_u16;
   Uint16   data_buffer_size = nonvolatile.regions[ NV_REGION_HARMON-1 ].data_ext.size/2; // /2 bo w bajtach
   //mozna odcyztywac z data_buffer_size/2 slow

   Uint16 items_max = data_buffer_size/sizeof(struct harmon_item);
   struct harmon_item * items_table = (struct harmon_item *) &shadow_buffer[1];

   memset(&SD_card.harmonics, 0, sizeof(SD_card.harmonics));

   struct harmon_item h_item;

   Uint16 ix = 0;
    for(int i=0; i<items_max; i++){
      h_item = items_table[ix++];
      if( ix<=25 ){
          SD_card.harmonics.on_off_odd_a[ i ] = 0;
          SD_card.harmonics.on_off_odd_b[ i ] = 0;
          SD_card.harmonics.on_off_odd_c[ i ] = 0;
      }else if( ix<=25+2 ){
          SD_card.harmonics.on_off_even_a[ i ] = 0;
          SD_card.harmonics.on_off_even_b[ i ] = 0;
          SD_card.harmonics.on_off_even_c[ i ] = 0;
      }
    }

   ix = 0;
   for(int i=0; i<items_max; i++){
       h_item = items_table[ix++];
       if( ix<=25 ){
           SD_card.harmonics.on_off_odd_a[ i ] = h_item.a;
           SD_card.harmonics.on_off_odd_b[ i ] = h_item.b;
           SD_card.harmonics.on_off_odd_c[ i ] = h_item.c;
       }else if( ix<=25+2 ){
           SD_card.harmonics.on_off_even_a[ i ] = h_item.a;
           SD_card.harmonics.on_off_even_b[ i ] = h_item.b;
           SD_card.harmonics.on_off_even_c[ i ] = h_item.c;
       }
   }
   SD_card.harmonics.available = 1;
   return FR_OK;
}

static Uint16 cb_nv_save_harmon( Uint16* shadow_buffer, Uint16 buffer_size ){
    Uint16 items_max = buffer_size/sizeof(struct nv_data_t::harmon_item);
    struct nv_data_t::harmon_item * items_table = (struct nv_data_t::harmon_item *) &shadow_buffer[1];

    struct nv_data_t::harmon_item h_item;

    Uint16 ix = 0;
    for(int i=0; i<25; i++){
        if( ix >= items_max ) break;//TODO sprawdzic warunek
        h_item.a = SD_card.harmonics.on_off_odd_a[ i ];
        h_item.b = SD_card.harmonics.on_off_odd_b[ i ];
        h_item.c = SD_card.harmonics.on_off_odd_c[ i ];
        items_table[ix++] = h_item;
    }

    for(int i=0; i<2; i++){
        if( ix >= items_max ) break;//TODO sprawdzic warunek
        h_item.a = SD_card.harmonics.on_off_even_a[ i ];
        h_item.b = SD_card.harmonics.on_off_even_b[ i ];
        h_item.c = SD_card.harmonics.on_off_even_c[ i ];
        items_table[ix++] = h_item;
    }
    return status_ok;
}


Uint16 nv_data_t::save_H_settings(){
    Uint16 retc = nonvolatile.save(NV_REGION_HARMON, NV_REGION_SAVE_HARMON_TIMEOUT, cb_nv_save_harmon);
    return ( retc!= 0 )? FR_INVALID_PARAMETER : FR_OK;
}


Uint16 nv_data_t::read_meter_data(){
    Uint16 retc = nonvolatile.retrieve(NV_REGION_METER, NV_REGION_READ_METER_TIMEOUT);
    if( retc!= 0 )
      return FR_INVALID_PARAMETER;

    //lokalizacja odczytanej kopii, UWAGA pierwsze slowo to CRC
    Uint16 * shadow_buffer = nonvolatile.regions[ NV_REGION_METER-1 ].data_int.address.ptr_u16;
    Uint16   data_buffer_size = nonvolatile.regions[ NV_REGION_METER-1 ].data_ext.size/2; // /2 bo w bajtach

    if( data_buffer_size != sizeof(SD_card.meter.Energy_meter))
        return FR_INVALID_PARAMETER;

    memcpy( (Uint16 *)&SD_card.meter.Energy_meter, shadow_buffer, sizeof(SD_card.meter.Energy_meter));

    SD_card.meter.available = 1;

    return FR_OK;
}

static Uint16 cb_nv_save_meter( Uint16* shadow_buffer, Uint16 buffer_size ){
    void * items_data = (void*)&shadow_buffer[1];

    Uint16 total_len = sizeof(SD_card.meter.Energy_meter.P_p);
    total_len += sizeof(SD_card.meter.Energy_meter.P_n);
    total_len += sizeof(SD_card.meter.Energy_meter.QI);
    total_len += sizeof(SD_card.meter.Energy_meter.QII);
    total_len += sizeof(SD_card.meter.Energy_meter.QIII);
    total_len += sizeof(SD_card.meter.Energy_meter.QIV);
    total_len += sizeof(SD_card.meter.Energy_meter.sum);

    if( buffer_size != total_len )
        return err_invalid;

    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.P_p,  (Uint16 *)&Energy_meter.upper.P_p,  sizeof(SD_card.meter.Energy_meter.P_p));
    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.P_n,  (Uint16 *)&Energy_meter.upper.P_n,  sizeof(SD_card.meter.Energy_meter.P_n));
    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.QI,   (Uint16 *)&Energy_meter.upper.QI,   sizeof(SD_card.meter.Energy_meter.QI));
    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.QII,  (Uint16 *)&Energy_meter.upper.QII,  sizeof(SD_card.meter.Energy_meter.QII));
    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.QIII, (Uint16 *)&Energy_meter.upper.QIII, sizeof(SD_card.meter.Energy_meter.QIII));
    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.QIV,  (Uint16 *)&Energy_meter.upper.QIV,  sizeof(SD_card.meter.Energy_meter.QIV));
    DINT_copy_CPUasm((Uint16 *)&SD_card.meter.Energy_meter.sum,  (Uint16 *)&Energy_meter.upper.sum,  sizeof(SD_card.meter.Energy_meter.sum));

    memcpy( items_data, (Uint16 *)&SD_card.meter.Energy_meter, sizeof(SD_card.meter.Energy_meter));

    return status_ok;
}

Uint16 nv_data_t::save_meter_data(){
    Uint16 retc = nonvolatile.save(NV_REGION_METER, NV_REGION_SAVE_METER_TIMEOUT, cb_nv_save_meter);
    return ( retc!= 0 )? FR_INVALID_PARAMETER : FR_OK;
}

Uint16 nv_data_t::read_calibration_data(){
   Uint16 retc = nonvolatile.retrieve(NV_REGION_CALIB, NV_REGION_READ_CALIB_TIMEOUT);
   if( retc!= 0 )
       return FR_INVALID_PARAMETER;

   //lokalizacja odczytanej kopii, UWAGA pierwsze slowo to CRC
   Uint16 * shadow_buffer = nonvolatile.regions[ NV_REGION_METER-1 ].data_int.address.ptr_u16;
   Uint16   data_buffer_size = nonvolatile.regions[ NV_REGION_METER-1 ].data_ext.size/2; // /2 bo w bajtach

   size_t first_part = sizeof(struct Measurements_ACDC_gain_offset_struct);
   if( data_buffer_size != first_part + sizeof(struct Measurements_ACDC_gain_offset_struct))
       return FR_INVALID_PARAMETER;

   memcpy( (Uint16 *)&SD_card.calibration.Meas_ACDC_gain, shadow_buffer, first_part);

   memcpy( (Uint16 *)&SD_card.calibration.Meas_ACDC_offset, &shadow_buffer[ first_part ], sizeof(struct Measurements_ACDC_gain_offset_struct));
   SD_card.calibration.available = 1;

   return FR_OK;

}

static Uint16 cb_nv_save_calibration_data( Uint16* shadow_buffer, Uint16 buffer_size ){
    Uint16 * items_table = &shadow_buffer[1];

    size_t first_part = sizeof(struct Measurements_ACDC_gain_offset_struct);
    if( buffer_size != first_part + sizeof(struct Measurements_ACDC_gain_offset_struct))
        return err_invalid;

    memcpy( items_table, (Uint16 *)&SD_card.calibration.Meas_ACDC_gain, first_part);

    memcpy( &items_table[ first_part ], (Uint16 *)&SD_card.calibration.Meas_ACDC_offset, sizeof(struct Measurements_ACDC_gain_offset_struct));
    return status_ok;
}

Uint16 nv_data_t::save_calibration_data(){
    Uint16 retc = nonvolatile.save(NV_REGION_CALIB, NV_REGION_SAVE_CALIB_TIMEOUT, cb_nv_save_calibration_data);
    return ( retc!= 0 )? FR_INVALID_PARAMETER : FR_OK;
}

static int compare_float (const void * a, const void * b)
{
    if ( *(float*)a <  *(float*)b ) return -1;
    else if ( *(float*)a >  *(float*)b ) return 1;
    else return 0;
}

Uint16 nv_data_t::read_CT_characteristic(){
    struct crc_n_len_s{
        Uint16 crc;
        Uint16 len;
    } crc_n_len;

    struct eeprom_i2c::event_region_xdata xdata;
    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = NV_CT_CHAR_FILE_ADDRESS;
    xdata.total_len = 4; //tylko crc i dlugosc
    xdata.data = (uint16_t*)&crc_n_len;

    Uint16 retc = nonvolatile.blocking_wait_for_finished( eeprom_i2c::event_read_region, &xdata, NV_CT_CHAR_FILE_HDR_TIMEOUT );
    if( retc != 0 )
        return FR_INVALID_PARAMETER;

    if( crc_n_len.len > NV_CT_CHAR_SIZE )
        return FR_INVALID_PARAMETER;


    xdata.status = eeprom_i2c::event_region_xdata::idle;
    xdata.start = NV_CT_CHAR_FILE_ADDRESS + 4;
    xdata.total_len = crc_n_len.len;
    xdata.data = (uint16_t*)&SD_card.CT_char.set_current; //UWAGA! zalozenie ze kolejne tablice sa po kolei w strukturze CT_char

    retc = nonvolatile.blocking_wait_for_finished( eeprom_i2c::event_read_region, &xdata, NV_CT_CHAR_FILE_DATA_TIMEOUT );
    if( retc != 0 )
        return FR_INVALID_PARAMETER;

    //sprawdzenie CRC

    Uint16 crc_calc = nonvolatile.crc8( &crc_n_len.len, 2);
    crc_calc = nonvolatile.crc8_continue( (uint16_t*)&SD_card.CT_char.set_current, crc_n_len.len, crc_calc );

    if( crc_calc != crc_n_len.crc )
        return FR_INVALID_PARAMETER;


    SD_card.CT_char.number_of_elements = crc_n_len.len/sizeof(float)/7;

    float sort_buffer[CT_CHARACTERISTIC_POINTS][7];
    for(Uint16 i = 0; i < SD_card.CT_char.number_of_elements;i++)
    {
        sort_buffer[i][0] = SD_card.CT_char.set_current[i];
        sort_buffer[i][1] = SD_card.CT_char.CT_ratio_a[i];
        sort_buffer[i][2] = SD_card.CT_char.CT_ratio_b[i];
        sort_buffer[i][3] = SD_card.CT_char.CT_ratio_c[i];
        sort_buffer[i][4] = SD_card.CT_char.phase_a[i];
        sort_buffer[i][5] = SD_card.CT_char.phase_b[i];
        sort_buffer[i][6] = SD_card.CT_char.phase_c[i];
    }

    qsort(sort_buffer, SD_card.CT_char.number_of_elements, 7 * sizeof(float), compare_float);

    for(Uint16 i = 0; i < SD_card.CT_char.number_of_elements;i++)
    {
        SD_card.CT_char.set_current[i] = sort_buffer[i][0];
        SD_card.CT_char.CT_ratio_a[i] = sort_buffer[i][1];
        SD_card.CT_char.CT_ratio_b[i] = sort_buffer[i][2];
        SD_card.CT_char.CT_ratio_c[i] = sort_buffer[i][3];
        SD_card.CT_char.phase_a[i] = sort_buffer[i][4];
        SD_card.CT_char.phase_b[i] = sort_buffer[i][5];
        SD_card.CT_char.phase_c[i] = sort_buffer[i][6];
    }


    SD_card.CT_char.available = 1;

    return FR_OK;


}




