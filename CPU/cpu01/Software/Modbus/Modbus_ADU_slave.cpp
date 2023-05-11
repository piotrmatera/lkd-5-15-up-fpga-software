//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "Modbus_Converter_memory.h"

#include <string.h>
#include "version.h"
#include "Modbus_ADU_slave_context.h"
#include "Modbus_ADU_api.h"
#include "files_readonly.h"

Modbus_ADU_slave_context_struct Mdb_slave_ADU;

static Uint16 MdbCrc(char *d, Uint8 len)
{
Uint16 mcrc;
Uint8 i;
Uint8 j;

    mcrc = 0xFFFF;
    for (i=0;i!=len;i++)
    {
    	// trzeba usun�� starsz� cz��
    	d[i] &= 0x00FF;
        mcrc ^= (Uint16) d[i];
        for (j=0;j!=8;j++)
        {
            if (mcrc&0x0001)
            {
                mcrc >>= 1;
                mcrc ^= 0xA001;
            }
            else
            {
                mcrc >>= 1;
            }
        }
    }

    return mcrc;
}

static void MdbErrorResp(enum Modbus_error_enum error_code)
{
    Mdb_slave_ADU.data_out[0] = Mdb_slave_ADU.slave_info.slave_address;
    Mdb_slave_ADU.data_out[1] = Mdb_slave_ADU.function | 0x80;
    Mdb_slave_ADU.data_out[2] = error_code;
    Mdb_slave_ADU.data_out_length = 3;

    Mdb_slave_ADU.last_error_function = Mdb_slave_ADU.function;
    Mdb_slave_ADU.last_error =
    Mdb_slave_ADU.error = error_code;
    Mdb_slave_ADU.error_count++;

    Mdb_slave_ADU.valid_request = 1;
}

static void MdbReadCoils()
{
    Uint16 start = Mdb_slave_ADU.start_address;
    Uint16 len = Mdb_slave_ADU.quantity;
    Uint16 end = Mdb_slave_ADU.end_address;

    if (len > Mdb_slave_ADU.slave_info.no_coils || len > 2000)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    if ((end >= Mdb_slave_ADU.slave_info.no_coils) || (start > end))
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    Mdb_slave_ADU.data_out[0] = Mdb_slave_ADU.slave_info.slave_address;
    Mdb_slave_ADU.data_out[1] = Read_Coils;
    Mdb_slave_ADU.data_out_length = 3;

    Uint8 index8 = start / 8;
    Uint8 shift = start % 8;
    Uint8 index16 = index8 >> 1;

    while (len)
    {
        Uint16 coil_temp;
        if (index8 & 0x01)
        {
            coil_temp = (Mdb_slave_ADU.slave_info.coils[index16] >> 8) | (Mdb_slave_ADU.slave_info.coils[index16 + 1] << 8);
        }
        else
        {
            coil_temp = Mdb_slave_ADU.slave_info.coils[index16];
        }
        Uint8 mask = (len > 8) ? 0xFF : 0xFF >> (8 - len);
        Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = (coil_temp >> shift) & mask;
        len -= (len > 8) ? 8 : len;
        index8++;
    }

    Mdb_slave_ADU.data_out[2] = Mdb_slave_ADU.data_out_length - 3;
}

