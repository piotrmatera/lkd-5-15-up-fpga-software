/*
 * Translator_map.h
 *
 *  Created on: 8 wrz 2021
 *      Author: Piotr
 */

#ifndef SOFTWARE_MODBUS_TRANSLATOR_MAP_H_
#define SOFTWARE_MODBUS_TRANSLATOR_MAP_H_

#include "stdafx.h"

/** @brief klasa do przechowywania definicji translacji rejestrow Modbus
 * */

class translator_map{
public:
    /** typy translacji */
    typedef enum{
         undefined,
         int16_to_int16,
         int32_to_low_int16,
         int32_to_high_int16,
         float_to_int16,

         max2int16_to_int16,

         time_to_hour,
         time_to_min,
         time_to_sec,
         time_to_year,
         time_to_month,
         time_to_day,

         spec_get_ilim, //pobiera Ilim z settings
    } translation_type_t;

    /**Deskryptor - Element mapy translacji - opis dla pojedynczego adresu Modbus widzianego z zewnatrz*/
    struct translator_map_item{
        translation_type_t translation_type;
        Uint16 source_register_index;
        const void * args;
    };


private:
    /** tresc mapy translacji, gdyby bylo wiecej instancji mapy - TODO zrobic inicjalizacje const zamiast static*/
    static const struct translator_map_item translator_map_table[];
    static const Uint16 translator_map_items;

    /**gdy brak opisu - zwracany ponizszy descryptor*/
    static const struct translator_map_item undefined_dsc;
public:


    translator_map(){};
    /**pobranie deskryptora translacji dla podanego adresu modbus*/
    const struct translator_map_item * get_translation_descriptor( Uint16 index );
    Uint16 get_map_items();
};

/**Instancja mapy - obecnie jedna*/
extern translator_map Mdb_translator_map;

#endif /* SOFTWARE_MODBUS_TRANSLATOR_MAP_H_ */
