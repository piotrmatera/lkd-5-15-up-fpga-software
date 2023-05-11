//Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_MODBUS_ADU_MASTER_H_
#define SOFTWARE_MODBUS_ADU_MASTER_H_

#include "stdafx.h"
#include "Modbus_ADU_slave.h"

#define Uint16_offset(struct, element) (((Uint16 *)&struct.element - (Uint16 *)&struct))

struct Modbus_ADU_master_struct
{
    char data_in[256];
    Uint16 data_in_length;
    char data_out[256];
    Uint16 data_out_length;

    enum Modbus_function_enum function;
    Uint16 start_address;
    Uint16 quantity;

    Uint8 request_or_response;

    enum Modbus_error_enum error;
    enum Modbus_error_enum last_error;
    enum Modbus_function_enum last_error_function;
    Uint8 last_error_slave;
    Uint8 error_count;

    struct Modbus_slave_info_struct slave_info;
};

/////////////////////////////////////////////////////////////////
extern struct Modbus_ADU_master_struct Mdb_master_ADU;

void MdbMasterPrepareRequest(struct Modbus_slave_info_struct *slave_info);
void MdbMasterProcessResponse();

#endif /* SOFTWARE_MODBUS_ADU_MASTER_H_ */
