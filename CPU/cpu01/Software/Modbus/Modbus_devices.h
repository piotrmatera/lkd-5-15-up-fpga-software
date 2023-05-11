/*
 * Modbus_devices.h
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */

#ifndef SOFTWARE_MODBUS_MODBUS_DEVICES_H_
#define SOFTWARE_MODBUS_MODBUS_DEVICES_H_


#include <Modbus_ADU_slave_transparent.h>
#include "Modbus_ADU_slave_translated.h"


extern Modbus_ADU_slave_transparent Modbus_slave_LCD;
extern Modbus_ADU_slave_transparent Modbus_slave_LCD_OLD;
extern Modbus_ADU_slave_transparent Modbus_slave_EXT;
extern Modbus_ADU_slave_translated Modbus_slave_EXT_translated;

extern Modbus_RTU_class RTU_LCD;
extern Modbus_RTU_class RTU_EXT;
extern Modbus_RTU_class RTU_LCD_OLD; //RTU na starym porcie

#define MODBUS_SLAVE_LCD_ARGS \
    1, &RTU_LCD

#define MODBUS_SLAVE_LCD_OLD_ARGS \
    1, &RTU_LCD_OLD

#define MODBUS_SLAVE_EXT_ARGS \
        MODBUS_EXT_ADDRESS, &RTU_EXT

#define MODBUS_SLAVE_EXT_TRANSLATED_ARGS \
    1, &RTU_EXT, &Mdb_translator_map, Modbus_translator::translator_active

#define MODBUS_EXT_ADDRESS 240
// MODBUS_SLAVE_EXT_TRANSLATED_ARGS - ustawiane w SD_card odczyt z pliku settings
// zakres 1-230
// wywolanie w SMachine_main w stanie init

#endif /* SOFTWARE_MODBUS_MODBUS_DEVICES_H_ */
