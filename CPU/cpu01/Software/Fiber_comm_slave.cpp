/*
 * Fiber_comm.cpp
 *
 *  Created on: 29 mar 2022
 *      Author: MrTea
 */

#include "Fiber_comm_slave.h"
#include "Modbus_devices.h"
#include "version.h"
#include "diskio.h"
#include "Scope.h"

class Fiber_comm_slave_class Fiber_comm_slave;

void Fiber_comm_slave_class::Main()
{
    if(Receive())
    {
        first = 1;
        timer_timeout = IpcRegs.IPCCOUNTERL;
        if(msg.any_frame.comm_func != comm_func_modbus_response_mosi) Modbus_slave_FIBER.RTU->state = Modbus_RTU_class::Modbus_RTU_idle;
        switch(msg.any_frame.comm_func)
        {
            case comm_func_modbus_request_mosi: modbus_request_mosi(); break;
            case comm_func_modbus_response_mosi: modbus_response_mosi(); break;
            case comm_func_scope_mosi: scope_mosi(); break;
            case comm_func_async_data_mosi: async_data_mosi(); break;
            default: status_flags.msg_error = 1; break;
        }
        status_flags.comm_active = 1;
    }
    Uint32 time_passed = IpcRegs.IPCCOUNTERL - timer_timeout;
    if(time_passed > 80000000UL && first)
    {
        first = 0;
        timer_timeout = IpcRegs.IPCCOUNTERL;
        timer_timeout_val = time_passed;
        status_flags.timeout = 1;
    }
    if(status_flags.timeout)
    {
        alarm_ACDC.bit.lopri_timeout = 1;
        status_flags.timeout = 0;
        status_flags.comm_active = 0;
    }
    if(status_flags.msg_error)
    {
        alarm_ACDC.bit.lopri_error = 1;
        status_flags.msg_error = 0;
        status_flags.comm_active = 0;
    }
}

void Fiber_comm_slave_class::modbus_request_mosi()
{
    register Uint16 *source_pointer = msg.modbus_data_frame.data;
    register Uint16 *data_pointer = (Uint16 *)&Modbus_slave_FIBER.RTU->data_in[msg.modbus_data_frame.modbus_header.start_address];
    register Uint16 loop_copy_modbus = msg.modbus_data_frame.modbus_header.packet_length + 1 >> 1;
    while(loop_copy_modbus--)
    {
        *data_pointer++ = *source_pointer & 0xFF;
        *data_pointer++ = *source_pointer++ >> 8 & 0xFF;
    }
    if(msg.modbus_data_frame.modbus_header.start_address + msg.modbus_data_frame.modbus_header.packet_length == msg.modbus_data_frame.modbus_header.total_length)
    {
        union FPGA_ACDC_sync_flags_union Sync_flags;
        Sync_flags.all = EMIF_mem.read.Sync_flags.all;
        if(Sync_flags.bit.node_number_rdy)
        {
            Modbus_slave_FIBER.slave_address = 2 + Sync_flags.bit.node_number;
            Modbus_slave_FIBER.RTU->state = Modbus_RTU_class::Modbus_RTU_idle;
            Modbus_slave_FIBER.RTU->data_in_length = msg.modbus_data_frame.modbus_header.total_length;
            Modbus_slave_FIBER.RTU->set_data_received();
            Modbus_slave_FIBER.task();
        }
    }
    Send(sizeof(msg.modbus_request_response), comm_func_modbus_request_miso);
}

void Fiber_comm_slave_class::modbus_response_mosi()
{
    if(Modbus_slave_FIBER.RTU->state != Modbus_RTU_class::Modbus_RTU_send_response)
        Modbus_slave_FIBER.RTU->data_out_length = 0;

    msg.modbus_data_frame.modbus_header.start_address = msg.modbus_request_response.modbus_header.start_address;

    register Uint16 max_length = sizeof(msg.modbus_data_frame.data)*2;
    register Uint16 requested_length = Modbus_slave_FIBER.RTU->data_out_length - msg.modbus_data_frame.modbus_header.start_address;

    Uint16 no_data_to_send = requested_length > max_length ? max_length : requested_length;
    Uint16 no_data_to_send_u16 = no_data_to_send+1 >> 1;

    if(requested_length <= max_length) Modbus_slave_FIBER.RTU->state = Modbus_RTU_class::Modbus_RTU_idle;

    msg.modbus_data_frame.modbus_header.packet_length = no_data_to_send;
    msg.modbus_data_frame.modbus_header.total_length = Modbus_slave_FIBER.RTU->data_out_length;

    register Uint16 *source_pointer = (Uint16 *)&Modbus_slave_FIBER.RTU->data_out[msg.modbus_data_frame.modbus_header.start_address];
    register Uint16 *data_pointer = msg.modbus_data_frame.data;
    register Uint16 loop_copy_modbus = no_data_to_send_u16;
    while(loop_copy_modbus--)
        *data_pointer++ = ((*source_pointer++ & 0x00FF) << 8) | (*source_pointer++ & 0x00FF);

    Send(sizeof(msg.modbus_data_frame) - sizeof(msg.modbus_data_frame.data)+no_data_to_send_u16, comm_func_modbus_response_miso);
}

