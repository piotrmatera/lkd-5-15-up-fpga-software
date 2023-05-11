/*
 * Modbus_ADU_api.h
 *
 *  Created on: 5 wrz 2021
 *      Author: Piotr
 */

#ifndef SOFTWARE_MODBUS_MODBUS_ADU_API_H_
#define SOFTWARE_MODBUS_MODBUS_ADU_API_H_

#include "Modbus_ADU_slave_context.h"

extern Modbus_ADU_slave_context_struct Mdb_slave_ADU;


typedef enum mdb_prepare_status_e{
    mdb_request_valid = 0,
    mdb_request_wrong_address,
    mdb_request_too_short,
    mdb_request_crc_error,
    mdb_no_request,
    mdb_request_error,

} mdb_prepare_status_t;


mdb_prepare_status_t MdbSlavePrepareRequest(struct Modbus_slave_info_struct *slave_info, translator_map * translation_mapping = NULL,
                            Modbus_translator::translator_mode_t mode = Modbus_translator::translator_disabled );
void MdbSlaveProcessRequest();



#endif /* SOFTWARE_MODBUS_MODBUS_ADU_API_H_ */
