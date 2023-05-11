//Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#include "Modbus_Converter_memory.h"

#pragma SET_DATA_SECTION("RAM_16BIT_ADDR")

struct Modbus_Converter_memory_struct Modbus_Converter =
{
    .info =
    {
        .slave_address = 1,
        .discrete_inputs = (Uint16 *)&Modbus_Converter.discrete_inputs,
        .coils = (Uint16 *)&Modbus_Converter.coils,
        .input_registers = (Uint16 *)&Modbus_Converter.input_registers,
        .holding_registers = (Uint16 *)&Modbus_Converter.holding_registers,
        .no_discrete_inputs = sizeof(Modbus_Converter.discrete_inputs)*CHAR_BIT,
        .no_coils = sizeof(Modbus_Converter.coils)*CHAR_BIT,
        .no_input_registers = sizeof(Modbus_Converter.input_registers) / sizeof(Uint16),
        .no_holding_registers = sizeof(Modbus_Converter.holding_registers) / sizeof(Uint16),
    },
    .discrete_inputs = {0},
    .coils = {0},
    .input_registers = {0},
    .holding_registers = {0},
};
