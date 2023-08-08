/*
 * SD_card.h
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#ifndef SD_CARD_H_
#define SD_CARD_H_

#include "State_master.h"
#include "State_slave.h"
#include "stdafx.h"
#include "ff.h"

#define WBUF_SIZE 100

class SD_card_class
{
    public:

    static struct CT_characteristic_struct CT_char;
    static struct harmonics_struct harmonics;
    static struct calibration_struct calibration;
    static struct settings_struct settings;
    static struct meter_struct meter;
    static Uint16 file_number_logs;
    static Uint16 file_number_errors;

    static Uint16 find_last_enumeration(Uint16 update, Uint16 *file_number, const char *head_filename, const char *scan_filename);
    static Uint16 log_data();
    static Uint16 save_state();

    static Uint16 save_settings();
    static Uint16 read_settings();
    static Uint16 read_CT_characteristic();
    static Uint16 save_H_settings();
    static Uint16 read_H_settings();
    static Uint16 save_calibration_data();
    static Uint16 read_calibration_data();
    static Uint16 save_meter_data();
    static Uint16 read_meter_data();

    static void Modbus_FatFS();
    static void Scope_snapshot_task();
    static Uint16 Scope_snapshot_state;
    static Uint16 save_error_state;
    static float Scope_input_last;

    private:

    static FRESULT fresult;
    static char working_buffer[WBUF_SIZE];
    static char filename_buffer[WBUF_SIZE];

    static void print_filinfo(Uint16 fno_p);
    static void save_single_state_ACDC(FIL *fil, union ALARM_ACDC alarm_ACDC_temp);
    static Uint32 save_FPGA_scope(FIL* fp, Uint32 index);

    static Uint16 *byte2_to_word2(Uint16 *word_array, Uint16 *byte_array, Uint16 size);
    static Uint16 *word2_to_byte2(Uint16 *byte_array, Uint16 *word_array, Uint16 size);
    static void save_memory(FIL* fp, Uint16 *source_address, Uint32 size);
    static Uint16 read_memory(FIL* fp, Uint16 *destination_address, Uint32 size);
    static void save_table(FIL* fp, const char *formatting, Uint16 max_columns, Uint16 max_rows, ...);
    static Uint16 read_table(FIL* fp, const char *formatting, Uint16 max_columns, Uint16 max_rows, ...);

    static void save_drivers_state(FIL *fil);
};

extern class SD_card_class SD_card;

#endif /* SD_CARD_H_ */
