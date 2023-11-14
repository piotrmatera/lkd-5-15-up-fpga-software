/*
 * status_codes.h
 *
 *  Created on: 2 paü 2020
 *      Author: Piotr
 */

#ifndef SOFTWARE_COMMON_STATUS_CODES_H_
#define SOFTWARE_COMMON_STATUS_CODES_H_


typedef enum{
        status_ok = 0,
        status_inactive,
        status_terminated,
        status_continue,

        err_nv_invalid = 10,
        err_nv_unknown = 11,
        err_nv_no_valid_copy = 100,
        err_nv_timeout = 200,

        err_generic = 0x80,
        err_timeout,
        err_busy,
        err_bus_busy,
        err_stop_not_ready,
        err_invalid

    } status_code_t;



#endif /* SOFTWARE_COMMON_STATUS_CODES_H_ */
