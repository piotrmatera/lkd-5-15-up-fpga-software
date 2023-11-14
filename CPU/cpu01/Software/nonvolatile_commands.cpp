/*
 * nonvolatile_commands.cpp
 *
 *  Created on: 14 lis 2023
 *      Author: Piotr
 */
#include "nonvolatile_commands.h"
#include "nv_section_types.h"
#include "version.h"
#include "State_master.h"
#include "State_slave.h"
#include "nonvolatile_sections.h"
#include "SD_card.h"
#include "Modbus_Converter_memory.h"





nonvolatile_commands_t nonvolatile_commands;

nonvolatile_commands_t::nonvolatile_commands_t(){
    return;
}

const section_type_t section_type[]={
//   sec_CT_characteristic, TODO dopisac zapis dla sd_card i nv_sections
   sec_settings,
   sec_H_settings,
   sec_calibration_data,
   sec_meter_data
};

void nonvolatile_commands_t::result( Uint16 res, Uint16 ext_res ){
    Modbus_Converter.holding_registers.result = res;
    Modbus_Converter.holding_registers.ext_result = ext_res;
}

status_code_t nonvolatile_commands_t::process( modbus_holding_commands_t cmd, Uint16 arg1, Uint16 arg2 ){
    (void)arg1;
    (void)arg2;

    status_code_t retc = err_invalid;
    Uint16 ext_res = 0xFF;

    this->result( retc, ext_res );

    if( Machine_master.state != Machine_master_class::state_idle
     || Machine_slave.state  !=  Machine_slave_class::state_idle ){
        retc = err_invalid;
        goto l_error;
    }

    switch( cmd ){
    case CMD_COPY_EEPROM_TO_SDCARD:
        for(Uint16 i=0; i<sizeof(section_type)/sizeof(section_type[0]); ++i){
            section_type_t section = section_type[i];
            ext_res = 2*i;
            if( nv_data.read( section ) != 0 )
                goto l_error;
            ext_res = 2*i+1;
            if( SD_card.save( section ) != 0 );
                goto l_error;
        }
        retc = status_ok;
        break;
    case CMD_COPY_SDCARD_TO_EEPROM:
        for(Uint16 i=0; i<sizeof(section_type)/sizeof(section_type[0]); ++i){
            section_type_t section = section_type[i];
            ext_res = 2*i;
            if( SD_card.read( section ) != 0 )
                goto l_error;
            ext_res = 2*i+1;
            if( nv_data.save( section ) != 0 );
                goto l_error;
        }
        retc = status_ok;
        break;
    case CMD_FORMAT_INFO:
    {
        struct region_info_t info = { .crc = 0,
                                      .data={
                                             .eeprom_version = NV_EEPROM_VERSION,
                                             .pcb_version = BOARD_ID,
                                             .eeprom_info_address = NONVOL_COPY_OFFSET}
        };

        struct region_info_ext_t info_ext = {.res = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}};
        retc = (status_code_t)nonvolatile.write_info( &info, &info_ext, NV_WRITE_INFO_TIMEOUT );
        break;
    }
    case CMD_EEPROM_ERASE_ALL:
        retc = (status_code_t)nonvolatile.erase_eeprom();
        break;

    case CMD_INVALIDATE_ALL_SECTIONS:
    {
        retc = (status_code_t)nv_data.invalidate_sections();
        break;
    }
    default:
        break;
    }
l_error:
    this->result( retc, ext_res );

    return retc;
};



