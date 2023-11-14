/*
 * nonvolatile_commands.h
 *
 *  Created on: 14 lis 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_NONVOLATILE_COMMANDS_H_
#define SOFTWARE_NONVOLATILE_COMMANDS_H_

#include "Modbus_converter_memory.h"
#include "status_codes.h"

class nonvolatile_commands_t{
public:
    nonvolatile_commands_t();

    status_code_t process( modbus_holding_commands_t cmd );

    void result( Uint16 res, Uint16 ext_res );
};

extern nonvolatile_commands_t nonvolatile_commands;

#endif /* SOFTWARE_NONVOLATILE_COMMANDS_H_ */