static void MdbReadDiscreteInputs()
{
    Uint16 start = Mdb_slave_ADU.start_address;
    Uint16 len = Mdb_slave_ADU.quantity;
    Uint16 end = Mdb_slave_ADU.end_address;

    if (len > Mdb_slave_ADU.slave_info.no_discrete_inputs || len > 2000)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    if ((end >= Mdb_slave_ADU.slave_info.no_discrete_inputs) || (start > end))
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    Mdb_slave_ADU.data_out[0] = Mdb_slave_ADU.slave_info.slave_address;
    Mdb_slave_ADU.data_out[1] = Read_Discrete_Inputs;
    Mdb_slave_ADU.data_out_length = 3;

    Uint8 index8 = start / 8;
    Uint8 shift = start % 8;
    Uint8 index16 = index8 >> 1;

    while (len)
    {
        Uint16 coil_temp;
        if (index8 & 0x01)
        {
            coil_temp = (Mdb_slave_ADU.slave_info.discrete_inputs[index16] >> 8) | (Mdb_slave_ADU.slave_info.discrete_inputs[index16 + 1] << 8);
        }
        else
        {
            coil_temp = Mdb_slave_ADU.slave_info.discrete_inputs[index16];
        }
        Uint8 mask = (len > 8) ? 0xFF : 0xFF >> (8 - len);
        Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = (coil_temp >> shift) & mask;
        len -= (len > 8) ? 8 : len;
        index8++;
    }

    Mdb_slave_ADU.data_out[2] = Mdb_slave_ADU.data_out_length - 3;
}
static void MdbReadHoldingRegisters(void)
{
    Uint16 start = Mdb_slave_ADU.start_address;
    Uint16 len = Mdb_slave_ADU.quantity;
    Uint16 end = Mdb_slave_ADU.end_address;

    if (len > Mdb_slave_ADU.slave_info.no_holding_registers || len > 125)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    if ((end >= Mdb_slave_ADU.slave_info.no_holding_registers) || (start > end))
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    Mdb_slave_ADU.data_out[0] = Mdb_slave_ADU.slave_info.slave_address;
    Mdb_slave_ADU.data_out[1] = 3;
    Mdb_slave_ADU.data_out_length = 3;

    Uint16 index = start;
    while(index <= end)
    {
        register Uint16 holding_register = Mdb_slave_ADU.slave_info.holding_registers[index++];
        Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = holding_register>>8;
        Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = holding_register;
    }

    Mdb_slave_ADU.data_out[2] = Mdb_slave_ADU.data_out_length - 3;
}

static void MdbReadInputRegisters(void)
{
    Uint16 start = Mdb_slave_ADU.start_address;
    Uint16 len = Mdb_slave_ADU.quantity;
    Uint16 end = Mdb_slave_ADU.end_address;

    if (len > Mdb_slave_ADU.slave_info.no_input_registers || len > 125)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    if ((end >= Mdb_slave_ADU.slave_info.no_input_registers) || (start > end))
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    Mdb_slave_ADU.data_out[0] = Mdb_slave_ADU.slave_info.slave_address;
    Mdb_slave_ADU.data_out[1] = 4;
    Mdb_slave_ADU.data_out_length = 3;

    Uint16 index = start;
    while(index <= end)
    {
        register Uint16 input_register = Mdb_slave_ADU.slave_info.input_registers[index++];
        Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = input_register>>8;
        Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = input_register;
    }

    Mdb_slave_ADU.data_out[2] = Mdb_slave_ADU.data_out_length - 3;
}

static void MdbWriteSingleCoil(void)
{
    Uint16 address = Mdb_slave_ADU.start_address;

    if (address >= Mdb_slave_ADU.slave_info.no_coils)
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Uint16 data = (((Uint16)Mdb_slave_ADU.data_in[4]) << 8) | ((Uint16)Mdb_slave_ADU.data_in[5] & 0xFF);
    
    if (data != 0xFF00 && data != 0x0000)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    if(data)
        Mdb_slave_ADU.slave_info.coils[address/16] |=  (1UL << (address % 16));
    else
        Mdb_slave_ADU.slave_info.coils[address/16] &= ~(1UL << (address % 16));

    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, 6);
    Mdb_slave_ADU.data_out_length = 6;
}

static void MdbWriteSingleRegister(void)
{
    Uint16 address = Mdb_slave_ADU.start_address;

    if (address >= Mdb_slave_ADU.slave_info.no_holding_registers)
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    Mdb_slave_ADU.slave_info.holding_registers[address] = (((Uint16)Mdb_slave_ADU.data_in[4]) << 8) | ((Uint16)Mdb_slave_ADU.data_in[5] & 0xFF);
    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, 6);
    Mdb_slave_ADU.data_out_length = 6;
}

