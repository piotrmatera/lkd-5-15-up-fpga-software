/*
 * Modbus_ADU_slave_translator.h
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 *
 * Urzadzenie modbus z mozliwoscia translacji adresow i typow rejestrow
 */

#ifndef SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_TRANSLATOR_H_
#define SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_TRANSLATOR_H_

#include "stdafx.h"
#include "Modbus_RTU.h"
#include "ff.h"
#include "Modbus_ADU_slave_general.h"


class Modbus_ADU_slave_translated: public Modbus_ADU_slave_general
{
public:
    Modbus_ADU_slave_translated( Uint8 slave_address, Modbus_RTU_class *RTU, translator_map * translation_mapping, Modbus_translator::translator_mode_t translator_mode);
    void clear();

private:
    Modbus_error_enum_t Fcn_before_processed();
    void Fcn_after_processed();

};


#endif /* SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_TRANSLATOR_H_ */
