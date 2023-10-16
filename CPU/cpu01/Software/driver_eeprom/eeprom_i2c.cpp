/*
 * eeprom_i2c.cpp
 *
 *  Created on: 8 lis 2022
 *      Author: Piotr
 */

#include <string.h>
#include "eeprom_i2c.h"

void memcpy_with_unpack( uint16_t * dest, const uint16_t * src, uint16_t bytes ){
    uint16_t ix;
    for( ix = 0; ix < bytes; ix+=2 ){
        dest[ix]   = src[ix/2] & 0xFF;
        if( ix + 1 < bytes )
            dest[ix+1] = (src[ix/2] >> 8)&0xFF;
    }
}

void memcpy_with_pack( uint16_t * dest, const uint16_t * src, uint16_t bytes ){
    uint16_t ix;
    for( ix = 0; ix < bytes; ix+=2 ){
        dest[ix/2] = src[ix];
        if( ix + 1 < bytes )
            dest[ix/2] += ((src[ix+1]&0xFF)<<8);
    }

}


eeprom_i2c::eeprom_i2c(){
    memset( &this->x_msg, 0, sizeof(this->x_msg));
    //memset( &this->x_msg_read, 0, sizeof(this->x_msg_read));
    region = NULL;
    state = state_reset;
}

status_code_t eeprom_i2c::init( i2c_transactions_t * i2c_bus ){
    //TODO jest to samo w Rtc - powinno byc tylko raz
    status_code_t retc;
    this->i2c_bus = i2c_bus;

    //wypelnienie 1/3
    // Fmod_i2c = SYSCLK/(I2CPSC+1) = 7-12MHz
    // MCP7940N Tccl_min = 0.6us, Tcch = 1.3us, f=400kHz

    static const i2c_t::i2c_clock_config_s clk_cfg = I2C_220kHz_DUTY1_2;
    retc = i2c_bus->init(&I2caRegs, &clk_cfg);
    if( retc!=status_ok) return retc;

    state = state_idle;
    return status_ok;
}

void eeprom_i2c::process()
{
    process_event( event_periodic );
}

