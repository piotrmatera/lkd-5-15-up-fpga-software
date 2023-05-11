//Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#include "Modbus_ADU_master.h"
#include <string.h>
#include <stdio.h>

struct Modbus_ADU_master_struct Mdb_master_ADU;

static Uint16 MdbCrc(char *d, Uint8 len)
{
Uint16 mcrc;
Uint8 i;
Uint8 j;

    mcrc = 0xFFFF;
    for (i=0;i!=len;i++)
    {
        // trzeba usun¹æ starsz¹ czêœæ
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

static void MdbError(enum Modbus_error_enum error_code)
{
    Mdb_master_ADU.last_error_slave = Mdb_master_ADU.slave_info.slave_address;
    Mdb_master_ADU.last_error_function = Mdb_master_ADU.function;
    Mdb_master_ADU.last_error =
    Mdb_master_ADU.error = error_code;
    Mdb_master_ADU.error_count++;
}

static void MdbReadCoils()
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Read_Coils;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Mdb_master_ADU.data_out[4] = Mdb_master_ADU.quantity>>8;
        Mdb_master_ADU.data_out[5] = Mdb_master_ADU.quantity;
        Mdb_master_ADU.data_out_length = 6;
    }
    else
    {
        if (Mdb_master_ADU.data_in[2] != Mdb_master_ADU.data_in_length-5)
        {
            MdbError(Message_length);
            return;
        }

        Uint16 start = Mdb_master_ADU.start_address;
        Uint16 len = Mdb_master_ADU.quantity;

        Uint8 index8 = start / 8;
        Uint8 shift = start % 8;
        Uint8 data_index = 3;

        while (len)
        {
            Uint16 mask = (len > 8) ? 0xFF : 0xFF >> (8 - len);
            mask <<= shift;
            Uint16 data_temp = (((Uint16)Mdb_master_ADU.data_in[data_index]) << shift) & mask;
            Uint8 index16 = index8 >> 1;
            Uint16 bits_temp;
            if (index8 & 0x01)
            {
                bits_temp = (Mdb_master_ADU.slave_info.coils[index16] >> 8) | (Mdb_master_ADU.slave_info.coils[index16 + 1] << 8);
                bits_temp = (bits_temp & ~mask) | data_temp;
                Mdb_master_ADU.slave_info.coils[index16] = (Mdb_master_ADU.slave_info.coils[index16] & 0x00FF) | (bits_temp << 8);
                Mdb_master_ADU.slave_info.coils[index16 + 1] = (Mdb_master_ADU.slave_info.coils[index16 + 1] & 0xFF00) | (bits_temp >> 8);
            }
            else
            {
                bits_temp = Mdb_master_ADU.slave_info.coils[index16];
                bits_temp = (bits_temp & ~mask) | data_temp;
                Mdb_master_ADU.slave_info.coils[index16] = bits_temp;
            }
            len -= (len > 8) ? 8 : len;
            index8++;
            data_index++;
        }
    }
}

static void MdbReadDiscreteInputs()
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Read_Discrete_Inputs;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Mdb_master_ADU.data_out[4] = Mdb_master_ADU.quantity>>8;
        Mdb_master_ADU.data_out[5] = Mdb_master_ADU.quantity;
        Mdb_master_ADU.data_out_length = 6;
    }
    else
    {
        if (Mdb_master_ADU.data_in[2] != Mdb_master_ADU.data_in_length - 5)
        {
            MdbError(Message_length);
            return;
        }

        Uint16 start = Mdb_master_ADU.start_address;
        Uint16 len = Mdb_master_ADU.quantity;

        Uint8 index8 = start / 8;
        Uint8 shift = start % 8;
        Uint8 data_index = 3;

        while (len)
        {
            Uint16 mask = (len > 8) ? 0xFF : 0xFF >> (8 - len);
            mask <<= shift;
            Uint16 data_temp = (((Uint16)Mdb_master_ADU.data_in[data_index]) << shift) & mask;
            Uint8 index16 = index8 >> 1;
            Uint16 bits_temp;
            if (index8 & 0x01)
            {
                bits_temp = (Mdb_master_ADU.slave_info.discrete_inputs[index16] >> 8) | (Mdb_master_ADU.slave_info.discrete_inputs[index16 + 1] << 8);
                bits_temp = (bits_temp & ~mask) | data_temp;
                Mdb_master_ADU.slave_info.discrete_inputs[index16] = (Mdb_master_ADU.slave_info.discrete_inputs[index16] & 0x00FF) | (bits_temp << 8);
                Mdb_master_ADU.slave_info.discrete_inputs[index16 + 1] = (Mdb_master_ADU.slave_info.discrete_inputs[index16 + 1] & 0xFF00) | (bits_temp >> 8);
            }
            else
            {
                bits_temp = Mdb_master_ADU.slave_info.discrete_inputs[index16];
                bits_temp = (bits_temp & ~mask) | data_temp;
                Mdb_master_ADU.slave_info.discrete_inputs[index16] = bits_temp;
            }
            len -= (len > 8) ? 8 : len;
            index8++;
            data_index++;
        }
    }
}

