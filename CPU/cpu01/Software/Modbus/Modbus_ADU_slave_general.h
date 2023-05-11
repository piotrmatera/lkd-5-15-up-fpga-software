/*
 * Modbus_ADU_slave_general.h
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 *
 *  Uogolnienie urzadzenia modbus - klasa bazowa dla:
 *  a) urzadzenia z obsluga modbus
 *  b) urzadzenie z ograniczonym dostepem
 */

#ifndef SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_GENERAL_H_
#define SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_GENERAL_H_

#include "stdafx.h"
#include "Modbus_RTU.h"
#include "Modbus_translator.h"
#include "Modbus_ADU_slave.h"
#include "Modbus_ADU_api.h"

class Modbus_ADU_slave_general{
protected:
    Modbus_translator::translator_mode_t translator_mode;
    translator_map * translation_mapping;

public:
    Modbus_ADU_slave_general( Uint8 slave_address, Modbus_RTU_class *RTU, translator_map * translation_mapping, Modbus_translator::translator_mode_t translator_mode );

    virtual  ~Modbus_ADU_slave_general();

    void init( Uint8 slave_address, Modbus_RTU_class *RTU,
               translator_map * translation_mapping = NULL,
               Modbus_translator::translator_mode_t translator_mode = Modbus_translator::translator_disabled);

    class Modbus_RTU_class *RTU;

    virtual mdb_prepare_status_t task();

    Uint8 slave_address;

    virtual void clear() = 0;

protected:
    virtual Modbus_error_enum_t Fcn_before_processed() = 0;
    virtual void Fcn_after_processed() = 0;


};


#endif /* SOFTWARE_MODBUS_MODBUS_ADU_SLAVE_GENERAL_H_ */