static void MdbDiagnostic(void)
{
    Uint16 instruction = (((Uint16)Mdb_slave_ADU.data_in[2]) << 8) | ((Uint16)Mdb_slave_ADU.data_in[3] & 0xFF);

	if (instruction==0)
	{
        Mdb_slave_ADU.error = No_Error;
	    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, Mdb_slave_ADU.data_in_length-2);
        Mdb_slave_ADU.data_out_length = Mdb_slave_ADU.data_in_length-2;
	}
	else
	{
		 MdbErrorResp(Illegal_Function);
	}
}

static void MdbWriteMultipleCoils(void)
{
    Uint16 start = Mdb_slave_ADU.start_address;
    Uint16 len = Mdb_slave_ADU.quantity;
    Uint16 end = Mdb_slave_ADU.end_address;

    if (len > Mdb_slave_ADU.slave_info.no_coils || len > 1968)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    if ((end >= Mdb_slave_ADU.slave_info.no_coils) || (start > end))
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;
    
    Uint8 index8 = start/8;
    Uint8 shift = start%8;
    Uint8 data_index = 7;

    while(len)
    {
        Uint16 mask = (len > 8) ? 0xFF : 0xFF>>(8-len);
        mask <<= shift;
        Uint16 data_temp = (((Uint16)Mdb_slave_ADU.data_in[data_index]) << shift) & mask;
        Uint8 index16 = index8>>1;
        Uint16 bits_temp;
        if(index8 & 0x01)
        {
            bits_temp = (Mdb_slave_ADU.slave_info.coils[index16] >> 8) | (Mdb_slave_ADU.slave_info.coils[index16 + 1] << 8);
            bits_temp = (bits_temp & ~mask) | data_temp;
            Mdb_slave_ADU.slave_info.coils[index16] = (Mdb_slave_ADU.slave_info.coils[index16] & 0x00FF) | (bits_temp << 8);
            Mdb_slave_ADU.slave_info.coils[index16 + 1] = (Mdb_slave_ADU.slave_info.coils[index16 + 1] & 0xFF00) | (bits_temp >> 8);
        }
        else
        {
            bits_temp = Mdb_slave_ADU.slave_info.coils[index16];
            bits_temp = (bits_temp & ~mask) | data_temp;
            Mdb_slave_ADU.slave_info.coils[index16] = bits_temp;
        }
        len -= (len > 8) ? 8 : len;
        index8++;
        data_index++;
    }

    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, 6);
    Mdb_slave_ADU.data_out_length = 6;
}

static void MdbWriteMultipleRegisters(void)
{
    Uint16 start = Mdb_slave_ADU.start_address;
    Uint16 len = Mdb_slave_ADU.quantity;
    Uint16 end = Mdb_slave_ADU.end_address;

    if (len > Mdb_slave_ADU.slave_info.no_holding_registers || len > 123)
    {
        MdbErrorResp(Illegal_Data_Value);
        return;
    }

    if ((end >= Mdb_slave_ADU.slave_info.no_holding_registers) || (start > end))
    {
        MdbErrorResp(Illegal_Data_Address);
        return;
    }

    Mdb_slave_ADU.error = No_Error;

    Uint16 index = start;
    Uint16 index_data_in = 7;
    while(index <= end)
    {
        Mdb_slave_ADU.slave_info.holding_registers[index++] = (((Uint16)Mdb_slave_ADU.data_in[index_data_in]) << 8) | ((Uint16)Mdb_slave_ADU.data_in[index_data_in + 1] & 0xFF);
        index_data_in += 2;
    }

    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, 6);
    Mdb_slave_ADU.data_out_length = 6;
}