status_code_t eeprom_i2c::process_event( event_t event, void * xdata )
{
    status_code_t retc = status_ok;

    if( state != state_idle && event == event_abort ){
        dbg_marker('A');
        region = NULL;
        return status_ok;
    }

    if( state != state_idle && event != event_periodic )
        return err_invalid;

    switch( state ){
    case state_reset:
        /* stan zanim wykona sie init */
        return err_invalid;

    case state_idle:
    {
        if( xdata == NULL )
            return err_invalid;

        /* glowny stan w ktorym mozna wydac polecenie (odczytu, zapisu, inicjalizacji) */
        switch( event ){
        case event_init:
            break;

        case event_read_region:
            region = (struct event_region_xdata *)xdata;
            region->status = event_region_xdata::started;
            goto l_loop_read;

        case event_write_region:
            region = (struct event_region_xdata *)xdata;
            region->status = event_region_xdata::started;
            goto l_loop_write;

        default:
            return err_invalid;
        }

    }
    break;

    case state_read_start: /* tez stan jest wydzielony aby wtracenie innej operacji i zajecie i2c,
            tylko wstrzymalo wykonywanie polecen a nie zablokowalo eeprom_i2c.
            Jesli i2c stanie sie zajete, to start_transaction() zwroci err_busy
            ale ten obiekt caly czas bedzie w tym stanie wiec bedzie ponawial proby
            az sie uda start_transaction */
        if( region == NULL ){ //gdyby wystapil abort w miedzyczasie
            change_state_to( state_error );
            break;
        }

        msg_factory(read_part, &x_msg, region);
        if( i2c_bus->start_transaction( &x_msg ) ){
            region->status = event_region_xdata::idle;
            return err_busy;
        }

        change_state_to( state_read_region );
        break;

    case state_write_start: //analogicznie do state_read_start
        if( region == NULL ){ //gdyby wystapil abort w miedzyczasie
            change_state_to( state_error );
            break;
        }

        msg_factory(write_part, &x_msg, region);
        if( i2c_bus->start_transaction( &x_msg ) ){
            region->status = event_region_xdata::idle;
            return err_busy; //TODO co sie stanie gdy tu wyjdzie w state = state_wait_for_write_delay; czy to goto nie generje problemu?
/* chyba nie moze sie nie udac start (err_busy) bo to by oznaczalo, ze
* cos rozpoczelo transakcje na i2c a eeprom jest uzywany w sposob blokujacy
*
* ew. nalezy:
* 1. wydzielic stan event_write_region_start - przed rozpoczeciem transakcji (sprawdzic na pocz. region == NULL aby ew. zrobic abort)
* 2. fragment od l_next_write do msg_factory (bez tego) skopiowac do state_wait_for_write_delay
* 3. zastapic goto przejsciem do tego stanu
* Przy takim rozwiazaniu gdy sie nie uda start_transaction to bedzie probowalo ponownie bez zmiany danych w region
* */

        }
        change_state_to( state_wait_for_write );
        break;

    case state_wait_for_write:
        if( x_msg.state == i2c_transaction_buffer::done ){
            this->write_ready_time = ReadIpcTimer() + EEPROM_WRITE_TIME;
            change_state_to( state_wait_for_write_delay );
        }else if( x_msg.state == i2c_transaction_buffer::error ){
            change_state_to( state_error );
        }
        break;

    case state_wait_for_write_delay:
        if( ReadIpcTimer() > this->write_ready_time ){
            dbg_marker('Q');
            if( region == NULL ){
                change_state_to( state_error );
                break;
            }

            region->start += region->len;
            region->data = &region->data[ region->len/2 ];
l_loop_write:
            region->len = this->aligned_part( region->start, &region->total_len );
            if( region->len == 0 ){ //koniec zapisu regionu
                region->status = event_region_xdata::done_ok;
                region = NULL;
                change_state_to( state_idle );
                return status_ok;
            }
            change_state_to( state_write_start );
            break;
        }
        break;
#if 0         //nieuzywany polling, procesor nie obsluguje
    case state_wait_for_write1:
        //msg_factory(polling, &x_msg_polling, xdata);    //polling przez wyslanie samego 'i2c control byte' nie dziala
        //if( i2c_bus->start_transaction( &x_msg_polling ) ) //sprzetowy problem w module i2c mikrokontrolera
                            /* aby samoczynnie sie wygenerowal STOP konieczne sa jakies dane (a w tym scenariuszu nie ma)
                             * przypadek z NACK jest do obsluzenia
                             * ale z ACK jest problem zeby wygenerowac STOP
                             * mozna wprawdzie wymusic powrod linii SCL do stanu wysokiego, ale
                             * w tym czasie SDA tez jest juz wysokie po ACK
                             * Dlatego nie mozna wygenerowac STOP, ktore jest L->H@SDA gdy SCL=H
                             * Byly przeprowadzone proby, ale zadna nie doprowazila do sukcesu:
                             * 1. bez fifo tx
                             * 2. reset fifo, reset modulu i2c
                             * Wyglada na to, ze po wejciu w taki stan nawet trudno przywrocic jego wlasciwe dzialanie
                             * Caly czas zachowuje sie jakby 'trzymal' zajeta magistrale i2c
                             */
        if( i2c_bus->start_transaction( &x_msg_read ) ) /* proba wykonywania pollingu przez wiadomosc odczytu
                             * z punktu widzenia eepromu
                             * 1. przypadek z NACK wyglada tak samo jak z samym bajtem kontrolnym
                             * 2. gdy jest ACK, wiadomosc [write ADR, (nostop) read data] nie powinna byc szkodliwa,
                             *    bo wykonuje odczyt
                             * z punktu widzenia mikrokontrolera
                             * 1. nigdy nie wystepuje przypadek z wysylaniem samego bajtu kontrolnego bez danych
                             * 2. obsluga NACK dziala
                             * 3. obsluga przypadku z ACK jest jak normalny odczyt
                             *
                             * TODO nie ma potrzeby czytania calego regionu. Zmniejszyc albo zrobic weryfikacje zapisu na tym
                             */
            return err_busy;
        change_state_to( state_wait_for_write2 );
        break;

    case state_wait_for_write2:
        if( x_msg_read.state == i2c_transaction_buffer::error ){
        //if( x_msg_polling.msg.ready == 2 ){
            asm (" nop ");
            change_state_to( state_wait_for_write1 );
        }
        if( x_msg_read.state == i2c_transaction_buffer::done ){
        //if( x_msg_polling.msg.ready == 1 ){
            asm (" nop ");
            change_state_to( state_idle );
        }
        break;
#endif
    case state_read_region:
        if( x_msg.state == i2c_transaction_buffer::done ){
              if( region == NULL ){
                  change_state_to( state_error );
                  break;
              }

              memcpy_with_pack( region->data, &x_msg.msg.data[0], region->len );

              region->start += region->len;
              region->data = &region->data[ region->len/2 ];
     l_loop_read:
              region->len = this->aligned_part( region->start, &region->total_len );
              if( region->len == 0 ){ //koniec odczytu regionu
                  region->status = event_region_xdata::done_ok;
                  region = NULL;
                  change_state_to( state_idle );
                  return status_ok;
              }
              change_state_to( state_read_start );
              break;



        }else if( x_msg.state == i2c_transaction_buffer::error ){
              change_state_to( state_error );
        }

        break;

    case state_error:
        if( region != NULL )
            region->status = event_region_xdata::done_error;
        change_state_to( state_idle );
        return status_ok;

    case state_invalid:

    default:
        return err_invalid;
    }
    return retc;
}


