/*
 * Modbus_translator.h
 *
 *  Created on: 5 wrz 2021
 *      Author: Piotr
 *
 * Adapter tlumaczacy rejestry
 * na zewnatrz widac inne (typy, kolejnosc, zbior) rejestry Modbus niz jest pod spodem w pamieci
 *
 * translacja odbywa sie w momencie odwolania operatorem [] - ukryta
 */

#ifndef SOFTWARE_MODBUS_MODBUS_TRANSLATOR_H_
#define SOFTWARE_MODBUS_MODBUS_TRANSLATOR_H_

#include "stdafx.h"
#include "Modbus_ADU_Slave.h"
#include "Translator_map.h"


class Modbus_translator{
public:
    typedef enum { translator_active,  //wykonuje translacje zgodnie z mapa translacji
                   translator_disabled //bezposrednio przekazuje dane bez translacji
    } translator_mode_t;

private:
    struct Modbus_slave_info_struct * slave_info;
    translator_map * translation_mapping;
    translator_mode_t mode;

public:

    Modbus_translator();

/**@brief
 * @param[in] slave_info struktura opisujaca zrodlo danych
 * @param[in] translation_mapping opis regul tlumaczenia
 * @param[in]  mode wybor sposobu zachowania
 */
    void Init( struct Modbus_slave_info_struct * slave_info,
               translator_map * translation_mapping,
               translator_mode_t mode
    );

    Uint16 operator[]( size_t index );
};




#endif /* SOFTWARE_MODBUS_MODBUS_TRANSLATOR_H_ */
