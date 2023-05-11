/*
 * time_bcd_struct.h
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */

#ifndef TIME_BCD_STRUCT_H_
#define TIME_BCD_STRUCT_H_

#include "stdafx.h"

struct time_BCD_struct
{
    Uint16 second : 4;
    Uint16 second10 : 4;
    Uint16 minute : 4;
    Uint16 minute10 : 4;
    Uint16 hour : 4;
    Uint16 hour10 : 4;
    Uint16 day : 4;
    Uint16 day10 : 4;
    Uint16 month : 4;
    Uint16 month10 : 4;
    Uint16 year : 4;
    Uint16 year10 : 4;
};


#endif /* TIME_BCD_STRUCT_H_ */
