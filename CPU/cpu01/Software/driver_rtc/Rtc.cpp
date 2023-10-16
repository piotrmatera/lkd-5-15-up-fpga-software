/*
 * rtc.cpp
 *
 *  Created on: 2 Nov 2020
 *      Author: Piotr
 */
#include <string.h>
#include "Rtc.h"
#include "debug/Dbg.h"


Rtc::Rtc()
{
    memset( &last_time_bcd, 0, sizeof(last_time_bcd) );
    state = state_reset;
    i2c_bus = NULL;
}

status_code_t Rtc::init( i2c_transactions_t * i2c_bus )
{
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

void Rtc::process()
{
    process_event( event_periodic );
}

status_code_t Rtc::process_event( event_t event, void * xdata )
{
   // status_code_t retc = err_invalid;

    if( state != state_idle && event != event_periodic )
        return err_invalid;

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
            msg_factory(get_time, &x_msg_gettime);
            if( i2c_bus->start_transaction( &x_msg_gettime ) )
                return err_busy;

            change_state_to( state_read_time );
            break;


            //TODO zrobic jeden bufor wiadomosci/ew kilka (malo)
            //     przygotowywac wiadomosci o okreslonej tresci - factory(&msg_buff)
            //     za duzo buforow teraz

        case event_init:
            msg_factory(get_status, &x_msg_getstatus);
            if( i2c_bus->start_transaction( &x_msg_getstatus ) )
                return err_busy;

            change_state_to( state_read_reg0 );
            break;

        case event_setup_time:
            new_time = time_convert_to_bcd( (Rtc::datetime_s *)xdata );
            goto l_common;

        case event_setup_time_bcd:
            {
            new_time = *((Rtc::datetime_bcd_s *)xdata);
        l_common:
            msg_factory(set_time, &x_msg_settime);
            if( prepare_setup_msg( &x_msg_settime, &new_time )!=status_ok )
                return err_invalid;

            msg_factory(stop_rtc, &x_msg_rtc);
            if( i2c_bus->start_transaction( &x_msg_rtc  ))
                return err_busy;

            change_state_to( state_new_time );
            }
            break;

        default:
            return err_invalid;
        }

    }
    break;

    case state_read_time:
        if( x_msg_gettime.state == i2c_transaction_buffer::done ){
            //tu juz zakonczono odczyt
            time_is_read( &x_msg_gettime.msg );
            change_state_to( state_idle );
        }else if( x_msg_gettime.state == i2c_transaction_buffer::error ){
            change_state_to( state_error );
        }
        break;


    case state_read_reg0:
        if( x_msg_getstatus.state == i2c_transaction_buffer::error ){
             change_state_to( state_error );
             break;
        }
        if( x_msg_getstatus.state == i2c_transaction_buffer::done ){
             //tu juz zakonczono odczyt
            if( x_msg_getstatus.msg.data[0] & MCP7940N_ST_MASK ){  //czy wlaczony zegar (REG0.ST=1?)
                dbg_marker('E');
                change_state_to( state_idle );
                break;
            }
            //jesli niewlaczony, to ustawienie domyslnego czasu i wystartowanie
            dbg_marker('e');
            change_state_to( state_setup_default );
            //TODO nie mozna zbyt szybko wysylac w tym miejscu - nie wysylalo [dlaczego?]
        }

        break;

    case state_setup_default:
        msg_factory(start_rtc0, &x_msg_rtc);
        if( i2c_bus->start_transaction( &x_msg_rtc ))
            return err_busy;
        //jak sie nie uruchomi to niewiele mozna zrobic
//        if( retc != status_ok ){
//            __HOOK_SETUP_DEFAULT_ERROR;
//            return retc;
//        }
        change_state_to( state_setup_default_wait_for_done );
        break;
    case state_setup_default_wait_for_done:
        if( x_msg_rtc.state == i2c_transaction_buffer::error ){
             change_state_to( state_error );
             break;
        }
        if( x_msg_rtc.state == i2c_transaction_buffer::done ){
            change_state_to( state_idle );
        }
        break;

    case state_new_time:
        if( x_msg_rtc.state == i2c_transaction_buffer::error ){
             change_state_to( state_error );
             break;
        }
        if( x_msg_rtc.state == i2c_transaction_buffer::done ){
            //wykonano ST=0
            change_state_to( state_new_time1 );
        }
        break;
    case state_new_time1:
        msg_factory(get_oscrun, &x_msg_getosc);
        if( i2c_bus->start_transaction( &x_msg_getosc )) //odczytac stan OSCRUN
            return err_busy;
        change_state_to( state_new_time4 );
        break;

    case state_new_time4:
        if( x_msg_getosc.state == i2c_transaction_buffer::error ){
             change_state_to( state_error );
             break;
        }
        if( x_msg_getosc.state == i2c_transaction_buffer::done ){
            if( !(x_msg_getosc.msg.data[0] & MCP7940N_WEEKDAY_OSCRUN_MASK) ){
                //oscylator zatrzymany
                change_state_to( state_new_time5 );
            }else{
                //osc jeszcze pracuje ponowic odczyt
                change_state_to( state_new_time1 );
            }
        }
        break;

    case state_new_time5:
        if( i2c_bus->start_transaction( &x_msg_settime )) //ustawienie czasu (wiadomosc przygotowana wczesniej)
             return err_busy;
        change_state_to( state_new_time6 );
        break;

    case state_new_time6:
        if( x_msg_settime.state == i2c_transaction_buffer::error ){
             change_state_to( state_error );
             break;
        }
        if( x_msg_settime.state == i2c_transaction_buffer::done ){
            prepare_start_msg( &x_msg_settime ); //w tej samej wiadomosci wlaczenie zegara reg0.ST:=1 aby nie zmienic sekund
            if( i2c_bus->start_transaction( &x_msg_settime ))
                 return err_busy;

            change_state_to( state_new_time7 );
        }
        break;
    case state_new_time7:
        if( x_msg_settime.state == i2c_transaction_buffer::error ){
             change_state_to( state_error );
             break;
        }
        if( x_msg_settime.state == i2c_transaction_buffer::done ){
            change_state_to( state_idle );
        }
        break;
    case state_error:
        //obecnie w przypadku bledu przechodzi do state_idle
        dbg_marker('R');
        change_state_to( state_idle );
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