static void MdbReadHoldingRegisters(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Read_Holding_Registers;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Mdb_master_ADU.data_out[4] = Mdb_master_ADU.quantity>>8;
        Mdb_master_ADU.data_out[5] = Mdb_master_ADU.quantity;
        Mdb_master_ADU.data_out_length = 6;
    }
    else
    {
        if (Mdb_master_ADU.data_in[2] != Mdb_master_ADU.data_in_length - 5)
        {
            MdbError(Message_length);
            return;
        }

        Uint16 start = Mdb_master_ADU.start_address;
        Uint16 len = Mdb_master_ADU.quantity;
        Uint16 end = start + len - 1;

        Uint16 index = start;
        Uint16 index_data_in = 3;
        while(index <= end)
        {
            Mdb_master_ADU.slave_info.holding_registers[index++] = (((Uint16)Mdb_master_ADU.data_in[index_data_in]) << 8) | ((Uint16)Mdb_master_ADU.data_in[index_data_in + 1] & 0xFF);
            index_data_in += 2;
        }
    }
}

static void MdbReadInputRegisters(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Read_Input_Registers;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Mdb_master_ADU.data_out[4] = Mdb_master_ADU.quantity>>8;
        Mdb_master_ADU.data_out[5] = Mdb_master_ADU.quantity;
        Mdb_master_ADU.data_out_length = 6;
    }
    else
    {
        if (Mdb_master_ADU.data_in[2] != Mdb_master_ADU.data_in_length-5)
        {
            MdbError(Message_length);
            return;
        }

        Uint16 start = Mdb_master_ADU.start_address;
        Uint16 len = Mdb_master_ADU.quantity;
        Uint16 end = start + len - 1;

        Uint16 index = start;
        Uint16 index_data_in = 3;
        while(index <= end)
        {
            Mdb_master_ADU.slave_info.input_registers[index++] = (((Uint16)Mdb_master_ADU.data_in[index_data_in]) << 8) | ((Uint16)Mdb_master_ADU.data_in[index_data_in + 1] & 0xFF);
            index_data_in += 2;
        }
    }
}

static void MdbWriteSingleCoil(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Write_Single_Coil;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        if(Mdb_master_ADU.slave_info.coils[Mdb_master_ADU.start_address/16] & (1UL << (Mdb_master_ADU.start_address % 16)))
            Mdb_master_ADU.data_out[4] = 0xFF;
        else
            Mdb_master_ADU.data_out[4] = 0x00;
        Mdb_master_ADU.data_out[5] = 0x00;
        Mdb_master_ADU.data_out_length = 6;
    }
}

static void MdbWriteSingleRegister(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Write_Single_Register;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Uint16 register_temp = Mdb_master_ADU.slave_info.holding_registers[Mdb_master_ADU.start_address];
        Mdb_master_ADU.data_out[4] = register_temp>>8;
        Mdb_master_ADU.data_out[5] = register_temp;
        Mdb_master_ADU.data_out_length = 6;
    }
}

static void MdbDiagnostic(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Diagnostics;
        Mdb_master_ADU.data_out[2] = 0;
        Mdb_master_ADU.data_out[3] = 0;
        Mdb_master_ADU.data_out_length = 4;
    }
}

