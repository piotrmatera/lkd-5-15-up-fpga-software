/*
 * common_utils.h
 *
 *  Created on: 30.03.2020
 *      Author: Piotr
 */

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

#include <string.h>

#define CLEAR_STRUCT( _struct ) memset( &(_struct), 0, sizeof(_struct))

/**@brief zamiana argumentu makra na string */
#define STRING(x) _STRING(x)
#define _STRING(x) #x

extern const char hex_digits[];

#define HEX_C( value ) _HEX_C( value )
#define _HEX_C( value ) 0x##value

#endif /* COMMON_UTILS_H_ */