static void MdbReadFileRecord(void)
{
    Mdb_slave_ADU.error = No_Error;
    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, 2); //skopiowanie adresu i nr funkcji
    Uint16 temp_index = 3; //przeskoczenie pola dlugosci wiadomosci modbus

    struct modbus_read_file_record_req * preq = (struct modbus_read_file_record_req*)&(Mdb_slave_ADU.data_in[1]);

    struct file_data_dsc data_dsc;

    data_dsc.file_nb = (preq->file_nb_hi << 8) + preq->file_nb_lo;
    data_dsc.rec_no  = (preq->rec_no_hi << 8) + preq->rec_no_lo;
    data_dsc.rec_len = (preq->rec_len_hi << 8)+ preq->rec_len_lo;

    if( MODBUS_FILERECORD_SUBREQ_CODE != preq->sub_req
            || (data_dsc.file_nb > files_readonly_number && data_dsc.file_nb <= MODBUS_FILE_HEADER_OFFSET)
            || data_dsc.file_nb > files_readonly_number+MODBUS_FILE_HEADER_OFFSET
            || data_dsc.rec_len > (MODBUS_MAX_PAYLOAD/2) ){
        Mdb_slave_ADU.last_error_function = Mdb_slave_ADU.function;
        Mdb_slave_ADU.last_error =
        Mdb_slave_ADU.error = Illegal_Data_Address;
        Mdb_slave_ADU.error_count++;
        return;
    }

    Mdb_slave_ADU.data_out[temp_index++] = data_dsc.rec_len;
    Mdb_slave_ADU.data_out[temp_index++] = MODBUS_FILERECORD_SUBREQ_CODE;

    status_code_t retc;
    if( data_dsc.file_nb > MODBUS_FILE_HEADER_OFFSET){
        retc = read_file_header( &data_dsc, Mdb_slave_ADU.data_out, &temp_index);
    }else{
        retc = read_file_data( &data_dsc, Mdb_slave_ADU.data_out, &temp_index);
    }

    if( retc != status_ok ){
        Mdb_slave_ADU.last_error_function = Mdb_slave_ADU.function;
        Mdb_slave_ADU.last_error =
        Mdb_slave_ADU.error = Illegal_Data_Address;
        Mdb_slave_ADU.error_count++;
        return;
    }

    Mdb_slave_ADU.data_out_length = temp_index;
    Mdb_slave_ADU.data_out[2] = temp_index - 3;
    Mdb_slave_ADU.data_out[3] = (temp_index - 3 -2 +1/*zaokraglenie w gore*/)/2;//liczba slow
    //uwaga ostatnie niepelne slowo przesylane jako bajt [CHECK]
    //czy nie nalezy uzupelniac do pelnego slowa a pole dlugosc oznaczyc ile bajtow jest
}

static void MdbReportServerID(void)
{
    Mdb_slave_ADU.error = No_Error;

    memcpy(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_in, 2);
    Uint16 temp_index = 3;
    Mdb_slave_ADU.data_out[temp_index++] = (fw_descriptor.modbus_id >> 8) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (fw_descriptor.modbus_id >> 0) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (fw_descriptor.board_id >> 8) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (fw_descriptor.board_id >> 0) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (fw_descriptor.software_id >> 8) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (fw_descriptor.software_id >> 0) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)&fw_descriptor.dsc >> 24) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)&fw_descriptor.dsc >> 16) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)&fw_descriptor.dsc >> 8) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)&fw_descriptor.dsc >> 0) & 0xFF;

    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[0] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[1] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[2] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[3] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[4] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[5] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[6] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[7] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[8] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[9] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[10] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[11] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[12] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[13] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[14] & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = fw_descriptor.dev_type[15] & 0xFF;
    //unique ID procesora - unikalny dla tego samego partnumber
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)(0x000703C0 + 0xC) >> 24) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)(0x000703C0 + 0xC) >> 16) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)(0x000703C0 + 0xC) >> 8) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = (*(Uint32 *)(0x000703C0 + 0xC) >> 0) & 0xFF;
    Mdb_slave_ADU.data_out[temp_index++] = 0x00;

    Mdb_slave_ADU.data_out_length = temp_index;
    Mdb_slave_ADU.data_out[2] = temp_index - 3;
}