status_code_t Rtc::prepare_setup_msg( i2c_transaction_buffer *msg_buff, datetime_bcd_s * new_time ){
    if( new_time->sec >= 0x60 ) return err_invalid;
    if( new_time->min >= 0x60 ) return err_invalid;
    if( new_time->hour >= 0x23 ) return err_invalid;
    if( (new_time->dayofweek==0) || (new_time->dayofweek>7)) return err_invalid;
    if( (new_time->day==0) || (new_time->day>0x31)) return err_invalid;//TODO mozna testwoac miesiace i lata przestepne
    if( (new_time->month==0) || (new_time->month>0x12)) return err_invalid;

    //+1 bo jest adress @[0]
    msg_buff->msg.data[ MCP7940N_SEC_REG+1 ] = new_time->sec;
    msg_buff->msg.data[ MCP7940N_MIN_REG+1 ] = new_time->min;
    msg_buff->msg.data[ MCP7940N_HOUR_REG+1 ] = new_time->hour;
    msg_buff->msg.data[ MCP7940N_WEEKDAY_REG+1 ] = new_time->dayofweek | MCP7940N_WEEKDAY_VBATEN_MASK;
    msg_buff->msg.data[ MCP7940N_DAY_REG+1 ] = new_time->day;
    msg_buff->msg.data[ MCP7940N_MONTH_REG+1 ] = new_time->month;
    msg_buff->msg.data[ MCP7940N_YEAR_REG+1 ] = new_time->year;
    msg_buff->len_out = 8;
    return status_ok;
}

status_code_t Rtc::prepare_start_msg( i2c_transaction_buffer * buffer ){
    buffer->msg.data[ MCP7940N_SEC_REG+1 ] |= MCP7940N_ST_MASK;
    buffer->len_out = 2;
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

void Rtc::msg_factory( msg_type_t msg_type, i2c_transaction_buffer * buffer )
{
    buffer->slave_address = MCP7940N_ADDRESS;
    buffer->state = i2c_transaction_buffer::idle;

    switch(msg_type){
    case start_rtc0: /**<@brief uruchomienie RTC i ustawienie domyslnego czasu*/
    {
        buffer->len_out = 8;
        buffer->len_in = 0;
        buffer->type = i2c_transaction_buffer::write_only;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_SEC_REG, MCP7940N_ST_MASK|0x00 /*sec*/, 0x00/*min*/, 0x00/*hr*/,
                                               MCP7940N_WEEKDAY_VBATEN_MASK|0x01, 0x01/*day*/, 0x01/*month*/, 0x00 /*year*/}};
        buffer->msg = msg;
        break;
    }
    case start_rtc:  /**<@brief uruchomienie osc. RTC*/
    {
        buffer->len_out = 2;
        buffer->len_in = 0;
        buffer->type = i2c_transaction_buffer::write_only;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_SEC_REG, MCP7940N_ST_MASK }};
        buffer->msg = msg;
        break;
    }
    case stop_rtc:   /**<@brief zatrzymanie osc. RTC*/
    {
        buffer->len_out = 2;
        buffer->len_in = 0;
        buffer->type = i2c_transaction_buffer::write_only;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_SEC_REG, 0 }};
        buffer->msg = msg;
        break;
    }
    case set_time:   /**<@brief ustawienie nowego czasu*/
    {
        buffer->len_out = 8;
        buffer->len_in = 0;
        buffer->type = i2c_transaction_buffer::write_only;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_SEC_REG, 0 }}; //TODO upewnic sie ze ST jest ustawiany
        buffer->msg = msg;
        break;
    }
    case get_time:   /**<@brief pobranie czasu z RTC*/
    {
        buffer->len_out = 1;
        buffer->len_in = 7;
        buffer->type = i2c_transaction_buffer::write_nostop_read;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_SEC_REG, 0 }};
        buffer->msg = msg;
        break;
    }
    //TODO czy odczyt obu ST i OSCRUN jest potrzebny?
    case get_status: /**<@brief pobranie statusu (reg0.ST)*/
    {
        buffer->len_out = 1;
        buffer->len_in = 1;
        buffer->type = i2c_transaction_buffer::write_nostop_read;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_SEC_REG, 0 }};
        buffer->msg = msg;
        break;
    }
    case get_oscrun:  /**<@brief pobranie stanu oscylatora (reg3.OSCRUN)*/
    {
        buffer->len_out = 1;
        buffer->len_in = 1;
        buffer->type = i2c_transaction_buffer::write_nostop_read;
        /**@brief wiadomosc do uruchomienia zegara w RTC, wlaczenia baterii i ustawienia czasu domyslnego 0:00:00 pn {20}00-01-01
         * @note uzywane przy pierwszym uruchomieniu, gdy RTC nie pracowalo, albo po wymianie baterii
         */
        const struct msg_buffer msg = { 0, 0, {MCP7940N_WEEKDAY_REG, 0 }};
        buffer->msg = msg;
        break;
    }
    default:
        break;
    }
}
