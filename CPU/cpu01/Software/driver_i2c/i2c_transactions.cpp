/*
 * i2c_transactions.cpp
 *
 *  Created on: 31 paü 2022
 *      Author: Piotr
 */

#include <string.h>
#include "i2c_transactions.h"
#include "Rtc.h"
#include "driver_eeprom/eeprom_i2c.h"
#include "interrupts_control.h"

extern Rtc rtc;
extern i2c_transactions_t i2c_bus;
extern eeprom_i2c eeprom;

i2c_transactions_t::i2c_transactions_t()
{
    state = idle;
    current_transaction = NULL;

}

status_code_t i2c_transactions_t::init(volatile struct I2C_REGS * i2cregs, const struct i2c_t::i2c_clock_config_s * psc_cfg)
{
    return this->i2c.init(i2cregs, psc_cfg);
}

#if USE_SD_INT_FOR_I2C_DRIVER_EEPROM_BUS
status_code_t i2c_transactions_t::process_i2c_interrupt(void){
    return this->i2c.interrupt_process();
}
#endif

status_code_t i2c_transactions_t::start_transaction( struct i2c_transaction_buffer *trans)
{
    if( (trans->type == i2c_transaction_buffer::write_only) ){
        if( (trans->len_in != 0) || (trans->len_out == 0))
            return err_invalid;
    }else if( (trans->type == i2c_transaction_buffer::write_nostop_read) ){
        if( (trans->len_in == 0) || (trans->len_out == 0))
            return err_invalid;
    }else if( (trans->type == i2c_transaction_buffer::polling) ){
        //if( (trans->len_in != 0) || (trans->len_out != 0)) procesor nie wspiera
            return err_invalid;
    }else{
        return err_invalid;
    }

    if( !i2c.is_idle() )
        return err_busy;


    if( _lock( trans ) != status_ok ) // tu pod current_transaction jest podczepiana trans
                                      // tu jest tez glowny LOCK
        return err_busy;

    change_state_to( idle );

    i2c.set_slave_address( this->current_transaction->slave_address);

    return status_ok;
}

status_code_t i2c_transactions_t::process( void )
{
#define MAX_TASKS 2
    status_code_t retc[MAX_TASKS] = {status_ok, status_ok};
    if( _custom_read_st1() & 0x01 == 1U )

    if( _are_interrupts_masked() ) //jesli przerwnaia sa wylaczone
        retc[0] = i2c.interrupt_process();  //obsluga interfejsu sprzetowego i2c

    retc[1] = this->process_internal(); //obsluga transakcji i2c

    //obsluga urzadzen na magistrali i2c
    rtc.process(); //obsluga Rtc

    eeprom.process();

    for(uint16_t i=0; i<MAX_TASKS; i++) //zwraca blad pierwszy ktory napotka
        if( retc[i] != status_ok )
            return retc[i];

    return status_ok;
}



status_code_t i2c_transactions_t::process_internal( void )
{
    status_code_t retc = err_invalid;

    //dalej przetwarzanie transakcji
    if( !is_busy_transaction() )
          return status_ok;

    switch( current_transaction->type){
    case i2c_transaction_buffer::write_only:
        retc = process_write_only();
        break;

    case i2c_transaction_buffer::write_nostop_read:
        retc = process_write_nostop_read();
        break;

   /* case i2c_transaction_buffer::polling:
        retc = process_polling();
        break;*/
    default:
        //nieznany typ transakcji
        transaction_error();

    }
    return retc;
}


void i2c_transactions_t::transaction_error( void )
{
    //ta funkcja jest czyms w rodzaju terminate
    current_transaction->state = i2c_transaction_buffer::error;
    transaction_done();
}

void i2c_transactions_t::transaction_fini( void )
{
    //ta funkcja jest czyms w rodzaju terminate
    current_transaction->state = i2c_transaction_buffer::done;
    transaction_done();
}

status_code_t i2c_transactions_t::process_write_only( void )
{
    status_code_t retc = err_invalid;

    switch( this->state ){
    case idle:
        if( (current_transaction->len_in != 0)|| (current_transaction->len_out == 0)){
            transaction_error();
            break;
        }

        current_transaction->msg.len = current_transaction->len_out;
        current_transaction->msg.ready = 0;

        change_state_to( write );
        current_transaction->state = i2c_transaction_buffer::busy;

        retc = i2c_write( current_transaction, 0 );
        if( retc != status_ok ){
            transaction_error();
            break;
        }
        retc = status_ok;
        break;
    case write:
        if( current_transaction->msg.ready == MSG_READY){
            transaction_fini();
            retc = status_ok;
        }else if( current_transaction->msg.ready == MSG_READY_W_ERROR){
            transaction_error();
            retc = status_ok;
        }
        break;
    default:
        transaction_error();
        break;
    }

    return retc;
}

status_code_t i2c_transactions_t::process_polling( void )
{
    status_code_t retc = err_invalid;
    return retc; //mikrokontroler nie obsluguje transakcji i2c zlozonych tylko z control byte

    /*switch( this->state ){
    case idle:
        if( (current_transaction->len_in != 0)|| (current_transaction->len_out != 0)){
            transaction_error();
            break;
        }

        current_transaction->msg.len = current_transaction->len_out;
        current_transaction->msg.ready = 0;

        change_state_to( write );
        current_transaction->state = i2c_transaction_buffer::busy;

        retc = i2c_write( current_transaction, 0 );
        if( retc != status_ok ){
            transaction_error();
            break;
        }
        retc = status_ok;
        break;
    case write:
        if( current_transaction->msg.ready ){
            transaction_fini();
            retc = status_ok;
            break;
        }
        break;
    default:
        transaction_error();
        break;
    }

    return retc;*/
}


status_code_t i2c_transactions_t::process_write_nostop_read( void )
{
    status_code_t retc = err_invalid;

    switch( this->state ){
    case idle:
        if( (current_transaction->len_in == 0) || (current_transaction->len_out == 0) ){
            transaction_error();
            break;
        }

        current_transaction->msg.len = current_transaction->len_out;
        current_transaction->msg.ready = 0;

        change_state_to( this->write_no_stop );
        current_transaction->state = i2c_transaction_buffer::busy;

        retc = i2c_write_nostop( current_transaction, 0 );
        if( retc != status_ok ){
            transaction_error();
            break;
        }
        retc = status_ok;
        break;
    case write_no_stop:
        if( current_transaction->msg.ready == MSG_READY){

                current_transaction->msg.len = current_transaction->len_in;
                current_transaction->msg.ready = 0;
                memset( current_transaction->msg.data, 0, sizeof(current_transaction->msg.data));
                retc = i2c_read( current_transaction, 0 );
                if( retc != status_ok ){
                    transaction_error();
                    break;
                }
                change_state_to( this->read );
                retc = status_ok;
                break;
        }else if( current_transaction->msg.ready == MSG_READY_W_ERROR){
                transaction_error();
                retc = status_ok;
        }
        break;

    case read:
        if( current_transaction->msg.ready == MSG_READY){
            transaction_fini();
        }else if( current_transaction->msg.ready == MSG_READY_W_ERROR){
            transaction_error();
        }
        retc = status_ok;
        break;
    default:
        transaction_error();
        break;
    }

    return retc;
}

