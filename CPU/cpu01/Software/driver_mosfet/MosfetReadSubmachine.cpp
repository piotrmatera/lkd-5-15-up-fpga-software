/*
 * MosfetReadSubmachine.cpp
 *
 *  Created on: 24 lut 2022
 *      Author: Piotr Romaniuk
 */

#include <string.h>
#include "MosfetReadSubmachine.h"
#include "chip1ed389mu12m.h"

status_code_t  MosfetReadSubmachine::process_event( event_t event, void * xdata)
{
    status_code_t retc;
    state_t new_state;

    switch( state ){
    case state_reset:
        return status_inactive;

    case state_idle:
        return status_ok;

    case state_start:
        msg.ready = 0;
        msg.len = 1;
        msg.data[0] = reg_nb;
        retc = i2c->write_nostop(&msg, 0);

        new_state = ( retc == status_ok )? state_waiting4_tx_end : state_error;
        change_state_to( new_state, __LINE__ );

        return retc;

    case state_waiting4_tx_end:
        if( i2c->get_error() !=status_ok )
            change_state_to( state_error, __LINE__ );
        else if( i2c->is_msg_finished_nostop() ){
            msg.ready = 0;
            msg.len = count;
            memset( msg.data, 0, sizeof(msg.data));
            retc = i2c->read( &msg, 0 );

            new_state = (retc == status_ok )? state_waiting4_rx_end : state_error;
            change_state_to( new_state, __LINE__ );
        }
        return status_ok;

    case state_waiting4_rx_end:
        if( i2c->get_error() !=status_ok )
            change_state_to( state_error, __LINE__ );
        else if( msg.ready ){
            change_state_to( state_terminated, __LINE__ );
            return status_terminated;
        }
        return status_ok;

    case state_terminated:
        return status_terminated;

    case state_error:
        if( event == event_restart ){
            change_state_to( state_idle, __LINE__ );
            return status_ok;
        }

    default:
        //z tego nie resetowac [?]
        return err_generic;

    }


}

status_code_t MosfetReadSubmachine::activate( uint16_t reg_nb, uint16_t count )
{
    if( count > MAX_BUFFER_SIZE )
        return err_invalid;

    if( (reg_nb+count) > (CHIP1ED389_LASTREG+1) )
        return err_invalid;

    change_state_to( state_start, __LINE__ );
    this->reg_nb = reg_nb;
    this->count = count;
    return status_ok;
}

void MosfetReadSubmachine::init( i2c_t * i2c ){
   this->i2c = i2c;
   switch_to_idle();
}

void MosfetReadSubmachine::switch_to_idle(void){
   change_state_to( state_idle, __LINE__ );
}

void MosfetReadSubmachine::change_state_to( state_t new_state, uint16_t line ){
    this->state = new_state;
    dbg_marker('*');
    dbg_marker(state);
    if( new_state == state_error )
        state_error_line = line;
    if( line ){
        dbg_marker(line & 0xFF);
    }
}



