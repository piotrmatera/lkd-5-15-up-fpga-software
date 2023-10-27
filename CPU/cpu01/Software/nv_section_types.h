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

//przygotowanie do przelaczalnego IF
#define NV_CLASS_read( section ) SD_card.read( section )

#define NV_CLASS_save( section ) SD_card.save( section )

#endif /* SOFTWARE_NV_SECTION_TYPES_H_ */