mdb_prepare_status_t MdbSlavePrepareRequest(struct Modbus_slave_info_struct *slave_info, translator_map * translation_mapping,
                            Modbus_translator::translator_mode_t mode)
{
    Mdb_slave_ADU.valid_request = 0;
    Mdb_slave_ADU.slave_info.init(slave_info, translation_mapping, mode);

    if ((Mdb_slave_ADU.data_in[0] & 0xFF) != Mdb_slave_ADU.slave_info.slave_address)
        return mdb_request_wrong_address;

    Mdb_slave_ADU.function = (enum Modbus_function_enum)(Mdb_slave_ADU.data_in[1] & 0xFF);

    if (Mdb_slave_ADU.data_in_length < 4)
    {
        Mdb_slave_ADU.last_error_function = Mdb_slave_ADU.function;
        Mdb_slave_ADU.last_error =
        Mdb_slave_ADU.error = Message_length;
        Mdb_slave_ADU.error_count++;
        return mdb_request_too_short;
    }

    Uint16 CRC_in = ((Uint16)(Mdb_slave_ADU.data_in[Mdb_slave_ADU.data_in_length-1]) << 8) | ((Uint16)(Mdb_slave_ADU.data_in[Mdb_slave_ADU.data_in_length-2]) & 0xFF);
    Uint16 CRC_in_calc = MdbCrc(Mdb_slave_ADU.data_in, Mdb_slave_ADU.data_in_length - 2);
    if (CRC_in != CRC_in_calc)
    {
        Mdb_slave_ADU.last_error_function = Mdb_slave_ADU.function;
        Mdb_slave_ADU.last_error =
        Mdb_slave_ADU.error = CRC_error;
        Mdb_slave_ADU.error_count++;
        return mdb_request_crc_error;
    }

    Mdb_slave_ADU.start_address = (((Uint16)Mdb_slave_ADU.data_in[2]) << 8) | ((Uint16)Mdb_slave_ADU.data_in[3] & 0xFF);
    Mdb_slave_ADU.quantity = (((Uint16)Mdb_slave_ADU.data_in[4]) << 8) | ((Uint16)Mdb_slave_ADU.data_in[5] & 0xFF);
    Mdb_slave_ADU.end_address = Mdb_slave_ADU.start_address + Mdb_slave_ADU.quantity - 1;

    Mdb_slave_ADU.valid_request = 1;
    return mdb_request_valid;
}

void MdbSlaveProcessRequest()
{
    switch (Mdb_slave_ADU.function)
    {
        case Read_Coils:
            MdbReadCoils();
            break;

        case Read_Discrete_Inputs:
            MdbReadDiscreteInputs();
            break;

        case Read_Holding_Registers:
            MdbReadHoldingRegisters();
            break;

        case Read_Input_Registers:
            MdbReadInputRegisters();
            break;

        case Write_Single_Coil:
            MdbWriteSingleCoil();
            break;

        case Write_Single_Register:
            MdbWriteSingleRegister();
            break;

        case Diagnostics:
            MdbDiagnostic();
            break;

        case Write_Multiple_Coils:
            MdbWriteMultipleCoils();
            break;

        case Write_Multiple_Registers:
            MdbWriteMultipleRegisters();
            break;

        case Report_Server_ID:
            MdbReportServerID();
            break;

        case Read_File_Record:
            MdbReadFileRecord();
            break;

        default:
            MdbErrorResp(Illegal_Function);
            break;
    }

    Uint16 CRC_out = MdbCrc(Mdb_slave_ADU.data_out, Mdb_slave_ADU.data_out_length);
    Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = CRC_out & 0xFF;
    Mdb_slave_ADU.data_out[Mdb_slave_ADU.data_out_length++] = (CRC_out>>8) & 0xFF;
}

