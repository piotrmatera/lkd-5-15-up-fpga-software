//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_MODBUS_ADU_SLAVE_H_
#define SOFTWARE_MODBUS_ADU_SLAVE_H_

#include <limits.h>
#include "stdafx.h"

#ifndef __TMS320C28XX_CLA__

#define Uint16_offset(struct, element) (((Uint16 *)&struct.element - (Uint16 *)&struct))

#if CHAR_BIT == 16
typedef Uint16 Uint8;
#endif

#endif

/////////////////////////////////////////////////////////////////
enum Modbus_function_enum
{
    No_Function              = 0x00,
    Read_Coils               = 0x01,
    Read_Discrete_Inputs     = 0x02,
    Read_Holding_Registers   = 0x03,
    Read_Input_Registers     = 0x04,
    Write_Single_Coil        = 0x05,
    Write_Single_Register    = 0x06,
    Diagnostics              = 0x08,
    Write_Multiple_Coils     = 0x0F,
    Write_Multiple_Registers = 0x10,
    Report_Server_ID         = 0x11,
    Read_File_Record         = 0x14

};

typedef enum Modbus_error_enum
{
    No_Error = 0,
    Illegal_Function = 1,
    Illegal_Data_Address = 2,
    Illegal_Data_Value = 3,
    Server_Device_Failure = 4,
    Message_length = 16,
    CRC_error = 17,
    Invalid_response = 18,
    No_Response = 19,
} Modbus_error_enum_t;

struct Modbus_slave_info_struct
{
    Uint8 slave_address;
    Uint16 *discrete_inputs;
    Uint16 *coils;
    Uint16 *input_registers;
    Uint16 *holding_registers;
    Uint16 no_discrete_inputs;
    Uint16 no_coils;
    Uint16 no_input_registers;
    Uint16 no_holding_registers;
};

struct modbus_read_file_record_req{
        Uint16 fn_code;     //0x14
        Uint16 len_bytes;   //dlugosc w bajtach wiadomosci
        Uint16 sub_req;     //sub-request = MODBUS_FILERECORD_SUBREQ_CODE = 6
        Uint16 file_nb_hi;  //file_nb 1-N, +0x1000 (=read header)
        Uint16 file_nb_lo;
        Uint16 rec_no_hi;   //rec_no 0-K (dla odczytu naglowka = 0)
        Uint16 rec_no_lo;
        Uint16 rec_len_hi;  //rec_len w [u16] (dla odczytu naglowka ignorowane)
        Uint16 rec_len_lo;
/* RSP:
 *  fn_code
 *  len_bytes
 *  file_len (1 bajt)
 *  dane (file_nb x slowa)
 *  jesli plik lub naglowek ma nieparzysta dlugosc, uzupelniane zerem
 * */

 };

#define MODBUS_FILERECORD_SUBREQ_CODE 6
#define MODBUS_MAX_PAYLOAD 253
#define MODBUS_FILE_HEADER_OFFSET 0x1000

/*struct Modbus_ADU_slave_struct - uzupelnione i przeniesione do Modbus_ADU_slave_context.h
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

    struct Modbus_slave_info_struct slave_info;
};*/

/////////////////////////////////////////////////////////////////

#endif /* SOFTWARE_MODBUS_ADU_SLAVE_H_ */
