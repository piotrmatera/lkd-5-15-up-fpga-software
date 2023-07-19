/*
 * Dbg.cpp
 *
 *  Created on: 2 paü 2020
 *      Author: Piotr
 */


#include "Dbg.h"
#include "status_codes.h"

#if DBG_MARKER_ON

DEFINE_DBG_CLASS;

#if defined( BOARD_TEXAS_INSTRUMENTS_MOSFETDRV )
# define SciRegs SciaRegs
#elif defined( BOARD_INFINEON_MOSFETDRV )
# define SciRegs ScibRegs
#endif

#if defined( BOARD_TEXAS_INSTRUMENTS_MOSFETDRV )
# define Debug_RX RX1
#endif
#define Debug_TX TX1

#define LSPCLK 50e6
#define CPU_CLK 200e6
#define CALC_BAUDRATE(x) (LSPCLK/((float)x*8.0f)-1.0f)
#define BAUDRATE 2000000 //115200
static Uint16 Baudrate_reg = CALC_BAUDRATE(BAUDRATE);

Dbg::Dbg( ){
}

status_code_t Dbg::init()
{

    GPIO_Setup(Debug_TX);
    //GPIO_Setup(Modbus_RX);

//    while(1){
//        GPIO_CLEAR(Debug_TX);
//        GPIO_SET(Debug_TX);
//    }

    EALLOW;
    CpuSysRegs.PCLKCR7.all |= 1<<(&SciRegs - &SciaRegs);
    EDIS;

    SciRegs.SCICTL1.bit.SWRESET = 0;
    SciRegs.SCIFFTX.bit.SCIRST = 0;
    SciRegs.SCIFFTX.bit.TXFIFORESET = 0;
    //SciRegs.SCIFFRX.bit.RXFIFORESET = 0;

    SciRegs.SCICCR.all = 0x0007;      // 1 stop bit,  No loopback
                                     // No parity,8 char bits,
                                     // async mode, idle-line protocol
    SciRegs.SCICTL1.bit.TXENA = 1;
    //SciRegs.SCICTL1.bit.RXENA = 1;

    SciRegs.SCIHBAUD.bit.BAUD = Baudrate_reg >> 8;
    SciRegs.SCILBAUD.bit.BAUD = Baudrate_reg;
    SciRegs.SCIFFTX.bit.SCIFFENA = 1;

    SciRegs.SCICTL1.bit.SWRESET = 1;
    SciRegs.SCIFFTX.bit.SCIRST = 1;
    SciRegs.SCIFFTX.bit.TXFIFORESET = 1;
    //SciRegs.SCIFFRX.bit.RXFIFORESET = 1;

    SciRegs.SCITXBUF.all = 0x55;
    return status_ok;
}


status_code_t Dbg::marker( uint16_t type ){
    SciRegs.SCITXBUF.all = type;
    return status_ok;
}

void Dbg::wait_txrdy(void)
{
    while( !SciRegs.SCICTL2.bit.TXRDY );
}

void Dbg::char_blocked( uint16_t type )
{
    this->wait_txrdy();
    marker( type );
}

void Dbg::hex_blocked( uint16_t type )
{
    char_blocked( type>>4 );
    char_blocked( type & 7 );
}

void Dbg::dec_blocked( uint16_t type )
{
    char_blocked( type/10 );
    char_blocked( type%10 );

}

void dbg_log_time_bcd( Rtc::datetime_bcd_s *now )
{
    dbg.hex_blocked( 0x20 );
    dbg.hex_blocked( now->year );
    dbg.char_blocked( '-' );
    dbg.hex_blocked( now->month );
    dbg.char_blocked( '-' );
    dbg.hex_blocked( now->day );
    dbg.char_blocked( '/' );
    dbg.char_blocked( '0'+(now->dayofweek&7) );
    dbg.char_blocked( '/' );
    dbg.hex_blocked( now->hour );
    dbg.char_blocked( ':' );
    dbg.hex_blocked( now->min );
    dbg.char_blocked( ':' );
    dbg.hex_blocked( now->sec );
}

void dbg_log_time( Rtc::datetime_s *now )
{
    dbg.hex_blocked( 0x20 );
    dbg.dec_blocked( now->year );
    dbg.char_blocked( '-' );
    dbg.dec_blocked( now->month );
    dbg.char_blocked( '-' );
    dbg.dec_blocked( now->day );
    dbg.char_blocked( '/' );
    dbg.char_blocked( '0'+(now->dayofweek&7) );
    dbg.char_blocked( '/' );
    dbg.dec_blocked( now->hour );
    dbg.char_blocked( ':' );
    dbg.dec_blocked( now->min );
    dbg.char_blocked( ':' );
    dbg.dec_blocked( now->sec );
}


#endif
