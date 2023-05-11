/*
 * rtc.cpp
 *
 *  Created on: 2 Nov 2020
 *      Author: Piotr
 */
#include <string.h>
#include "Rtc.h"
#include "RtcReadSubmachine.h"
#include "debug/Dbg.h"

class RtcReadSubmachine submachine;/**<@brief zewnetrzna klasa submaszyny
zdefiniowana tak, aby uproscic przydzielenie dla niej pamieci (nie uzywac new)*/

Rtc::Rtc()
{
    memset( &last_time_bcd, 0, sizeof(last_time_bcd) );
    state = state_reset;
    submachine.init( this );
}

status_code_t Rtc::init()
{
    status_code_t retc;
    retc = i2c.set_slave_address( MCP7940N_ADDRESS );
    if( retc!=status_ok) return retc;

    retc = i2c.init();
    if( retc!=status_ok) return retc;

    state = state_idle;
    return status_ok;
}

void Rtc::process()
{
    i2c.interrupt_process();
    process_event( event_periodic );
}

status_code_t Rtc::process_event( event_t event, void * xdata )
{
    status_code_t retc = err_invalid;

    if( state != state_idle && event != event_periodic )
        return err_invalid;

    submachine.process_event(event, xdata);

    datetime_bcd_s new_time;

    switch( state ){
    case state_reset:
        /* stan zanim wykona sie init */
        return err_invalid;

    case state_idle:
    {
        /* glowny stan w ktorym mozna wydac polecenie (odczytu, zapisu, inicjalizacji) */
        switch( event ){
        case event_read_time:
            if( !i2c.is_idle() )
                return err_busy;

            submachine.activate( state_read_time1, MCP7940N_SEC_REG, 7);//wewnatrz jest zmiana stanu
            break;
        case event_init:
            if( !i2c.is_idle() )
                return err_busy;

            submachine.activate( state_read_reg0_1, MCP7940N_SEC_REG, 1);
            break;

        case event_setup_time:
            new_time = time_convert_to_bcd( (Rtc::datetime_s *)xdata );
            goto l_common;

        case event_setup_time_bcd:
            {
            new_time = *((Rtc::datetime_bcd_s *)xdata);
        l_common:
            if( prepare_setup_msg( &new_time )!=status_ok )
                return err_invalid;

            retc = i2c.write( &msg_stop_rtc, 0 );
            if( retc != status_ok )
                 return retc;

            change_state_to( state_new_time );
            }
            break;

        default:
            return err_invalid;
        }
    }

    //submaszyna odczytu czasu (reg0-reg6)
    case state_read_time1:
    case state_read_time2:
    case state_read_time3:
        break;
    case state_read_time4: //tu juz zakonczono odczyt
        time_is_read( &submachine.msg );
        change_state_to( state_idle );
        break;

    //submaszyna odczytu rejestru reg0 - na potrzeby odczytu ST
    case state_read_reg0_1:
    case state_read_reg0_2:
    case state_read_reg0_3:
        break;
    case state_read_reg0_4:
        if( submachine.msg.data[0] & MCP7940N_ST_MASK ){  //czy wlaczony zegar (REG0.ST=1?)
            dbg_marker('E');
            change_state_to( state_idle );
            break;
            }
        dbg_marker('e');
        change_state_to( state_setup_default );
        //TODO nie mozna zbyt szybko wysylac w tym miejscu - nie wysylalo [dlaczego?]
        break;

    case state_setup_default:
        retc = i2c.write( &msg_start_rtc0, 0 );//jak sie nie uruchomi to niewiele mozna zrobic
        if( retc != status_ok ){
            __HOOK_SETUP_DEFAULT_ERROR;
            return retc;
        }
        change_state_to( state_idle );
        break;

    case state_new_time:
        if( msg_stop_rtc.ready ){
            submachine.activate( state_new_time1, MCP7940N_WEEKDAY_REG, 1);
        }
        break;

//submaszyna odczytu reg3 - tam jest OSCRUN
    case state_new_time1:
    case state_new_time2:
    case state_new_time3:
        break;
    case state_new_time4:
        if( !(submachine.msg.data[0] & MCP7940N_WEEKDAY_OSCRUN_MASK) ){
             change_state_to( state_new_time5 );
             break;
        }
        submachine.activate( state_new_time1, MCP7940N_WEEKDAY_REG, 1);
        break;

    case state_new_time5:
        retc = i2c.write( &msg_new_time, 0 );
        if( retc != status_ok )
             return retc;
        change_state_to( state_new_time6 );
        break;

    case state_new_time6:
        if( msg_new_time.ready ){
            prepare_start_msg();
            retc = i2c.write( &msg_new_time, 0 );
            if( retc != status_ok )
                 return retc;
            change_state_to( state_new_time7 );
        }
        break;
    case state_new_time7:
        if( msg_new_time.ready ){
            change_state_to( state_idle );
        }
        break;
    default:
        return err_invalid;
    }
    return status_ok;
}

