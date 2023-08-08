/*
 * Modbus_devices.cpp
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */
#include "Modbus_devices.h"
#include "Modbus_translator.h"

//interfejsy sprzetowe modbus
Modbus_RTU_class RTU_LCD;
Modbus_RTU_class RTU_EXT;
Modbus_RTU_class RTU_FIBER;

// Urzadzenia modbus
// 1. interfejs do wifi - urzadzenie pelne z FatFS
Modbus_ADU_slave_transparent Modbus_slave_LCD( MODBUS_SLAVE_LCD_ARGS );

// 2a. interfejs zewnetrzny - urzadzenie pelne
Modbus_ADU_slave_transparent Modbus_slave_EXT( MODBUS_SLAVE_EXT_ARGS );

// 2b. jw, interfejs uproszczony
Modbus_ADU_slave_translated Modbus_slave_EXT_translated( MODBUS_SLAVE_EXT_TRANSLATED_ARGS );

// 3. interfejs do wifi poprzez swiatlowod - urzadzenie pelne z FatFS
Modbus_ADU_slave_transparent Modbus_slave_FIBER( MODBUS_SLAVE_FIBER_ARGS );

// wskaznik typu general aby modbus_transparent uzyskal dostep do translated
// i zmienil mu server-id
Modbus_ADU_slave_general * pModbus_slave_EXT_translated = &Modbus_slave_EXT_translated;
