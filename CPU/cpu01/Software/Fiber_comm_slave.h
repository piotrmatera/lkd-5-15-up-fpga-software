/*
 * Fiber_comm.h
 *
 *  Created on: 29 mar 2022
 *      Author: MrTea
 */

#ifndef FIBER_COMM_SLAVE_H_
#define FIBER_COMM_SLAVE_H_

#include "stdafx.h"

class Fiber_comm_slave_class
{
    public:

    void Main();
    struct
    {
        Uint32 msg_error : 1;
        Uint32 timeout : 1;
    }status_flags;
    Uint32 timer_timeout_val;
    private:

    Uint32 timer_timeout;
    Uint32 first;

    union COMM_async_msg_union msg;

    void Send(Uint16 length, enum comm_func_enum comm_func);
    Uint16 Receive();

    void modbus_request_mosi();
    void modbus_response_mosi();
    void scope_mosi();
    void async_data_mosi();
};

extern class Fiber_comm_slave_class Fiber_comm_slave;

#endif /* FIBER_COMM_SLAVE_H_ */