static void MdbWriteMultipleCoils(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Write_Multiple_Coils;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Mdb_master_ADU.data_out[4] = Mdb_master_ADU.quantity>>8;
        Mdb_master_ADU.data_out[5] = Mdb_master_ADU.quantity;

        Uint16 start = Mdb_master_ADU.start_address;
        Uint16 len = Mdb_master_ADU.quantity;

        Uint8 index8 = start/8;
        Uint8 shift = start%8;
        Uint8 index16 = index8 >> 1;

        Mdb_master_ADU.data_out_length = 7;

        while(len)
        {
            Uint16 bits_temp;
            if(index8 & 0x01)
            {
                bits_temp = (Mdb_master_ADU.slave_info.coils[index16] >> 8) | (Mdb_master_ADU.slave_info.coils[index16 + 1] << 8);
            }
            else
            {
                bits_temp = Mdb_master_ADU.slave_info.coils[index16];
            }
            Uint8 mask = (len > 8) ? 0xFF : 0xFF>>(8-len);
            Mdb_master_ADU.data_out[Mdb_master_ADU.data_out_length++] = (bits_temp >> shift) & mask;
            len -= (len > 8) ? 8 : len;
            index8++;
        }

        Mdb_master_ADU.data_out[6] = Mdb_master_ADU.data_out_length - 7;
    }
}

static void MdbWriteMultipleRegisters(void)
{
    if(!Mdb_master_ADU.request_or_response)
    {
        Mdb_master_ADU.data_out[0] = Mdb_master_ADU.slave_info.slave_address;
        Mdb_master_ADU.data_out[1] = Write_Multiple_Registers;
        Mdb_master_ADU.data_out[2] = Mdb_master_ADU.start_address>>8;
        Mdb_master_ADU.data_out[3] = Mdb_master_ADU.start_address;
        Mdb_master_ADU.data_out[4] = Mdb_master_ADU.quantity>>8;
        Mdb_master_ADU.data_out[5] = Mdb_master_ADU.quantity;
        Mdb_master_ADU.data_out[6] = Mdb_master_ADU.quantity<<1;

        Uint16 start = Mdb_master_ADU.start_address;
        Uint16 len = Mdb_master_ADU.quantity;
        Uint16 end = start + len - 1;

        Mdb_master_ADU.data_out_length = 7;

        Uint16 index = start;
        while(index <= end)
        {
            Mdb_master_ADU.data_out[Mdb_master_ADU.data_out_length++] = Mdb_master_ADU.slave_info.holding_registers[index]>>8;
            Mdb_master_ADU.data_out[Mdb_master_ADU.data_out_length++] = Mdb_master_ADU.slave_info.holding_registers[index++];
        }
    }
}

void MdbMasterPrepareRequest(struct Modbus_slave_info_struct *slave_info)
{
    memcpy(&Mdb_master_ADU.slave_info, slave_info, sizeof(struct Modbus_slave_info_struct));

    Mdb_master_ADU.request_or_response = 0;

    switch (Mdb_master_ADU.function)
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

        default:
            break;
    }

    Uint16 CRC_out = MdbCrc(Mdb_master_ADU.data_out, Mdb_master_ADU.data_out_length);
    Mdb_master_ADU.data_out[Mdb_master_ADU.data_out_length++] = CRC_out & 0xFF;
    Mdb_master_ADU.data_out[Mdb_master_ADU.data_out_length++] = (CRC_out>>8) & 0xFF;
}

void MdbMasterProcessResponse()
{
    if (Mdb_master_ADU.data_in_length < 5)
    {
        MdbError(Message_length);
        return;
    }

    Uint16 CRC_in = ((Uint16)(Mdb_slave_ADU.data_in[Mdb_slave_ADU.data_in_length - 1]) << 8) | ((Uint16)(Mdb_slave_ADU.data_in[Mdb_slave_ADU.data_in_length - 2]) & 0xFF);
    Uint16 CRC_in_calc = MdbCrc(Mdb_slave_ADU.data_in, Mdb_slave_ADU.data_in_length - 2);
    if (CRC_in != CRC_in_calc)
    {
        MdbError(CRC_error);
        return;
    }

    if (Mdb_master_ADU.data_in[0] != Mdb_master_ADU.slave_info.slave_address || Mdb_master_ADU.data_in[1] != Mdb_master_ADU.function)
    {
        if (Mdb_master_ADU.data_in[1] & 0x80)
        {
            MdbError((enum Modbus_error_enum)Mdb_master_ADU.data_in[2]);
            return;
        }
        MdbError(Invalid_response);
        return;
    }

    Mdb_master_ADU.request_or_response = 1;
    Mdb_master_ADU.error = No_Error;

    switch (Mdb_master_ADU.function)
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

        default:
            break;
    }
}
