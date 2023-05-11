/*
 * Fiber_comm.h
 *
 *  Created on: 29 mar 2022
 *      Author: MrTea
 */

#ifndef FIBER_COMM_MASTER_H_
#define FIBER_COMM_MASTER_H_

#include "stdafx.h"
#include <string.h>

class Fiber_comm_master_class
{
    public:

    Fiber_comm_master_class()
    {
        memset(this, 0, sizeof(class Fiber_comm_master_class));

        for(Uint16 i = 0; i < state_max; i++)
            state_pointers[i] = NULL;

        state_pointers[state_idle] = &Fiber_comm_master_class::idle;
        state_pointers[state_modbus_request_mosi] = &Fiber_comm_master_class::modbus_request_mosi;
        state_pointers[state_modbus_request_miso] = &Fiber_comm_master_class::modbus_request_miso;
        state_pointers[state_modbus_response_mosi] = &Fiber_comm_master_class::modbus_response_mosi;
        state_pointers[state_modbus_response_miso] = &Fiber_comm_master_class::modbus_response_miso;
        state_pointers[state_scope_mosi] = &Fiber_comm_master_class::scope_mosi;
        state_pointers[state_scope_miso] = &Fiber_comm_master_class::scope_miso;
        state_pointers[state_async_data_mosi] = &Fiber_comm_master_class::async_data_mosi;
        state_pointers[state_async_data_miso] = &Fiber_comm_master_class::async_data_miso;

        for(Uint16 i = 0; i < state_max; i++)
            if(state_pointers[i] == NULL) ESTOP0;

        timeout = 120000000UL;//600ms
    }

    Uint16 node_number;

    void Main();
    struct
    {
        Uint16 send_modbus : 1;
        Uint16 read_scope : 1;
        Uint16 read_async_data : 1;
    }input_flags;
    struct
    {
        Uint16 timeout_error : 1;
        Uint16 msg_error : 1;
        Uint16 wait_for_async_data : 1;
    }status_flags;
    private:


    Uint32 timeout_stamp;
    Uint32 timeout;
    Uint16 modbus_data_counter;
    Uint16 scope_data_counter;

    union COMM_async_msg_union msg;

    enum state_enum
    {
        state_idle,
        state_modbus_request_mosi,
        state_modbus_request_miso,
        state_modbus_response_mosi,
        state_modbus_response_miso,
        state_scope_mosi,
        state_scope_miso,
        state_async_data_mosi,
        state_async_data_miso,
        state_max,
        __dummybig_state = 300000
    };

    enum func_result_enum
    {
        result_error = -1,
        result_idle,
        result_rdy,
    };

    enum state_enum state, state_last;

    void Send(Uint16 length, enum comm_func_enum comm_func);
    Uint16 Receive();
    enum func_result_enum Receive_timeout(enum comm_func_enum comm_func);
    void timestamp_timeout();

    void idle();
    void modbus_request_mosi();
    void modbus_request_miso();
    void modbus_response_mosi();
    void modbus_response_miso();
    void scope_mosi();
    void scope_miso();
    void async_data_mosi();
    void async_data_miso();

    static void (Fiber_comm_master_class::*state_pointers[state_max])();
};

extern class Fiber_comm_master_class Fiber_comm[4];

#endif /* FIBER_COMM_MASTER_H_ */
