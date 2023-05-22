/*
 * Fiber_comm.cpp
 *
 *  Created on: 29 mar 2022
 *      Author: MrTea
 */

#include "Fiber_comm_master.h"
#include "Modbus_devices.h"
#include "version.h"
#include "diskio.h"
#include "State.h"
#include "Scope.h"

class Fiber_comm_master_class Fiber_comm[4];
void (Fiber_comm_master_class::*Fiber_comm_master_class::state_pointers[Fiber_comm_master_class::state_max])();

void Fiber_comm_master_class::Main()
{
    register void (Fiber_comm_master_class::*pointer_temp)() = state_pointers[state];

    if(pointer_temp != NULL && state < sizeof(state_pointers)/sizeof(state_pointers[0]))
        (this->*pointer_temp)();
    else
        state = state_idle;
}

void Fiber_comm_master_class::idle()
{
    if (state_last != state)
    {
        state_last = state;
    }

    if(input_flags.send_modbus)
    {
        state = state_modbus_request_mosi;
        input_flags.send_modbus = 0;
    }
    else if(input_flags.read_scope)
    {
        state = state_scope_mosi;
    }
    else
    {
        state = state_async_data_mosi;
    }
}

void Fiber_comm_master_class::modbus_request_mosi()
{
    if (state_last != state)
    {
        if(state_last != state_modbus_request_miso) modbus_data_counter = 0;
        state_last = state;
    }

    register Uint16 max_length = sizeof(msg.modbus_data_frame.data)*2;
    register Uint16 requested_length = Modbus_slave_LCD.RTU->data_in_length - modbus_data_counter;
    Uint16 no_data_to_send = requested_length > max_length ? max_length : requested_length;
    Uint16 no_data_to_send_u16 = no_data_to_send+1 >> 1;

    msg.modbus_data_frame.modbus_header.start_address = modbus_data_counter;
    msg.modbus_data_frame.modbus_header.packet_length = no_data_to_send;
    msg.modbus_data_frame.modbus_header.total_length = Modbus_slave_LCD.RTU->data_in_length;

    register Uint16 *source_pointer = (Uint16 *)&Modbus_slave_LCD.RTU->data_in[modbus_data_counter];
    register Uint16 *data_pointer = msg.modbus_data_frame.data;
    register Uint16 loop_copy_modbus = no_data_to_send_u16;
    while(loop_copy_modbus--)
        *data_pointer++ = ((*source_pointer++ & 0x00FF) << 8) | (*source_pointer++ & 0x00FF);

    Send(sizeof(msg.modbus_data_frame) - sizeof(msg.modbus_data_frame.data)+no_data_to_send_u16, comm_func_modbus_request_mosi);

    modbus_data_counter += no_data_to_send;

    state = state_modbus_request_miso;
}

void Fiber_comm_master_class::modbus_request_miso()
{
    if (state_last != state)
    {
        state_last = state;
    }

    register int16 result = Receive_timeout(comm_func_modbus_request_miso);
    if(result == result_rdy)
        state = (modbus_data_counter == Modbus_slave_LCD.RTU->data_in_length) ? state_modbus_response_mosi : state_modbus_request_mosi;
    else if(result == result_error)
        state = state_idle;
}

void Fiber_comm_master_class::modbus_response_mosi()
{
    if (state_last != state)
    {
        if(state_last != state_modbus_response_miso) modbus_data_counter = 0;
        state_last = state;
    }

    msg.modbus_request_response.modbus_header.start_address = modbus_data_counter;
    msg.modbus_request_response.modbus_header.packet_length = 0;
    msg.modbus_request_response.modbus_header.total_length = 0;

    Send(sizeof(msg.modbus_request_response), comm_func_modbus_response_mosi);

    state = state_modbus_response_miso;
}

void Fiber_comm_master_class::modbus_response_miso()
{
    if (state_last != state)
    {
        state_last = state;
    }

    register int16 result = Receive_timeout(comm_func_modbus_response_miso);
    if(result == result_rdy)
    {
        register Uint16 *source_pointer = msg.modbus_data_frame.data;
        register Uint16 *data_pointer = (Uint16 *)&Modbus_slave_LCD.RTU->data_out[modbus_data_counter];
        register Uint16 loop_copy_modbus = msg.modbus_data_frame.modbus_header.packet_length + 1  >> 1;
        while(loop_copy_modbus--)
        {
            *data_pointer++ = *source_pointer & 0xFF;
            *data_pointer++ = *source_pointer++ >> 8 & 0xFF;
        }
        modbus_data_counter += msg.modbus_data_frame.modbus_header.packet_length;
        if(modbus_data_counter == msg.modbus_data_frame.modbus_header.total_length)
        {
            Modbus_slave_LCD.RTU->data_out_length = msg.modbus_data_frame.modbus_header.total_length;
            if(Modbus_slave_LCD.RTU->data_out_length)
                Modbus_slave_LCD.RTU->send_data();
            state = state_idle;
        }
        else
            state = state_modbus_response_mosi;
    }
    else if(result == result_error)
        state = state_idle;
}