void eeprom_i2c::change_state_to( state_t new_state )
{
    state = new_state;
    dbg_marker('&');
    dbg_marker('0'+state);
}

status_code_t eeprom_i2c::msg_factory( msg_type_t msg_type, i2c_transaction_buffer * buffer, void * xdata ){
    struct event_region_xdata * data_dsc = (struct event_region_xdata*)xdata;

    buffer->slave_address = EEPROM_ADDRESS;
    buffer->state = i2c_transaction_buffer::idle;

    switch( msg_type ){
    case read_part:
    {
        if( data_dsc->len > EEPROM_PAGE )
             return err_invalid;
        buffer->len_out = 2;
        buffer->len_in = data_dsc->len;
        buffer->type = i2c_transaction_buffer::write_nostop_read;

        buffer->msg.len = 0;
        buffer->msg.ready = 0;
        buffer->msg.data[0] = (data_dsc->start & EEPROM_ADDRESS_MASK) >> 8;
        buffer->msg.data[1] = (data_dsc->start & EEPROM_ADDRESS_MASK) & 0x00FF;

        return status_ok;
    }
    case write_part:
    {
        if( data_dsc->len > EEPROM_PAGE )
            return err_invalid; //zapis nie wiecej niz 1 strony

        if( (data_dsc->start & EEPROM_PAGE_MASK) != ((data_dsc->start+data_dsc->len-1) & EEPROM_PAGE_MASK))
            return err_invalid; //musi byc w obrebie jednej strony

        buffer->len_out = 2 + data_dsc->len;
        buffer->len_in = 0;
        buffer->type = i2c_transaction_buffer::write_only;

        buffer->msg.len = 0;
        buffer->msg.ready = 0;
        buffer->msg.data[0] = (data_dsc->start & EEPROM_ADDRESS_MASK) >> 8;
        buffer->msg.data[1] = (data_dsc->start & EEPROM_ADDRESS_MASK) & 0x00FF;

        memcpy_with_unpack( &buffer->msg.data[2], data_dsc->data, data_dsc->len );
        return status_ok;
    }
    case polling:
        buffer->len_out = 0;
        //buffer->len_out = 1;
        buffer->len_in = 0;
        buffer->type = i2c_transaction_buffer::i2c_transaction_buffer::polling;
        //buffer->type = i2c_transaction_buffer::write_only;

        buffer->msg.len = 0;
        buffer->msg.ready = 0;
        buffer->msg.data[0] = 0;

        //jakis nowy typ transakcji, zeby wykrywac kiedy jest a kiedy brak ACK

        return status_ok;
    default:
        break;
    }
    return err_invalid;
}

uint16_t eeprom_i2c::aligned_part( uint16_t start_address, uint16_t * len_left ){
    uint16_t part_size;
    uint16_t part_size_to_end_of_page = EEPROM_PAGE;

    if( start_address & (EEPROM_PAGE-1) ){
        //niewyrownany adres
        part_size_to_end_of_page = EEPROM_PAGE - (start_address & (EEPROM_PAGE-1));//do konca strony
    }

    part_size = ( (*len_left) < part_size_to_end_of_page )? (*len_left) : EEPROM_PAGE;

    *len_left -= part_size;

    return part_size;
}



