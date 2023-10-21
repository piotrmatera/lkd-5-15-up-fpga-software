/*
 * nonvolatile_sections.h
 *
 *  Created on: 17 pa� 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_NONVOLATILE_SECTIONS_H_
#define SOFTWARE_NONVOLATILE_SECTIONS_H_

#include "nonvolatile.h"

#define NV_ONOFF_SWITCH_TYPE 0
#define NV_LGRID_TYPE 1
#define NV_ERROR_RETRY_TYPE 2

#define ENABLE_NV_TIMEOUTS 0
#if ENABLE_NV_TIMEOUTS
# define NV_ONOFF_SAVE_TIMEOUT 40 //ms
# define NV_LGRID_SAVE_TIMEOUT 100 //ms
# define NV_ERROR_RETRY_SAVE_TIMEOUT 40 //ms

# define NV_ONOFF_READ_TIMEOUT 10 //ms
# define NV_LGRID_READ_TIMEOUT 20 //ms
# define NV_RETRY_READ_TIMEOUT_ERROR 10 //ms

# define NV_READ_INFO 50
#else

# define NV_ONOFF_SAVE_TIMEOUT 0 //ms
# define NV_LGRID_SAVE_TIMEOUT 0 //ms
# define NV_ERROR_RETRY_SAVE_TIMEOUT 0 //ms

# define NV_ONOFF_READ_TIMEOUT 0 //ms
# define NV_LGRID_READ_TIMEOUT 0 //ms
# define NV_RETRY_READ_TIMEOUT_ERROR 0 //ms

# define NV_READ_INFO 0
#endif


extern const class nonvolatile_t nonvolatile;




#endif /* SOFTWARE_NONVOLATILE_SECTIONS_H_ */