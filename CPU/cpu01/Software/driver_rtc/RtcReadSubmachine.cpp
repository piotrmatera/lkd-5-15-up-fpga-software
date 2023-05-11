/*
 * RtcReadSubmachine.cpp
 *
 *  Created on: 3 Nov 2020
 *      Author: Piotr
 */
#include <string.h>
#include "Rtc.h"
#include "RtcReadSubmachine.h"

status_code_t  RtcReadSubmachine::process_event( Rtc::event_t event, void * xdata)
{
    status_code_t retc;
    if( !this->active )
        return status_inactive;

    if( rtc->state == base_state ){
        msg.ready = 0;
        msg.len = 1;
        msg.data[0] = reg_nb;
        retc = rtc->i2c.write_nostop(&msg, 0);
        if( retc == status_ok ){
            rtc->change_state_to( (Rtc::state_t)(base_state+1) );
            return status_ok;
        }
        return retc;
    }
    if( rtc->state == (base_state+1) ){
        if( rtc->i2c.is_msg_finished_nostop() ){
            msg.ready = 0;
            msg.len = count;
            memset( msg.data, 0, sizeof(msg.data));
            rtc->i2c.read( &msg, 0 );
            rtc->change_state_to( (Rtc::state_t)(base_state+2 ));
        }
        return status_ok;
    }
    if( rtc->state == (base_state+2) ){
        if( msg.ready ){
            rtc->change_state_to( (Rtc::state_t)(base_state+3 ));
            return status_terminated;
        }
        return status_ok;
    }
//gdy wykryty niewlasciwy stan
    active = false;
    return err_invalid;
}

status_code_t RtcReadSubmachine::activate( Rtc::state_t base_state, uint16_t reg_nb, uint16_t count )
{
    if( count > MAX_BUFFER_SIZE )
        return err_invalid;
    if( reg_nb > MCP7940N_LAST_REG )
        return err_invalid;
    if( (reg_nb ==  MCP7940N_RES1) || (reg_nb ==  MCP7940N_RES2) || (reg_nb == MCP7940N_RES3) )
        return err_invalid;

    this->base_state = base_state;
    active = true;
    this->reg_nb = reg_nb;
    this->count = count;
    rtc->change_state_to( base_state );
    return status_ok;
}



