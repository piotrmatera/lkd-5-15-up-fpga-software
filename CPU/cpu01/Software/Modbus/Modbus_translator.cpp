/*
 * Modbus_translator.cpp
 *
 *  Created on: 5 wrz 2021
 *      Author: Piotr
 */

#include "Modbus_translator.h"
#include "time_bcd_struct.h"
#include "SD_card.h"

Modbus_translator::Modbus_translator(){
    slave_info = NULL;
    translation_mapping = NULL;
    mode = translator_disabled;
}

void Modbus_translator::Init( struct Modbus_slave_info_struct * slave_info, translator_map * translation_mapping, translator_mode_t mode )
{
    this->slave_info = slave_info;
    this->translation_mapping = translation_mapping;
    this->mode = mode;
}

Uint16 Modbus_translator::operator[]( size_t index ){
    if( mode == translator_disabled ){
        if( slave_info )
            return slave_info->input_registers[ index ];
        else
            return 0;
    }

    //tu bedzie translacja

    if( this->translation_mapping ){

        const struct translator_map::translator_map_item * dsc = this->translation_mapping->get_translation_descriptor( index  );
        switch( dsc->translation_type ){
        case translator_map::undefined:
            return 0;

        case translator_map::int16_to_int16:
            return slave_info->input_registers[dsc->source_register_index];

        case translator_map::int32_to_low_int16:
            //TODO sprawdzic kolejnosc bajtow i zwracane slowa
            //
            // F2000 jest LE wiec u32 (=XH,H,L,XL) w pamieci u16 jest:
            //
            // 0:  L, XL
            // 1: XH,  H
            return slave_info->input_registers[dsc->source_register_index];

        case translator_map::int32_to_high_int16:
            return slave_info->input_registers[dsc->source_register_index+1];

        case translator_map::float_to_int16:
        {
            //TODO kolejnosc bajtow - ok, to Modbus_ADU_slave zajmuje sie zamiana kolejnosci
            //   w pamieci slave_info->input_registers kolejnosc jest zgodna z kolejnoscia procesora;
            //   z tego miejsca do Modbus_ADU_Slave jest przekazywana wartosc int16
            float f = *(float*)&slave_info->input_registers[dsc->source_register_index];
            f = f * (float)(Uint16)dsc->args;
            Uint16 x;
            if( f>=0.0f ) x = (int)(f+0.5f);
            else x = (int)(f-0.5f);
            return x;
        }

        case translator_map::max2int16_to_int16:
        {
            Uint16 x1 = slave_info->input_registers[dsc->source_register_index];
            Uint16 x2 = slave_info->input_registers[dsc->source_register_index+1];
            if( x2>x1 ) x1 = x2;
            return x1;
        }

        case translator_map::time_to_hour:
        {
            struct time_BCD_struct tm = *(struct time_BCD_struct*)&slave_info->input_registers[dsc->source_register_index];
            return tm.hour10*10+tm.hour;
        }
        case translator_map::time_to_min:
        {
            struct time_BCD_struct tm = *(struct time_BCD_struct*)&slave_info->input_registers[dsc->source_register_index];
            return tm.minute10*10+tm.minute;
        }
        case translator_map::time_to_sec:
        {
            struct time_BCD_struct tm = *(struct time_BCD_struct*)&slave_info->input_registers[dsc->source_register_index];
            return tm.second10*10+tm.second;
        }
        case translator_map::time_to_year:
        {
            struct time_BCD_struct tm = *(struct time_BCD_struct*)&slave_info->input_registers[dsc->source_register_index];
            return tm.year10*10+tm.year + 2000;//TODO sprawdzic czy 2000?
        }
        case translator_map::time_to_month:
        {
            struct time_BCD_struct tm = *(struct time_BCD_struct*)&slave_info->input_registers[dsc->source_register_index];
            return tm.month10*10+tm.month;
        }
        case translator_map::time_to_day:
        {
            struct time_BCD_struct tm = *(struct time_BCD_struct*)&slave_info->input_registers[dsc->source_register_index];
            return tm.day10*10+tm.day;
        }
        case translator_map::spec_get_ilim:
        {
            extern SD_card_class SD_card;
            float f = Conv.I_lim;
            Uint16 x = (int)(f+0.5f);
            return x;
        }
        default:
            return 0;
        }

    }else{
        return 0;
    }

}
