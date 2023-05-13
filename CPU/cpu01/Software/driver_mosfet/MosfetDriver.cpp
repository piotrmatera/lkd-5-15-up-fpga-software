/*
 * MosfetDriver.cpp
 *
 *  Created on: 24 lut 2022
 *      Author: Piotr Romaniuk
 */

#include <string.h>
#include "MosfetDriver.h"
#include "debug/Dbg.h"

MosfetDriver::MosfetDriver(): state(state_reset){
}


status_code_t MosfetDriver::init()
{
    status_code_t retc;
    retc = i2c.set_slave_address( CHIP1ED389_ADDRESS_INIT );
    if( retc!=status_ok ) return retc;

    //wypelnienie 1/3
    // Fmod_i2c = SYSCLK/(I2CPSC+1) = 7-12MHz
    // MCP7940N Tccl_min = 0.6us, Tcch = 1.3us, f=400kHz

    static const i2c_t::i2c_clock_config_s clk_cfg = I2C_220kHz_DUTY1_2;
    retc = i2c.init(&I2cbRegs, &clk_cfg);
    if( retc!=status_ok ) return retc;

    this->submachine_read_reg.init( &this->i2c );

    state = state_idle;
    return status_ok;
}

status_code_t MosfetDriver::set_i2c_device_address( uint16_t addr ){
    return i2c.set_slave_address( addr );
}

void MosfetDriver::process()
{
    i2c.interrupt_process();// tu nie mozna reagowac na blad, bo jeszcze moze potrzebowac wykonac i2c.stop

    submachine_read_reg.process_event(MosfetReadSubmachine::event_periodic, NULL);
    process_event( event_periodic );
}

status_code_t MosfetDriver::process_event( event_t event, void * xdata )
{
    status_code_t retc = err_invalid;

    if( state == state_error && event == event_restart ){
        submachine_read_reg.process_event(MosfetReadSubmachine::event_restart, NULL);
        change_state_to( state_idle );
    }

    if( state != state_idle && event != event_periodic )
        return err_invalid;

    switch( state ){
    case state_reset:
        /* stan zanim wykona sie init */
        return err_invalid;

    case state_idle: /* w tym stanie przyjmuje wydawane polecenia */
        if( !i2c.is_idle() )
            return err_busy;

        switch( event ){
        case event_read_regs:
        {
            struct xdata_event_regs * s = (struct xdata_event_regs*)xdata;
            if( s->count > MAX_BUFFER_SIZE ) //TODO sprawdzic czy uwzglednia adres rejestru
                return err_invalid;

            this->read_buf = *s; //UWAGA! ten bufor musi istniec az do zakonczenia transakcji na i2c

            retc = submachine_read_reg.activate( s->address, s->count );
            if( retc == status_ok )
                change_state_to( state_read_regs );
            return retc;
        }
        case event_write_regs:
        {
            struct xdata_event_regs * s = (struct xdata_event_regs*)xdata;
            if( s->count > MAX_BUFFER_SIZE ) //TODO sprawdzic czy uwzglednia adres rejestru
                return err_invalid;

            this->msg_write_reg.len = s->count+1;
            this->msg_write_reg.ready = 0;
            this->msg_write_reg.data[0] = s->address;
            for(int i=0;i<s->count;i++)
                this->msg_write_reg.data[i+1] = s->data[i];

            retc = i2c.write( &msg_write_reg, 0 );
            change_state_to( state_write_regs );
            return retc;
        }

        default:
            return status_ok;
        }

     //oczekiwanie na zakonczenie odczytu rejestru
     case state_read_regs:
         if( submachine_read_reg.state == MosfetReadSubmachine::state_error ){
              change_state_to( state_error );
              return err_invalid;
          }
         if( submachine_read_reg.state == MosfetReadSubmachine::state_terminated ){
              // akcja do przepisania do lokalnych struktur wyniku:
              notify_read_reg( &submachine_read_reg.msg );
              submachine_read_reg.switch_to_idle();
              change_state_to( state_idle );
          }
          return status_ok;

     //oczekiwanie na zakonczenie zapisu rejestru
     case state_write_regs:
          if( i2c.is_msg_finished() ){
              status_code_t err = i2c.get_error();
              change_state_to( err == status_ok? state_idle : state_error );
          }
          return status_ok;

     case state_error:
     default:
         return err_invalid;
     }


}

void MosfetDriver::change_state_to( state_t new_state ){
    state = new_state;
    dbg_marker('%');
    dbg_marker(state);
}

void MosfetDriver::notify_read_reg( msg_buffer * msg ){
    if( read_buf.data == NULL )
        return;

    for(int i=0; i < this->read_buf.count; i++){
        //przepisanie z wiadomosci do struktury podanej przy wywolaniu read_regs
        read_buf.data[i] = msg->data[i];
    }
}


