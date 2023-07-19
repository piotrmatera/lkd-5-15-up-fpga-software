/*
 * Dbg.h
 *
 *  Created on: 2 paü 2020
 *      Author: Piotr
 */

#ifndef SOFTWARE_DEBUG_DBG_H_
#define SOFTWARE_DEBUG_DBG_H_

#include "stdafx.h"
#include "status_codes.h"
#include "DbgGpio.h"

#define DBG_MARKER_ON 0

//#define __NOTHING do{}while(0)

#if DBG_MARKER_ON


#include "driver_rtc/Rtc.h"

# define __HOOK_SETUP_DEFAULT_ERROR GPIO_SET( LED4 )
//# define dbg_marker( x ) dbg.marker( x )
# define dbg_marker( x ) dbg.char_blocked( x )
# define DEFINE_DBG_CLASS Dbg dbg
# define DBG_CLASS_INIT() do{ dbg.init(); }while(0)

class Dbg{

public:
    Dbg( );

    status_code_t init();

    status_code_t marker( uint16_t type );

    void char_blocked( uint16_t type );

    void hex_blocked( uint16_t type );

    void dec_blocked( uint16_t type );

    void wait_txrdy(void);

};


void dbg_log_time_bcd( Rtc::datetime_bcd_s *now );

void dbg_log_time( Rtc::datetime_s *now );


extern Dbg dbg;


#else
# define __HOOK_SETUP_DEFAULT_ERROR __NOTHING()
# define dbg_marker( x ) __NOTHING()
# define DEFINE_DBG_CLASS
# define DBG_CLASS_INIT() __NOTHING()
# define dbg_log_time_bcd( now )
# define dbg_log_time( now )

#endif



#endif /* SOFTWARE_DEBUG_DBG_H_ */