void Fiber_comm_master_class::scope_mosi()
{
    if (state_last != state)
    {
        if(state_last != state_scope_miso) scope_data_counter = 0;
        state_last = state;
    }

    msg.scope_master.start_address = scope_data_counter;

    Send(sizeof(msg.scope_master), comm_func_scope_mosi);

    state = state_scope_miso;
}

void Fiber_comm_master_class::scope_miso()
{
    if (state_last != state)
    {
        state_last = state;
    }

    register int16 result = Receive_timeout(comm_func_scope_miso);
    if(result == result_rdy)
    {
        register Uint16 max_length = sizeof(msg.scope_slave.data);
        register Uint16 requested_length = sizeof(Scope.data) - msg.scope_slave.start_address;
        Uint16 copy_size = requested_length > max_length ? max_length : requested_length;

        memcpy((Uint16 *)&Scope.data + msg.scope_slave.start_address, &msg.scope_slave.data, copy_size);

        scope_data_counter += sizeof(msg.scope_slave.data);
        if(scope_data_counter >= sizeof(Scope.data))
        {
            state = state_idle;
            input_flags.read_scope = 0;
        }
        else
            state = state_scope_mosi;
    }
    else if(result == result_error)
    {
        state = state_idle;
        input_flags.read_scope = 0;
    }
}

void Fiber_comm_master_class::async_data_mosi()
{
    if (state_last != state)
    {
        state_last = state;
    }

    if(input_flags.read_async_data) status_flags.wait_for_async_data = 1;

    msg.async_master.code_version = SW_ID;
    msg.async_master.FatFS_time = *(Uint32 *)&FatFS_time;
    msg.async_master.w_filter = Conv.w_filter;
    msg.async_master.compensation2 = Conv.compensation2;
    msg.async_master.Meas_master_gain = Meas_master_gain;
    msg.async_master.Meas_master_offset = Meas_master_offset;

    Send(sizeof(msg.async_master), comm_func_async_data_mosi);

    state = state_async_data_miso;
}

void Fiber_comm_master_class::async_data_miso()
{
    if (state_last != state)
    {
        state_last = state;
    }

    register int16 result = Receive_timeout(comm_func_async_data_miso);
    if(result == result_rdy)
    {
        if(status_flags.wait_for_async_data) input_flags.read_async_data = 0;
        state = state_idle;
    }
    else if(result == result_error)
    {
        state = state_idle;
    }
    status_flags.wait_for_async_data = 0;
}

void Fiber_comm_master_class::Send(Uint16 length, enum comm_func_enum comm_func)
{
    msg.any_frame.comm_header.length = length - 1;//comm_header + crc + function
    msg.any_frame.comm_header.rsvd = 0;
    msg.any_frame.comm_header.destination_mailbox = node_number;
    msg.any_frame.comm_func = comm_func;

    register Uint32 *dest = (Uint32 *)&EMIF_mem.write.tx1_lopri_msg[node_number];
    register Uint32 *src = (Uint32 *)&msg;
    register Uint16 loop_copy_EMIF = length >> 1;
    while(loop_copy_EMIF--)
        *dest++ = *src++;

    union COMM_flags_union COMM_flags;
    COMM_flags.all = 0;
    COMM_flags.bit.port1_lopri_msg = 1 << node_number;
    EMIF_mem.write.rx_ack.all = COMM_flags.all;
    timestamp_timeout();
    EMIF_mem.write.tx_start.all = COMM_flags.all;

}

Uint16 Fiber_comm_master_class::Receive()
{
    union COMM_flags_union COMM_flags;
    COMM_flags.all = EMIF_mem.read.rx_rdy.all;
    if(COMM_flags.bit.port1_lopri_msg & 1 << node_number)
    {
        register Uint32 *dest = (Uint32 *)&msg;
        register Uint32 *src = (Uint32 *)&EMIF_mem.read.rx1_lopri_msg[node_number];
        *dest++ = *src++;

        register Uint16 loop_copy_EMIF = (msg.any_frame.comm_header.length+1) >> 1;
        loop_copy_EMIF--;

        while(loop_copy_EMIF--)
            *dest++ = *src++;

        COMM_flags.all = 0;
        COMM_flags.bit.port1_lopri_msg = 1 << node_number;
        EMIF_mem.write.rx_ack.all = COMM_flags.all;

        return 1;
    }
    else return 0;
}

void Fiber_comm_master_class::timestamp_timeout()
{
    timeout_stamp = IpcRegs.IPCCOUNTERL;
}

enum Fiber_comm_master_class::func_result_enum Fiber_comm_master_class::Receive_timeout(enum comm_func_enum comm_func)
{
    if(Receive())
    {
        if(msg.any_frame.comm_func == comm_func)
            return result_rdy;
        else
        {
            status_flags.msg_error = 1;
            return result_error;
        }
    }
    else if(IpcRegs.IPCCOUNTERL - timeout_stamp > timeout)
    {
        status_flags.timeout_error = 1;
        return result_error;
    }
    return result_idle;
}