void Rtc::change_state_to( state_t new_state )
{
    state = new_state;
    dbg_marker('+');
    dbg_marker('0'+state);
}

void Rtc::time_is_read( msg_buffer * msg )
{
    last_time_bcd.sec = msg->data[ MCP7940N_SEC_REG ] & MCP7940N_SEC_MASK;
    last_time_bcd.min = msg->data[ MCP7940N_MIN_REG ] & MCP7940N_MIN_MASK;
    last_time_bcd.hour = msg->data[ MCP7940N_HOUR_REG ] & MCP7940N_HOUR_MASK;
    last_time_bcd.dayofweek = msg->data[  MCP7940N_WEEKDAY_REG ] & MCP7940N_WEEKDAY_MASK;
    last_time_bcd.day = msg->data[ MCP7940N_DAY_REG ] & MCP7940N_DAY_MASK;
    last_time_bcd.month = msg->data[ MCP7940N_MONTH_REG ] & MCP7940N_MONTH_MASK;
    last_time_bcd.year = msg->data[ MCP7940N_YEAR_REG ];

    last_time = time_convert_from_bcd( &last_time_bcd );
}

status_code_t Rtc::prepare_setup_msg( datetime_bcd_s * new_time ){
    if( new_time->sec >= 0x60 ) return err_invalid;
    if( new_time->min >= 0x60 ) return err_invalid;
    if( new_time->hour >= 0x23 ) return err_invalid;
    if( (new_time->dayofweek==0) || (new_time->dayofweek>7)) return err_invalid;
    if( (new_time->day==0) || (new_time->day>0x31)) return err_invalid;//TODO mozna testwoac miesiace i lata przestepne
    if( (new_time->month==0) || (new_time->month>0x12)) return err_invalid;

    //+1 bo jest adress @[0]
    msg_new_time.data[ MCP7940N_SEC_REG+1 ] = new_time->sec;
    msg_new_time.data[ MCP7940N_MIN_REG+1 ] = new_time->min;
    msg_new_time.data[ MCP7940N_HOUR_REG+1 ] = new_time->hour;
    msg_new_time.data[ MCP7940N_WEEKDAY_REG+1 ] = new_time->dayofweek | MCP7940N_WEEKDAY_VBATEN_MASK;
    msg_new_time.data[ MCP7940N_DAY_REG+1 ] = new_time->day;
    msg_new_time.data[ MCP7940N_MONTH_REG+1 ] = new_time->month;
    msg_new_time.data[ MCP7940N_YEAR_REG+1 ] = new_time->year;
    msg_new_time.len = 8;
    return status_ok;
}

status_code_t Rtc::prepare_start_msg( void ){
    msg_new_time.data[ MCP7940N_SEC_REG+1 ] |= MCP7940N_ST_MASK;
    msg_new_time.len = 2;
    return status_ok;
}

Rtc::datetime_bcd_s Rtc::get_last_time_bcd( void )
{
    return last_time_bcd;
}

Rtc::datetime_s Rtc::get_last_time( void )
{
    return last_time;
}

Rtc::datetime_bcd_s Rtc::time_convert_to_bcd( datetime_s * datetime )
{
    datetime_bcd_s datetime_bcd;
    datetime_bcd.sec = dec_2_bcd( datetime->sec );
    datetime_bcd.min = dec_2_bcd( datetime->min );
    datetime_bcd.hour = dec_2_bcd( datetime->hour );
    datetime_bcd.dayofweek = datetime->dayofweek;
    datetime_bcd.day = dec_2_bcd( datetime->day );
    datetime_bcd.month = dec_2_bcd( datetime->month );
    datetime_bcd.year = dec_2_bcd( datetime->year );
    return datetime_bcd;
}

Rtc::datetime_s Rtc::time_convert_from_bcd( datetime_bcd_s * datetime_bcd )
{
    datetime_s datetime;
    datetime.sec = bcd_2_dec( datetime_bcd->sec );
    datetime.min = bcd_2_dec( datetime_bcd->min );
    datetime.hour = bcd_2_dec( datetime_bcd->hour );
    datetime.dayofweek = datetime_bcd->dayofweek;
    datetime.day = bcd_2_dec( datetime_bcd->day );
    datetime.month = bcd_2_dec( datetime_bcd->month );
    datetime.year = bcd_2_dec( datetime_bcd->year );
    return datetime;
}

uint16_t Rtc::dec_2_bcd( uint16_t dec )
{
    return (( (dec/10)<<4) + (dec%10));
}

uint16_t Rtc::bcd_2_dec( uint16_t bcd )
{
    return (( (bcd>>4)*10) + (bcd%16));
}