void Fiber_comm_slave_class::scope_mosi()
{
    msg.scope_slave.start_address = msg.scope_master.start_address;
    memcpy(&msg.scope_slave.data, (Uint16 *)&Scope.data + msg.scope_slave.start_address, sizeof(msg.scope_slave.data));

    Send(sizeof(msg.scope_slave), comm_func_scope_miso);
}

void Fiber_comm_slave_class::async_data_mosi()
{
    if(SW_ID == msg.async_master.code_version)
    {
        msg.async_slave.code_version = SW_ID;

        msg.async_slave.C_conv = Conv.C_conv;
        if(Conv.RDY2)
        {
            msg.async_slave.P_conv_1h_filter.a = Grid_filter.P_conv_1h.a;
            msg.async_slave.P_conv_1h_filter.b = Grid_filter.P_conv_1h.b;
            msg.async_slave.P_conv_1h_filter.c = Grid_filter.P_conv_1h.c;
            msg.async_slave.Q_conv_1h_filter.a = Grid_filter.Q_conv_1h.a;
            msg.async_slave.Q_conv_1h_filter.b = Grid_filter.Q_conv_1h.b;
            msg.async_slave.Q_conv_1h_filter.c = Grid_filter.Q_conv_1h.c;
        }
        else
        {
            msg.async_slave.P_conv_1h_filter.a =
            msg.async_slave.P_conv_1h_filter.b =
            msg.async_slave.P_conv_1h_filter.c =
            msg.async_slave.Q_conv_1h_filter.a =
            msg.async_slave.Q_conv_1h_filter.b =
            msg.async_slave.Q_conv_1h_filter.c = 0.0f;
        }

        Send(sizeof(msg.async_slave), comm_func_async_data_miso);
    }
    else
    {
        status_flags.msg_error = 1;
    }
}

void Fiber_comm_slave_class::Send(Uint16 length, enum comm_func_enum comm_func)
{
    union FPGA_ACDC_sync_flags_union Sync_flags;
    Sync_flags.all = EMIF_mem.read.Sync_flags.all;
    Uint16 node_number = Sync_flags.bit.node_number;

    if(Sync_flags.bit.node_number_rdy)
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
        EMIF_mem.write.tx_start.all = COMM_flags.all;
    }
}

Uint16 Fiber_comm_slave_class::Receive()
{
    union FPGA_ACDC_sync_flags_union Sync_flags;
    union COMM_flags_union COMM_flags;
    Sync_flags.all = EMIF_mem.read.Sync_flags.all;
    COMM_flags.all = EMIF_mem.read.rx_rdy.all;
    COMM_flags.bit.port1_hipri_msg = 0;
//    COMM_flags.bit.port1_lopri_msg = 0;
    COMM_flags.bit.port2_hipri_msg = 0;
    COMM_flags.bit.port2_lopri_msg = 0;
    EMIF_mem.write.rx_ack.all = COMM_flags.all;

    Uint16 node_number = Sync_flags.bit.node_number;
    if(Sync_flags.bit.node_number_rdy)
    {
        if(COMM_flags.bit.port1_lopri_msg & 1 << node_number)
        {
            register Uint32 *dest = (Uint32 *)&msg;
            register Uint32 *src = (Uint32 *)&EMIF_mem.read.rx1_lopri_msg[node_number];
            *dest++ = *src++;

            register Uint16 loop_copy_EMIF = msg.any_frame.comm_header.length+1 >> 1;
            loop_copy_EMIF--;

            while(loop_copy_EMIF--)
                *dest++ = *src++;

            return 1;
        }
    }
    return 0;
}
