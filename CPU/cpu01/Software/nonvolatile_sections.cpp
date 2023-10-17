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

#define NV_SECTION_0_EXT_SIZE 2  //wielkosc uzywanego pola w app (UWAGA! jesli poda sie za duzo bedzie 'mazac' po pamieci w APP)
#define NV_SECTION_0_INT_SIZE 8  //wielkosc bufora (+crc {2B}; musi byc wyrownany do strony - 8 bajtow; zaokraglony w gore) wewnetrznego na kopie tymczasowa

#define NV_SECTION_1_EXT_SIZE 40
#define NV_SECTION_1_INT_SIZE 48

#define NV_SECTION_2_EXT_SIZE 2
#define NV_SECTION_2_INT_SIZE 8

#define NV_SECTION_0_EPP_ADDR 8 //adres poczatku sekcji w eepromie
#define NV_SECTION_1_EPP_ADDR (NV_SECTION_0_EPP_ADDR+NV_SECTION_0_INT_SIZE)
#define NV_SECTION_2_EPP_ADDR (NV_SECTION_1_EPP_ADDR+NV_SECTION_1_INT_SIZE)
#define NV_SECTIONS_SIZE      (NV_SECTION_2_EPP_ADDR+NV_SECTION_2_INT_SIZE)

//obecnie zajmuje 72 bajty = 9stron po 8bajtow

//TODO inicjalizacja i uzycie sekcji eeprom.info

uint16_t nv_shadow_buffer[ NV_SECTIONS_SIZE ];  //jako bufor tymczasowy do dzialania nonvolatile

const region_memories_t _nv_regions[] = {
// REGION 1. ONOFF switch

    {  /*ext*/{ .address.ptr_u16 = (Uint16 *)&ONOFF.ONOFF_FLASH, .size = NV_SECTION_0_EXT_SIZE },
       /*int*/{ .address.ptr_u16 = &nv_shadow_buffer[NV_SECTION_0_EPP_ADDR/2], .size = NV_SECTION_0_INT_SIZE },
       /*epp*/{ .address.u16 = NV_SECTION_0_EPP_ADDR, .size = NV_SECTION_0_INT_SIZE /*NU?*/  }},
      //TODO upewnic sie ze:
      //  ext.size jest podawany w bajtach i bez crc
      //  int.size calego regionu
      //  eeprom.size jest nieuzywane

// REGION 2. L_grid_previous
    {  { .address.ptr_u16 = (Uint16 *)&L_grid_meas.L_grid_previous, .size = NV_SECTION_1_EXT_SIZE },
       { .address.ptr_u16 = &nv_shadow_buffer[NV_SECTION_1_EPP_ADDR/2], .size = NV_SECTION_1_INT_SIZE },
       { .address.u16 = NV_SECTION_1_EPP_ADDR, .size = NV_SECTION_1_INT_SIZE /*NU?*/  }},

// REGION 3. error_retry
    {  { .address.ptr_u16 = (Uint16 *)&Machine_slave.error_retry, .size = NV_SECTION_2_EXT_SIZE },
       { .address.ptr_u16 = &nv_shadow_buffer[NV_SECTION_1_EPP_ADDR/2], .size = NV_SECTION_2_INT_SIZE },
       { .address.u16 = NV_SECTION_2_EPP_ADDR, .size = NV_SECTION_2_INT_SIZE /*NU?*/  }}

// REGION 4. liczniki czasu pracy
      //{ .address.ptr_u16 = (Uint16 *)&work_counters, .size = 6 /*pakowanie 24bity?*/ }, { .address.ptr_u16 = &nv_shadow_buffer[56/2], .size = 8 },  { .address.u16 = 56, .size = 8 /*NU?*/  }}

};

const class nonvolatile_t nonvolatile = //korzysta z globalnego obiektu eeprom
{
     .regions_count  = 3,
     .regions = _nv_regions
};


