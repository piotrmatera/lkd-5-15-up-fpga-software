/*
 * nv_section_types.h
 *
 *  Created on: 27 paü 2023
 *      Author: Piotr
 */

#ifndef SOFTWARE_NV_SECTION_TYPES_H_
#define SOFTWARE_NV_SECTION_TYPES_H_


typedef enum{
        sec_CT_characteristic,
        sec_settings,
        sec_H_settings,
        sec_calibration_data,
        sec_meter_data,

        sec_type_max
    } section_type_t;

#define NV_IN_EEPROM 1

#if NV_IN_EEPROM
//przelaczalny IF

class nv_data_t{
public:
        Uint16 read( section_type_t section );

        Uint16 save( section_type_t section );

private:

   //funkcje zapisuja dane do SD_card
        Uint16 read_settings();
        Uint16 read_H_settings();
        Uint16 read_calibration_data();
        Uint16 read_meter_data();
        Uint16 read_CT_characteristic();

   //funkcje odczytuja dane do obiektu SD_card
        Uint16 save_settings();
        Uint16 save_H_settings();
        Uint16 save_calibration_data();
        Uint16 save_meter_data();



};

extern nv_data_t nv_data;

# define NV_CLASS_read( section ) nv_data.read( section )
# define NV_CLASS_save( section ) nv_data.save( section )
#else
# define NV_CLASS_read( section ) SD_card.read( section )
# define NV_CLASS_save( section ) SD_card.save( section )
#endif

#endif /* SOFTWARE_NV_SECTION_TYPES_H_ */
