/*
 * Modbus_ADU_slave_context.h
 *
 *  Created on: 5 wrz 2021
 *      Author: Piotr
 *
 *      Rozszerzenie struktury Modbus_slave_info do komunikacji pomiedzy urzadzeniem modbus a przetwarzaniem protokolu ADU
 *      rozszerzenie o translacje
 */

#ifndef SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_CONTEXT_H_
#define SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_CONTEXT_H_

#include "Modbus_translator.h"


struct Modbus_slave_info_w_translator_struct
{
    Uint8 slave_address;
    Uint16 *discrete_inputs;
    Uint16 *coils;
    Modbus_translator input_registers;
    Uint16 *holding_registers;
    Uint16 no_discrete_inputs;
    Uint16 no_coils;
    Uint16 no_input_registers;
    Uint16 no_holding_registers;

    void init( struct Modbus_slave_info_struct *slave_info,
               translator_map * translation_mapping,
               Modbus_translator::translator_mode_t mode){

        this->slave_address = slave_info->slave_address;
        this->discrete_inputs = slave_info->discrete_inputs;
        this->coils = slave_info->coils;
        this->input_registers.Init(slave_info, translation_mapping, mode);
        this->holding_registers = slave_info->holding_registers;
        this->no_discrete_inputs = slave_info->no_discrete_inputs;
        this->no_coils = slave_info->no_coils;
        this->no_input_registers = (mode == Modbus_translator::translator_active)? translation_mapping->get_map_items()
                                                                                 : slave_info->no_input_registers;
        this->no_holding_registers = slave_info->no_holding_registers;
    }
};

struct Modbus_ADU_slave_context_struct
{
    char* data_in;
    Uint16 data_in_length;
    char* data_out;
    Uint16 data_out_length;

    Uint16 start_address;
    Uint16 end_address;
    Uint16 quantity;
    enum Modbus_function_enum function;

    Uint8 valid_request;

    enum Modbus_error_enum error;
    enum Modbus_error_enum last_error;
    enum Modbus_function_enum last_error_function;
    Uint8 error_count;

    struct Modbus_slave_info_w_translator_struct slave_info;

};






#endif /* SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_CONTEXT_H_ */
