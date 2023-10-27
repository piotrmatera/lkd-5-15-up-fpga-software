/*
 * nonvolatile_sections.cpp
 *
 *  Created on: 17 paü 2023
 *      Author: Piotr
 */


#include "nonvolatile_sections.h"
#include "State_background.h"
#include "State_master.h"
#include "State_slave.h"

extern struct ONOFF_struct ONOFF;
extern struct L_grid_meas_struct L_grid_meas;
extern class Machine_slave_class Machine_slave;


//eeprom wersja 1.03

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
#define NV_SECTION_4_EXT_SIZE 0 //oznacza ze bedzie kopiowane samodzielnie, wymaga jakiejs konwersji danych
#define NV_SECTION_4_INT_SIZE 224

//harmonics
#define NV_SECTION_5_EXT_SIZE 0
#define NV_SECTION_5_INT_SIZE 56

//meter
#define NV_SECTION_6_EXT_SIZE 0
#define NV_SECTION_6_INT_SIZE 384

//settings
#define NV_SECTION_7_EXT_SIZE 0
#define NV_SECTION_7_INT_SIZE 240


#define NV_SECTION_1_EPP_ADDR 8 //adres poczatku sekcji w eepromie
#define NV_SECTION_2_EPP_ADDR (NV_SECTION_1_EPP_ADDR+NV_SECTION_1_INT_SIZE)
#define NV_SECTION_3_EPP_ADDR (NV_SECTION_2_EPP_ADDR+NV_SECTION_2_INT_SIZE)
#define NV_SECTION_4_EPP_ADDR (NV_SECTION_3_EPP_ADDR+NV_SECTION_3_INT_SIZE)
#define NV_SECTION_5_EPP_ADDR (NV_SECTION_4_EPP_ADDR+NV_SECTION_4_INT_SIZE)
#define NV_SECTION_6_EPP_ADDR (NV_SECTION_5_EPP_ADDR+NV_SECTION_5_INT_SIZE)
#define NV_SECTION_7_EPP_ADDR (NV_SECTION_6_EPP_ADDR+NV_SECTION_6_INT_SIZE)

#define NV_SECTIONS_SIZE      (NV_SECTION_7_EPP_ADDR+NV_SECTION_7_INT_SIZE)

//obecnie zajmuje 72 bajty = 9stron po 8bajtow

//TODO inicjalizacja i uzycie sekcji eeprom.info

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
    NV_SEC_DECLARE( NULL,                         0,                     &nv_shadow_buffer[NV_SECTION_4_EPP_ADDR/2], NV_SECTION_4_EPP_ADDR, NV_SECTION_4_INT_SIZE),

// REGION 5. harmoniczne
    NV_SEC_DECLARE( NULL,                         0,                     &nv_shadow_buffer[NV_SECTION_5_EPP_ADDR/2], NV_SECTION_5_EPP_ADDR, NV_SECTION_5_INT_SIZE),

// REGION 6. meter
    NV_SEC_DECLARE( NULL,                         0,                     &nv_shadow_buffer[NV_SECTION_6_EPP_ADDR/2], NV_SECTION_6_EPP_ADDR, NV_SECTION_6_INT_SIZE),

// REGION 7. settings
    NV_SEC_DECLARE( NULL,                         0,                     &nv_shadow_buffer[NV_SECTION_7_EPP_ADDR/2], NV_SECTION_7_EPP_ADDR, NV_SECTION_7_INT_SIZE)


};

const class nonvolatile_t nonvolatile = //korzysta z globalnego obiektu eeprom
{
     .regions_count  = 3,
     .regions = _nv_regions
};


