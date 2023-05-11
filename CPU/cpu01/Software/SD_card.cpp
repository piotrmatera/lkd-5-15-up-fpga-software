/*
 * SD_card.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>

#include "stdafx.h"

#include "Modbus_Converter_memory.h"
#include "ff.h"
#include "diskio.h"
#include "Scope.h"
#include "SD_card.h"
#include "State.h"

#include "Fiber_comm_master.h"

extern FATFS fs;
class SD_card_class SD_card;

struct CT_characteristic_struct SD_card_class::CT_char;
struct harmonics_struct SD_card_class::harmonics;
struct calibration_struct SD_card_class::calibration;
struct settings_struct SD_card_class::settings;
struct meter_struct SD_card_class::meter;
FRESULT SD_card_class::fresult = FR_OK;
char SD_card_class::working_buffer[WBUF_SIZE];
char SD_card_class::filename_buffer[WBUF_SIZE];
Uint16 SD_card_class::file_number_logs = 0;
Uint16 SD_card_class::file_number_errors = 0;
Uint16 SD_card_class::Scope_snapshot_state = 0;
float SD_card_class::Scope_input_last = 0;
Uint16 SD_card_class::save_error_state = 0;

static int compare_float (const void * a, const void * b)
{
    if ( *(float*)a <  *(float*)b ) return -1;
    else if ( *(float*)a >  *(float*)b ) return 1;
    else return 0;
}

Uint16 *SD_card_class::byte2_to_word2(Uint16 *word_array, Uint16 *byte_array, Uint16 input_word_size)
{
    register Uint16 *destination_pointer = word_array;
    register Uint16 *source_pointer = byte_array;

    for(Uint16 i = 0;i<input_word_size;i++)
    {
        *destination_pointer++ = __byte((int *)source_pointer,0);
        *destination_pointer++ = __byte((int *)source_pointer++,1);
    }
    return source_pointer;
}

Uint16 *SD_card_class::word2_to_byte2(Uint16 *byte_array, Uint16 *word_array, Uint16 output_word_size)
{
    register Uint16 *source_pointer = word_array;
    register Uint16 *destination_pointer = byte_array;

    for(Uint16 i = 0;i<output_word_size;i++)
    {
        __byte((int *)destination_pointer,0) = *source_pointer++;
        __byte((int *)destination_pointer++,1) = *source_pointer++;
    }
    return destination_pointer;
}

void SD_card_class::save_memory(FIL* fp, Uint16 *source_address, Uint32 size)
{
    const Uint16 buffer_size = 100;
    static Uint16 buffer[buffer_size];
    Uint16 bw;

    while(size)
    {
        Uint32 wtw = size > buffer_size/2 ? buffer_size/2 : size;
        if(alarm_master.bit.FLT_SUPPLY_MASTER) return;
        source_address = byte2_to_word2(buffer, source_address, wtw);
        fresult = f_write(fp, buffer, wtw*2, &bw);
        size -= wtw;
    }
}

Uint16 SD_card_class::read_memory(FIL* fp, Uint16 *destination_address, Uint32 size)
{
    const Uint16 buffer_size = 100;
    static Uint16 buffer[buffer_size];
    Uint16 br;

    while(size)
    {
        Uint32 btr = size > buffer_size/2 ? buffer_size/2 : size;
        btr <<= 1;
        fresult = f_read(fp, buffer, btr, &br);
        size -= br>>1;
        destination_address = word2_to_byte2(destination_address, buffer, br>>1);
        if(btr != br) return size;
    }

    return size;
}

void SD_card_class::save_table(FIL* fp, const char *formatting, Uint16 max_columns, Uint16 max_rows, ...)
{
//    ESTOP0;
    va_list vl;
    va_start(vl, max_rows);

    float **columns;
    columns = (float **) malloc(max_columns * sizeof(float *));
    if (columns==NULL) abort();

    for (Uint16 i = 0;i<max_columns;i++)
    {
        columns[i] = va_arg(vl,float *);
    }
    va_end(vl);

    for(Uint16 i = 0; i < max_rows;i++)
    {
        for (Uint16 j = 0;j<max_columns;j++)
        {
            snprintf(working_buffer, WBUF_SIZE, formatting, *(columns[j]++));
            f_puts(working_buffer, fp);
            f_putc(';', fp);
        }
        f_lseek(fp, f_tell(fp) - 1);
        f_putc('\n', fp);
    }
    free(columns);
}

Uint16 SD_card_class::read_table(FIL* fp, const char *formatting, Uint16 max_columns, Uint16 max_rows, ...)
{
    va_list vl;
    va_start(vl, max_rows);

    float **columns;
    columns = (float **) malloc(max_columns * sizeof(float *));
    if (columns==NULL) abort();

    for (Uint16 i = 0;i<max_columns;i++)
    {
        columns[i] = va_arg(vl,float *);
    }
    va_end(vl);
    Uint16 i;
    for(i = 0; i < max_rows;i++)
    {
        if(!f_gets(working_buffer, WBUF_SIZE, fp)) break;
        char *pch = working_buffer;
        for (Uint16 j = 0;j<max_columns;j++)
        {
            sscanf(pch, formatting, columns[j]++);
            pch = strchr(pch,';');
            if(pch == NULL) break;
            pch += 1;
        }
    }
    free(columns);
    return i;
}

void SD_card_class::Scope_snapshot_task()
{
    static FIL fil;
    static Uint16 acquire_before_trigger;
    static Uint32 timeout_counter;
    static Uint16 current_node;

    switch(Scope_snapshot_state)
    {
        case 0:
        {
            if(control_master.triggers.bit.Scope_snapshot && !control_master.triggers.bit.CPU_reset)
            {
                control_master.triggers.bit.Scope_snapshot = 0;
                acquire_before_trigger = scope_global.acquire_before_trigger;
                static volatile Uint16 acquire_before = 3;
                scope_global.acquire_before_trigger = acquire_before;
                Scope_input_last = Kalman_U_grid[0].states[2];
                timeout_counter = IpcRegs.IPCCOUNTERL;
                Scope_snapshot_state++;
            }
            break;
        }
        case 1:
        {
            if(IpcRegs.IPCCOUNTERL - timeout_counter > 8000000UL)//40ms
                scope_global.scope_trigger = 1;

            if(Scope.finished_sorting)
            {
                if(fresult = f_open(&fil, "scope.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
                {
                    Scope_snapshot_state = 100;
                    f_close(&fil);
                }
                else
                {
                    for(Uint16 i = 0; i<6; i++)
                    {
                        save_memory(&fil, (Uint16 *)&Scope.data[i], sizeof(Scope.data[0]));
                        Fiber_comm[0].Main();
                        Fiber_comm[1].Main();
                        Fiber_comm[2].Main();
                        Fiber_comm[3].Main();
                    }
                    memset(&Scope.data, 0, sizeof(Scope.data));
                    f_close(&fil);
                    current_node = 0;
                    Fiber_comm[current_node].input_flags.read_scope = 1;
                    Scope_snapshot_state++;
                }
            }
            break;
        }
        case 2:
        {
            if(!Fiber_comm[current_node].input_flags.read_scope)
            {
                if((fresult = f_open(&fil, "scope.bin", FA_READ | FA_WRITE | FA_OPEN_APPEND)) || alarm_master.bit.FLT_SUPPLY_MASTER)
                {
                    Scope_snapshot_state = 100;
                    f_close(&fil);
                }
                else
                {
                    for(Uint16 i = 0; i<6; i++)
                    {
                        save_memory(&fil, (Uint16 *)&Scope.data[i], sizeof(Scope.data[0]));
                        Fiber_comm[0].Main();
                        Fiber_comm[1].Main();
                        Fiber_comm[2].Main();
                        Fiber_comm[3].Main();
                    }
                    f_close(&fil);
                    if(++current_node >= 4)
                    {
                        status_master.Scope_snapshot_error = 0;
                        Scope.acquire_before_trigger = acquire_before_trigger;
                        scope_global.scope_trigger = 0;
                        Scope_snapshot_state = 0;
                    }
                    else
                    {
                        memset(&Scope.data, 0, sizeof(Scope.data));
                        Fiber_comm[current_node].input_flags.read_scope = 1;
                    }
                }
            }
            break;
        }
        default:
        {
            status_master.Scope_snapshot_error = 1;
            scope_global.scope_trigger = 0;
            Scope_snapshot_state = 0;
            break;
        }
    }
}

Uint16 SD_card_class::find_last_enumeration(Uint16 update, Uint16 *file_number, const char *head_filename, const char *scan_filename)
{
    FIL fil;

    if(fresult = f_stat(head_filename, NULL))
    {
        if(fresult != FR_NO_FILE) return fresult;

        *file_number = 1;

        do
        {
            if(++*file_number >= 1000)
            {
                *file_number = 1;
                ltoa(*file_number, filename_buffer, 10);
                strcat(filename_buffer, scan_filename);
                break;
            }
            ltoa(*file_number, filename_buffer, 10);
            strcat(filename_buffer, scan_filename);
        }
        while(!(fresult = f_stat(filename_buffer, NULL)));

        if(fresult = f_open(&fil, head_filename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
        {
            fresult = f_close(&fil);
            return fresult;
        }
    }
    else
    {
        if(fresult = f_open(&fil, head_filename, FA_READ | FA_WRITE))
        {
            fresult = f_close(&fil);
            return fresult;
        }
        float value;
        f_gets(working_buffer, WBUF_SIZE, &fil);
        sscanf(working_buffer, "%g", &value);
        *file_number = value;
        if(update)
            if(++*file_number >= 1000) *file_number = 1;
    }
    fresult = f_lseek(&fil, 0);
    f_puts(ltoa(*file_number, working_buffer, 10), &fil);
    fresult = f_truncate(&fil);
    fresult = f_close(&fil);

    return fresult;
}

Uint16 SD_card_class::log_data()
{
    static FIL log_file;
//    static Uint32 benchmark_log_mem;
//    benchmark_log_mem = ReadIpcTimer();

    static Uint16 change_file_number = 1;
    static Uint16 log_counter = 0;
    if(++log_counter > 6 * 60 * 24) // jeden dzien zapisu co 10s
    {
        log_counter = 0;
        change_file_number = 1;
    }

    if(change_file_number)
    {
        log_counter =
        change_file_number = 0;

        fresult = f_close(&log_file);

        f_mount(0, "", 1);
        f_mount(&fs, "", 1);

        find_last_enumeration(1, &file_number_logs, "log_cnt.txt", "logs.bin");

        ltoa(file_number_logs+1, filename_buffer, 10);
        strcat(filename_buffer,"logs.bin");
        f_unlink(filename_buffer);

        ltoa(file_number_logs, filename_buffer, 10);
        strcat(filename_buffer,"logs.bin");
        if(fresult = f_open(&log_file, filename_buffer, FA_WRITE | FA_CREATE_ALWAYS)) return fresult;
    }

    float temp_array[20];
    temp_array[0] = *(float *)&FatFS_time;
    temp_array[1] = CLA2toCLA1.Grid_filter.U_grid_1h.a;
    temp_array[2] = CLA2toCLA1.Grid_filter.U_grid_1h.b;
    temp_array[3] = CLA2toCLA1.Grid_filter.U_grid_1h.c;
    temp_array[4] = CLA2toCLA1.Grid_filter.Q_grid_1h.a;
    temp_array[5] = CLA2toCLA1.Grid_filter.Q_grid_1h.b;
    temp_array[6] = CLA2toCLA1.Grid_filter.Q_grid_1h.c;
    temp_array[7] = CLA2toCLA1.Grid_filter.P_grid_1h.a;
    temp_array[8] = CLA2toCLA1.Grid_filter.P_grid_1h.b;
    temp_array[9] = CLA2toCLA1.Grid_filter.P_grid_1h.c;

//    if(!status_master.slave_rdy_0)
//    {
//        log_slave[0].P_conv_1h.a =
//        log_slave[0].P_conv_1h.b =
//        log_slave[0].P_conv_1h.c =
//        log_slave[0].Q_conv_1h.a =
//        log_slave[0].Q_conv_1h.b =
//        log_slave[0].Q_conv_1h.c =
//        log_slave[0].Temperature.a =
//        log_slave[0].Temperature.b =
//        log_slave[0].Temperature.c =
//        log_slave[0].Temperature.n = 0.0f;
//    }

    temp_array[10] = CLA2toCLA1.Grid_filter.Q_conv_1h.a;//log_slave[0].P_conv_1h.a;
    temp_array[11] = CLA2toCLA1.Grid_filter.Q_conv_1h.b;//log_slave[0].P_conv_1h.b;
    temp_array[12] = CLA2toCLA1.Grid_filter.Q_conv_1h.c;//log_slave[0].P_conv_1h.c;
    temp_array[13] = CLA2toCLA1.Grid_filter.P_conv_1h.a;//log_slave[0].Q_conv_1h.a;
    temp_array[14] = CLA2toCLA1.Grid_filter.P_conv_1h.b;//log_slave[0].Q_conv_1h.b;
    temp_array[15] = CLA2toCLA1.Grid_filter.P_conv_1h.c;//log_slave[0].Q_conv_1h.c;
    temp_array[16] = log_slave[0].Temperature.a;
    temp_array[17] = log_slave[0].Temperature.b;
    temp_array[18] = log_slave[0].Temperature.c;
    temp_array[19] = log_slave[0].Temperature.n;

//    if(!status_master.slave_rdy_1)
//    {
//        log_slave[1].P_conv_1h.a =
//        log_slave[1].P_conv_1h.b =
//        log_slave[1].P_conv_1h.c =
//        log_slave[1].Q_conv_1h.a =
//        log_slave[1].Q_conv_1h.b =
//        log_slave[1].Q_conv_1h.c =
//        log_slave[1].Temperature.a =
//        log_slave[1].Temperature.b =
//        log_slave[1].Temperature.c =
//        log_slave[1].Temperature.n = 0.0f;
//    }
//
//    temp_array[20] = log_slave[1].P_conv_1h.a;
//    temp_array[21] = log_slave[1].P_conv_1h.b;
//    temp_array[22] = log_slave[1].P_conv_1h.c;
//    temp_array[23] = log_slave[1].Q_conv_1h.a;
//    temp_array[24] = log_slave[1].Q_conv_1h.b;
//    temp_array[25] = log_slave[1].Q_conv_1h.c;
//    temp_array[26] = log_slave[1].Temperature.a;
//    temp_array[27] = log_slave[1].Temperature.b;
//    temp_array[28] = log_slave[1].Temperature.c;
//    temp_array[29] = log_slave[1].Temperature.n;
//
//    if(!status_master.slave_rdy_2)
//    {
//        log_slave[2].P_conv_1h.a =
//        log_slave[2].P_conv_1h.b =
//        log_slave[2].P_conv_1h.c =
//        log_slave[2].Q_conv_1h.a =
//        log_slave[2].Q_conv_1h.b =
//        log_slave[2].Q_conv_1h.c =
//        log_slave[2].Temperature.a =
//        log_slave[2].Temperature.b =
//        log_slave[2].Temperature.c =
//        log_slave[2].Temperature.n = 0.0f;
//    }
//
//    temp_array[30] = log_slave[2].P_conv_1h.a;
//    temp_array[31] = log_slave[2].P_conv_1h.b;
//    temp_array[32] = log_slave[2].P_conv_1h.c;
//    temp_array[33] = log_slave[2].Q_conv_1h.a;
//    temp_array[34] = log_slave[2].Q_conv_1h.b;
//    temp_array[35] = log_slave[2].Q_conv_1h.c;
//    temp_array[36] = log_slave[2].Temperature.a;
//    temp_array[37] = log_slave[2].Temperature.b;
//    temp_array[38] = log_slave[2].Temperature.c;
//    temp_array[39] = log_slave[2].Temperature.n;
//
//    if(!status_master.slave_rdy_3)
//    {
//        log_slave[3].P_conv_1h.a =
//        log_slave[3].P_conv_1h.b =
//        log_slave[3].P_conv_1h.c =
//        log_slave[3].Q_conv_1h.a =
//        log_slave[3].Q_conv_1h.b =
//        log_slave[3].Q_conv_1h.c =
//        log_slave[3].Temperature.a =
//        log_slave[3].Temperature.b =
//        log_slave[3].Temperature.c =
//        log_slave[3].Temperature.n = 0.0f;
//    }
//
//    temp_array[40] = log_slave[3].P_conv_1h.a;
//    temp_array[41] = log_slave[3].P_conv_1h.b;
//    temp_array[42] = log_slave[3].P_conv_1h.c;
//    temp_array[43] = log_slave[3].Q_conv_1h.a;
//    temp_array[44] = log_slave[3].Q_conv_1h.b;
//    temp_array[45] = log_slave[3].Q_conv_1h.c;
//    temp_array[46] = log_slave[3].Temperature.a;
//    temp_array[47] = log_slave[3].Temperature.b;
//    temp_array[48] = log_slave[3].Temperature.c;
//    temp_array[49] = log_slave[3].Temperature.n;

    save_memory(&log_file, (Uint16 *)temp_array, sizeof(temp_array));
    fresult = f_sync(&log_file);
//    benchmark_log = (float)((Uint32)ReadIpcTimer() - benchmark_log_mem)*5e-9;
    return fresult;
}

void SD_card_class::save_single_state_master(FIL *fil, union ALARM_master alarm_master_temp)
{
    if(alarm_master_temp.bit.rx1_crc_error    ) f_puts("\t\trx1_crc_error \n", fil);
    if(alarm_master_temp.bit.rx1_overrun_error) f_puts("\t\trx1_overrun_error \n", fil);
    if(alarm_master_temp.bit.rx1_frame_error  ) f_puts("\t\trx1_frame_error \n", fil);
    if(alarm_master_temp.bit.rx2_crc_error    ) f_puts("\t\trx2_crc_error \n", fil);
    if(alarm_master_temp.bit.rx2_overrun_error) f_puts("\t\trx2_overrun_error \n", fil);
    if(alarm_master_temp.bit.rx2_frame_error  ) f_puts("\t\trx2_frame_error \n", fil);
    if(alarm_master_temp.bit.rx1_port_nrdy    ) f_puts("\t\trx1_port_nrdy \n", fil);
    if(alarm_master_temp.bit.rx2_port_nrdy    ) f_puts("\t\trx2_port_nrdy \n", fil);
    if(alarm_master_temp.bit.sed_err          ) f_puts("\t\tsed_err \n", fil);

    if(alarm_master_temp.bit.Not_enough_data_master) f_puts("\t\tNot_enough_data_master \n", fil);
    if(alarm_master_temp.bit.CT_char_error         ) f_puts("\t\tCT_char_error \n", fil);
    if(alarm_master_temp.bit.PLL_UNSYNC            ) f_puts("\t\tPLL_UNSYNC \n", fil);
    if(alarm_master_temp.bit.FLT_SUPPLY_MASTER     ) f_puts("\t\tFLT_SUPPLY_MASTER \n", fil);

    if(alarm_master_temp.bit.U_grid_rms_a_L) f_puts("\t\tU_grid_rms_a_L \n", fil);
    if(alarm_master_temp.bit.U_grid_rms_b_L) f_puts("\t\tU_grid_rms_b_L \n", fil);
    if(alarm_master_temp.bit.U_grid_rms_c_L) f_puts("\t\tU_grid_rms_c_L \n", fil);

    if(alarm_master_temp.bit.U_grid_abs_a_H) f_puts("\t\tU_grid_abs_a_H \n", fil);
    if(alarm_master_temp.bit.U_grid_abs_b_H) f_puts("\t\tU_grid_abs_b_H \n", fil);
    if(alarm_master_temp.bit.U_grid_abs_c_H) f_puts("\t\tU_grid_abs_c_H \n", fil);

    if(alarm_master_temp.bit.no_sync) f_puts("\t\tno_sync \n", fil);

    snprintf(working_buffer, WBUF_SIZE, "\t\t{%08lX}\n", alarm_master_temp.all);
    f_puts(working_buffer, fil);
}

void SD_card_class::save_single_state_slave(FIL *fil, union ALARM_slave alarm_slave_temp)
{
    if(alarm_slave_temp.bit.I_conv_a_H) f_puts("\t\tI_conv_a_H \n", fil);
    if(alarm_slave_temp.bit.I_conv_a_L) f_puts("\t\tI_conv_a_L \n", fil);
    if(alarm_slave_temp.bit.I_conv_b_H) f_puts("\t\tI_conv_b_H \n", fil);
    if(alarm_slave_temp.bit.I_conv_b_L) f_puts("\t\tI_conv_b_L \n", fil);
    if(alarm_slave_temp.bit.I_conv_c_H) f_puts("\t\tI_conv_c_H \n", fil);
    if(alarm_slave_temp.bit.I_conv_c_L) f_puts("\t\tI_conv_c_L \n", fil);
    if(alarm_slave_temp.bit.I_conv_n_H) f_puts("\t\tI_conv_n_H \n", fil);
    if(alarm_slave_temp.bit.I_conv_n_L) f_puts("\t\tI_conv_n_L \n", fil);
    //
    if(alarm_slave_temp.bit.Driver_FLT_a_A) f_puts("\tDriver_FLT_a_A \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_a_B) f_puts("\tDriver_FLT_a_B \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_b_A) f_puts("\tDriver_FLT_b_A \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_b_B) f_puts("\tDriver_FLT_b_B \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_c_A) f_puts("\tDriver_FLT_c_A \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_c_B) f_puts("\tDriver_FLT_c_B \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_n_A) f_puts("\tDriver_FLT_n_A \n", fil);
    if(alarm_slave_temp.bit.Driver_FLT_n_B) f_puts("\tDriver_FLT_n_B \n", fil);
    //
    if(alarm_slave_temp.bit.Driver_nRDY_a_A) f_puts("\tDriver_nRDY_a_A \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_a_B) f_puts("\tDriver_nRDY_a_B \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_b_A) f_puts("\tDriver_nRDY_b_A \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_b_B) f_puts("\tDriver_nRDY_b_B \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_c_A) f_puts("\tDriver_nRDY_c_A \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_c_B) f_puts("\tDriver_nRDY_c_B \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_n_A) f_puts("\tDriver_nRDY_n_A \n", fil);
    if(alarm_slave_temp.bit.Driver_nRDY_n_B) f_puts("\tDriver_nRDY_n_B \n", fil);
    //
    if(alarm_slave_temp.bit.I_conv_rms_a) f_puts("\t\tI_conv_rms_a  \n", fil);
    if(alarm_slave_temp.bit.I_conv_rms_b) f_puts("\t\tI_conv_rms_b  \n", fil);
    if(alarm_slave_temp.bit.I_conv_rms_c) f_puts("\t\tI_conv_rms_c  \n", fil);
    if(alarm_slave_temp.bit.I_conv_rms_n) f_puts("\t\tI_conv_rms_n  \n", fil);
    //
    if(alarm_slave_temp.bit.Temperature_H) f_puts("\t\tTemperature_H \n", fil);
    if(alarm_slave_temp.bit.Temperature_L) f_puts("\t\tTemperature_L \n", fil);
    if(alarm_slave_temp.bit.U_dc_H       ) f_puts("\t\tU_dc_H        \n", fil);
    if(alarm_slave_temp.bit.U_dc_L       ) f_puts("\t\tU_dc_L        \n", fil);

    if(alarm_slave_temp.bit.Not_enough_data_slave) f_puts("\t\tNot_enough_data_slave \n", fil);
    if(alarm_slave_temp.bit.CONV_SOFTSTART       ) f_puts("\t\tCONV_SOFTSTART \n", fil);
    if(alarm_slave_temp.bit.FUSE_BROKEN          ) f_puts("\t\tFUSE_BROKEN \n", fil);
    if(alarm_slave_temp.bit.FLT_SUPPLY_SLAVE     ) f_puts("\t\tFLT_SUPPLY_SLAVE \n", fil);
    //
    if(alarm_slave_temp.bit.TZ_FPGA_FLT ) f_puts("\t\tTZ_FPGA_FLT \n", fil);
    if(alarm_slave_temp.bit.TZ_CLOCKFAIL) f_puts("\t\tTZ_CLOCKFAIL \n", fil);
    if(alarm_slave_temp.bit.TZ_EMUSTOP  ) f_puts("\t\tTZ_EMUSTOP \n", fil);
    if(alarm_slave_temp.bit.TZ          ) f_puts("\t\tTZ \n", fil);
    //
    if(alarm_slave_temp.bit.sync_error ) f_puts("\t\tsync_error \n", fil);

    if(alarm_slave_temp.bit.rx1_crc_error    ) f_puts("\t\trx1_crc_error \n", fil);
    if(alarm_slave_temp.bit.rx1_overrun_error) f_puts("\t\trx1_overrun_error \n", fil);
    if(alarm_slave_temp.bit.rx1_frame_error  ) f_puts("\t\trx1_frame_error \n", fil);
    if(alarm_slave_temp.bit.rx2_crc_error    ) f_puts("\t\trx2_crc_error \n", fil);
    if(alarm_slave_temp.bit.rx2_overrun_error) f_puts("\t\trx2_overrun_error \n", fil);
    if(alarm_slave_temp.bit.rx2_frame_error  ) f_puts("\t\trx2_frame_error \n", fil);
    if(alarm_slave_temp.bit.rx1_port_nrdy    ) f_puts("\t\trx1_port_nrdy \n", fil);
    if(alarm_slave_temp.bit.rx2_port_nrdy    ) f_puts("\t\trx2_port_nrdy \n", fil);
    if(alarm_slave_temp.bit.sed_err          ) f_puts("\t\tsed_err \n", fil);

    if(alarm_slave_temp.bit.U_dc_balance          ) f_puts("\t\tU_dc_balance \n", fil);

    if(alarm_slave_temp.bit.lopri_timeout          ) f_puts("\t\tlopri_timeout \n", fil);
    if(alarm_slave_temp.bit.lopri_error            ) f_puts("\t\tlopri_error \n", fil);
    if(alarm_slave_temp.bit.msg2_error             ) f_puts("\t\tmsg2_error \n", fil);
    if(alarm_slave_temp.bit.msg0_error             ) f_puts("\t\tmsg0_error \n", fil);

    snprintf(working_buffer, WBUF_SIZE, "\t\t{%08lX, %08lX}\n", alarm_slave_temp.all[1], alarm_slave_temp.all[0]);
    f_puts(working_buffer, fil);
}

void SD_card_class::save_text_message(FIL *fil)
{
    f_puts("Error time:\n", fil);

    working_buffer[0] = '0' + RTC_current_time.day10;
    working_buffer[1] = '0' + RTC_current_time.day;
    working_buffer[2] = '.';
    working_buffer[3] = '0' + RTC_current_time.month10;
    working_buffer[4] = '0' + RTC_current_time.month;
    working_buffer[5] = '.';
    working_buffer[6] = '2';
    working_buffer[7] = '0';
    working_buffer[8] = '0' + RTC_current_time.year10;
    working_buffer[9] = '0' + RTC_current_time.year;
    working_buffer[10] = ' ';
    working_buffer[11] = '0' + RTC_current_time.hour10;
    working_buffer[12] = '0' + RTC_current_time.hour;
    working_buffer[13] = ':';
    working_buffer[14] = '0' + RTC_current_time.minute10;
    working_buffer[15] = '0' + RTC_current_time.minute;
    working_buffer[16] = ':';
    working_buffer[17] = '0' + RTC_current_time.second10;
    working_buffer[18] = '0' + RTC_current_time.second;
    working_buffer[19] = '\n';
    working_buffer[20] = 0;
    f_puts(working_buffer, fil);

    f_puts("\nMaster errors:\n", fil);
    f_puts("\tList of snapshot errors:\n", fil);
    save_single_state_master(fil, alarm_master_snapshot);
    f_puts("\tList of all errors:\n", fil);
    save_single_state_master(fil, alarm_master);

    f_puts("\nSlave 0 errors:\n", fil);
    f_puts("\tList of snapshot errors:\n", fil);
    save_single_state_slave(fil, alarm_slave_snapshot[0]);
    f_puts("\tList of all errors:\n", fil);
    save_single_state_slave(fil, alarm_slave[0]);

//    f_puts("\nSlave 1 errors:\n", fil);
//    f_puts("\tList of snapshot errors:\n", fil);
//    save_single_state_slave(fil, alarm_slave_snapshot[1]);
//    f_puts("\tList of all errors:\n", fil);
//    save_single_state_slave(fil, alarm_slave[1]);
//
//    f_puts("\nSlave 2 errors:\n", fil);
//    f_puts("\tList of snapshot errors:\n", fil);
//    save_single_state_slave(fil, alarm_slave_snapshot[2]);
//    f_puts("\tList of all errors:\n", fil);
//    save_single_state_slave(fil, alarm_slave[2]);
//
//    f_puts("\nSlave 3 errors:\n", fil);
//    f_puts("\tList of snapshot errors:\n", fil);
//    save_single_state_slave(fil, alarm_slave_snapshot[3]);
//    f_puts("\tList of all errors:\n", fil);
//    save_single_state_slave(fil, alarm_slave[3]);
}

void SD_card_class::save_state_task()
{
    static FIL fil;
    static Uint16 current_node;
    static Uint32 delay_timer;

    switch(save_error_state)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            scope_global.scope_trigger = 1;
            EMIF_mem.write.Scope_trigger = 1;
            Fiber_comm[0].input_flags.read_async_data = 1;
//            Fiber_comm[1].input_flags.read_async_data = 1;
//            Fiber_comm[2].input_flags.read_async_data = 1;
//            Fiber_comm[3].input_flags.read_async_data = 1;

            delay_timer = IpcRegs.IPCCOUNTERL;
            save_error_state++;
            break;
        }
        case 2:
        {
            if (Fiber_comm[0].input_flags.read_async_data == 0 || IpcRegs.IPCCOUNTERL - delay_timer > 1000000000)
            {
                Fiber_comm[0].input_flags.read_async_data = 0;
                save_error_state++;
            }
//            if ((Fiber_comm[0].input_flags.read_async_data == 0 &&
//                Fiber_comm[1].input_flags.read_async_data == 0 &&
//                Fiber_comm[2].input_flags.read_async_data == 0 &&
//                Fiber_comm[3].input_flags.read_async_data == 0) ||
//                IpcRegs.IPCCOUNTERL - delay_timer > 1000000000)
//            {
//                Fiber_comm[0].input_flags.read_async_data = 0;
//                Fiber_comm[1].input_flags.read_async_data = 0;
//                Fiber_comm[2].input_flags.read_async_data = 0;
//                Fiber_comm[3].input_flags.read_async_data = 0;
//                save_error_state++;
//            }
            break;
        }
        case 3:
        {
            find_last_enumeration(1, &file_number_errors, "err_cnt.txt", "error.txt");

            ltoa(file_number_errors+1, filename_buffer, 10);
            strcat(filename_buffer,"error.txt");
            f_unlink(filename_buffer);
            ltoa(file_number_errors+1, filename_buffer, 10);
            strcat(filename_buffer,"scope.bin");
            f_unlink(filename_buffer);
            ltoa(file_number_errors+1, filename_buffer, 10);
            strcat(filename_buffer,"scope.bit");
            f_unlink(filename_buffer);

            ltoa(file_number_errors, filename_buffer, 10);
            strcat(filename_buffer,"Error.txt");
            f_close(&fil);
            if(fresult = f_open(&fil, filename_buffer, FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
            {
                f_close(&fil);
                save_error_state = 7;
            }
            else
            {
                save_text_message(&fil);
                fresult = f_close(&fil);
                save_error_state++;
            }

            break;
        }
        case 4:
        {
            if(Scope.finished_sorting)
            {
                ltoa(file_number_errors, filename_buffer, 10);
                strcat(filename_buffer,"Scope.bin");
                if((fresult = f_open(&fil, filename_buffer, FA_READ | FA_WRITE | FA_CREATE_ALWAYS)) || alarm_master.bit.FLT_SUPPLY_MASTER)
                {
                    f_close(&fil);
                    save_error_state = 7;
                }
                else
                {
                    for(Uint16 i = 0; i<6; i++)
                    {
                        save_memory(&fil, (Uint16 *)&Scope.data[i], sizeof(Scope.data[0]));
                        Fiber_comm[0].Main();
                        Fiber_comm[1].Main();
                        Fiber_comm[2].Main();
                        Fiber_comm[3].Main();
                    }
                    f_close(&fil);

                    memset(&Scope.data, 0, sizeof(Scope.data));
                    current_node = 0;
                    Fiber_comm[current_node].input_flags.read_scope = 1;

                    save_error_state++;
                }
            }
            break;
        }
        case 5:
        {
            if(!Fiber_comm[current_node].input_flags.read_scope)
            {
                ltoa(file_number_errors, filename_buffer, 10);
                strcat(filename_buffer,"Scope.bin");
                if((fresult = f_open(&fil, filename_buffer, FA_READ | FA_WRITE | FA_OPEN_APPEND)) || alarm_master.bit.FLT_SUPPLY_MASTER)
                {
                    f_close(&fil);
                    save_error_state = 7;
                }
                else
                {
                    for(Uint16 i = 0; i<6; i++)
                    {
                        save_memory(&fil, (Uint16 *)&Scope.data[i], sizeof(Scope.data[0]));
                        Fiber_comm[0].Main();
                        Fiber_comm[1].Main();
                        Fiber_comm[2].Main();
                        Fiber_comm[3].Main();
                    }
                    if(++current_node >= 1)//ograniczone do jednego scope
                    {
                        save_error_state++;
                        save_text_message(&fil);
                    }
                    else
                    {
                        memset(&Scope.data, 0, sizeof(Scope.data));
                        Fiber_comm[current_node].input_flags.read_scope = 1;
                    }
                    f_close(&fil);
                }
            }
            break;
        }
        case 6:
        {
            if(EMIF_mem.read.Scope_rdy)
            {
                ltoa(file_number_errors, filename_buffer, 10);
                strcat(filename_buffer,"Scope.bit");
                f_close(&fil);
                if(fresult = f_open(&fil, filename_buffer, FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
                {
                    f_close(&fil);
                    save_error_state = 7;
                }
                else
                {
                    Uint32 Scope_depth = EMIF_mem.read.Scope_depth;
                    Uint32 index = 0;
                    while(index < Scope_depth)
                    {
                        index = save_FPGA_scope(&fil, index);
                        Fiber_comm[0].Main();
                        Fiber_comm[1].Main();
                        Fiber_comm[2].Main();
                        Fiber_comm[3].Main();
                    }
                    fresult = f_close(&fil);
                    save_error_state++;
                }
            }
            break;
        }
        default:
        {
            scope_global.scope_trigger = 0;
            EMIF_mem.write.Scope_trigger = 0;
            save_error_state = 0;
            break;
        }
    }
}

Uint16 SD_card_class::read_settings()
{
    memset(&SD_card.settings, 0, sizeof(SD_card.settings));

    FIL fil;

    if(fresult = f_open(&fil, "settings.csv", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    while(f_gets(working_buffer, WBUF_SIZE, &fil))
    {
        char * pch = working_buffer;
        while(*pch) *pch++ = toupper(*pch);

        pch = strchr(working_buffer,';') + 1;
        float value;
        sscanf(pch,"%g", &value);

        if(!strncmp(working_buffer, "STATIC Q COMPENSATION A", sizeof("STATIC Q COMPENSATION A")-1))
            settings.control.Q_set.a = value;

        if(!strncmp(working_buffer, "STATIC Q COMPENSATION B", sizeof("STATIC Q COMPENSATION B")-1))
            settings.control.Q_set.b = value;

        if(!strncmp(working_buffer, "STATIC Q COMPENSATION C", sizeof("STATIC Q COMPENSATION C")-1))
            settings.control.Q_set.c = value;

        if(!strncmp(working_buffer, "ENABLE Q COMPENSATION A", sizeof("ENABLE Q COMPENSATION A")-1))
            settings.control.flags.bit.enable_Q_comp_a = value;

        if(!strncmp(working_buffer, "ENABLE Q COMPENSATION B", sizeof("ENABLE Q COMPENSATION B")-1))
            settings.control.flags.bit.enable_Q_comp_b = value;

        if(!strncmp(working_buffer, "ENABLE Q COMPENSATION C", sizeof("ENABLE Q COMPENSATION C")-1))
            settings.control.flags.bit.enable_Q_comp_c = value;

        if(!strncmp(working_buffer, "ENABLE P SYMMETRIZATION", sizeof("ENABLE P SYMMETRIZATION")-1))
            settings.control.flags.bit.enable_P_sym = value;

        if(!strncmp(working_buffer, "ENABLE H COMPENSATION", sizeof("ENABLE H COMPENSATION")-1))
            settings.control.flags.bit.enable_H_comp = value;

        if(!strncmp(working_buffer, "VERSION P SYMMETRIZATION", sizeof("VERSION P SYMMETRIZATION")-1))
            settings.control.flags.bit.version_P_sym = value;

        if(!strncmp(working_buffer, "VERSION Q COMPENSATION A", sizeof("VERSION Q COMPENSATION A")-1))
            settings.control.flags.bit.version_Q_comp_a = value;

        if(!strncmp(working_buffer, "VERSION Q COMPENSATION B", sizeof("VERSION Q COMPENSATION B")-1))
            settings.control.flags.bit.version_Q_comp_b = value;

        if(!strncmp(working_buffer, "VERSION Q COMPENSATION C", sizeof("VERSION Q COMPENSATION C")-1))
            settings.control.flags.bit.version_Q_comp_c = value;

        if(!strncmp(working_buffer, "TANGENS RANGE A HIGH", sizeof("TANGENS RANGE A HIGH")-1))
            settings.control.tangens_range[0].a = value;

        if(!strncmp(working_buffer, "TANGENS RANGE B HIGH", sizeof("TANGENS RANGE B HIGH")-1))
            settings.control.tangens_range[0].b = value;

        if(!strncmp(working_buffer, "TANGENS RANGE C HIGH", sizeof("TANGENS RANGE C HIGH")-1))
            settings.control.tangens_range[0].c = value;

        if(!strncmp(working_buffer, "TANGENS RANGE A LOW", sizeof("TANGENS RANGE A LOW")-1))
            settings.control.tangens_range[1].a = value;

        if(!strncmp(working_buffer, "TANGENS RANGE B LOW", sizeof("TANGENS RANGE B LOW")-1))
            settings.control.tangens_range[1].b = value;

        if(!strncmp(working_buffer, "TANGENS RANGE C LOW", sizeof("TANGENS RANGE C LOW")-1))
            settings.control.tangens_range[1].c = value;

        if(!strncmp(working_buffer, "BAUDRATE", sizeof("BAUDRATE")-1))
            settings.Baudrate = value;

        if(!strncmp(working_buffer, "EXT_SERVER_ID", sizeof("EXT_SERVER_ID")-1))
            settings.modbus_ext_server_id = value;

        if(!strncmp(working_buffer, "WIFI_ONOFF", sizeof("WIFI_ONOFF")-1))
            settings.wifi_on = value==0? 0 : 1;

        if(!strncmp(working_buffer, "NUMBER OF SLAVES", sizeof("NUMBER OF SLAVES")-1))
            settings.number_of_slaves = value;
    }
    if(fresult = f_close(&fil)) return fresult;

    if(settings.modbus_ext_server_id <1 || settings.modbus_ext_server_id>230 )
        settings.modbus_ext_server_id = 1;
    if(settings.number_of_slaves < 1.0f || settings.number_of_slaves > 4.0f) return fresult;
    if(settings.control.tangens_range[0].a < -1.0f || settings.control.tangens_range[0].a > 1.0f) return fresult;
    if(settings.control.tangens_range[0].b < -1.0f || settings.control.tangens_range[0].b > 1.0f) return fresult;
    if(settings.control.tangens_range[0].c < -1.0f || settings.control.tangens_range[0].c > 1.0f) return fresult;
    if(settings.control.tangens_range[1].a < -1.0f || settings.control.tangens_range[1].a > 1.0f) return fresult;
    if(settings.control.tangens_range[1].b < -1.0f || settings.control.tangens_range[1].b > 1.0f) return fresult;
    if(settings.control.tangens_range[1].c < -1.0f || settings.control.tangens_range[1].c > 1.0f) return fresult;


    settings.available = 1;

    return fresult;
}

Uint16 SD_card_class::save_settings()
{
    FIL fil;
    if(fresult = f_open(&fil, "settings.csv", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
    {
        f_close(&fil);
        return fresult;
    }

    f_puts("STATIC Q COMPENSATION A;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.Q_set.a);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("STATIC Q COMPENSATION B;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.Q_set.b);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("STATIC Q COMPENSATION C;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.Q_set.c);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    f_puts("ENABLE Q COMPENSATION A;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.enable_Q_comp_a);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("ENABLE Q COMPENSATION B;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.enable_Q_comp_b);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("ENABLE Q COMPENSATION C;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.enable_Q_comp_c);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    f_puts("ENABLE P SYMMETRIZATION;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.enable_P_sym);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("ENABLE H COMPENSATION;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.enable_H_comp);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("VERSION P SYMMETRIZATION;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.version_P_sym);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    f_puts("VERSION Q COMPENSATION A;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.version_Q_comp_a);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("VERSION Q COMPENSATION B;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.version_Q_comp_b);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("VERSION Q COMPENSATION C;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%u", settings.control.flags.bit.version_Q_comp_c);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    f_puts("TANGENS RANGE A HIGH;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.tangens_range[0].a);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("TANGENS RANGE B HIGH;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.tangens_range[0].b);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("TANGENS RANGE C HIGH;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.tangens_range[0].c);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("TANGENS RANGE A LOW;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.tangens_range[1].a);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("TANGENS RANGE B LOW;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.tangens_range[1].b);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("TANGENS RANGE C LOW;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.control.tangens_range[1].c);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    f_puts("BAUDRATE;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.Baudrate);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("EXT_SERVER_ID;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.modbus_ext_server_id);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);
    f_puts("WIFI_ONOFF;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.wifi_on);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    f_puts("NUMBER OF SLAVES;", &fil);
    snprintf(working_buffer, WBUF_SIZE, "%g", settings.number_of_slaves);
    f_puts(working_buffer, &fil);
    f_putc('\n', &fil);

    fresult = f_close(&fil);
    return fresult;
}

Uint16 SD_card_class::read_CT_characteristic()
{
    memset(&SD_card.CT_char, 0, sizeof(SD_card.CT_char));

    FIL fil;
    if(fresult = f_open(&fil, "CT_char.csv", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    Uint16 br;
    Uint16 buff = 0;
    while(buff != '\n')
    {
        if(fresult = f_read(&fil, &buff, 1, &br))
        {
            f_close(&fil);
            return fresult;
        }
        if(!br)
        {
            f_close(&fil);
            return fresult;
        }
    }

    const Uint16 no_columns = 7;
    Uint16 no_elements = read_table(&fil, "%g", no_columns, CT_CHARACTERISTIC_POINTS, CT_char.set_current,
     CT_char.CT_ratio_a, CT_char.CT_ratio_b, CT_char.CT_ratio_c,
     CT_char.phase_a, CT_char.phase_b, CT_char.phase_c);

    CT_char.number_of_elements = no_elements;

    float sort_buffer[CT_CHARACTERISTIC_POINTS][7];
    for(Uint16 i = 0; i < no_elements;i++)
    {
        sort_buffer[i][0] = CT_char.set_current[i];
        sort_buffer[i][1] = CT_char.CT_ratio_a[i];
        sort_buffer[i][2] = CT_char.CT_ratio_b[i];
        sort_buffer[i][3] = CT_char.CT_ratio_c[i];
        sort_buffer[i][4] = CT_char.phase_a[i];
        sort_buffer[i][5] = CT_char.phase_b[i];
        sort_buffer[i][6] = CT_char.phase_c[i];
    }

    qsort(sort_buffer, no_elements, no_columns * sizeof(float), compare_float);

    for(Uint16 i = 0; i < no_elements;i++)
    {
        CT_char.set_current[i] = sort_buffer[i][0];
        CT_char.CT_ratio_a[i] = sort_buffer[i][1];
        CT_char.CT_ratio_b[i] = sort_buffer[i][2];
        CT_char.CT_ratio_c[i] = sort_buffer[i][3];
        CT_char.phase_a[i] = sort_buffer[i][4];
        CT_char.phase_b[i] = sort_buffer[i][5];
        CT_char.phase_c[i] = sort_buffer[i][6];
    }

    if(fresult = f_close(&fil)) return fresult;

    CT_char.available = 1;
    return fresult;
}

Uint16 SD_card_class::read_H_settings()
{
    memset(&SD_card.harmonics, 0, sizeof(SD_card.harmonics));

    FIL fil;
    if(fresult = f_open(&fil, "harmon.csv", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    Uint16 br;
    Uint16 buff = 0;
    while(buff != '\n')
    {
        if(fresult = f_read(&fil, &buff, 1, &br))
        {
            f_close(&fil);
            return fresult;
        }
    }

    struct harmonics_struct
    {
        float harmonic_number[50];
        float on_off_a[50];
        float on_off_b[50];
        float on_off_c[50];
    }harmonic;

    const Uint16 no_columns = 4;
    Uint16 no_elements = read_table(&fil, "%g", no_columns, 50, &harmonic.harmonic_number, &harmonic.on_off_a, &harmonic.on_off_b, &harmonic.on_off_c);

    memset(&harmonics, 0, sizeof(harmonics));

    Uint16 element = 0;
    while(element < no_elements)
    {
        if(harmonic.harmonic_number[element] > 1.0f && harmonic.harmonic_number[element] < 50.0f)
        {
            Uint16 harmonic_uint = (Uint16)harmonic.harmonic_number[element];
            if(harmonic_uint & 0x0001)
            {
                Uint16 index = (harmonic_uint-1)>>1;
                harmonics.on_off_odd_a[index] = harmonic.on_off_a[element];
                harmonics.on_off_odd_b[index] = harmonic.on_off_b[element];
                harmonics.on_off_odd_c[index] = harmonic.on_off_c[element];
            }
            else
            {
                Uint16 index = (harmonic_uint-2)>>1;
                harmonics.on_off_even_a[index] = harmonic.on_off_a[element];
                harmonics.on_off_even_b[index] = harmonic.on_off_b[element];
                harmonics.on_off_even_c[index] = harmonic.on_off_c[element];
            }
        }
        element++;
    }

    if(fresult = f_close(&fil)) return fresult;
    harmonics.available = 1;

    return fresult;
}

Uint16 SD_card_class::save_H_settings()
{
    FIL fil;
    if(fresult = f_open(&fil, "harmon.csv", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
    {
        f_close(&fil);
        return fresult;
    }

    f_puts("harmonic;", &fil);
    f_puts("on_off phase A;", &fil);
    f_puts("on_off phase B;", &fil);
    f_puts("on_off phase C\n", &fil);

    float numbers[25];

    for(Uint16 index = 0;index<25;index++)
    {
        numbers[index] = (float)(index*2 + 1);
    }

    save_table(&fil, "%g", 4, 25, numbers, harmonics.on_off_odd_a, harmonics.on_off_odd_b, harmonics.on_off_odd_c);

    for(Uint16 index = 0;index<25;index++)
    {
        numbers[index] = (float)(index*2 + 2);
    }

    save_table(&fil, "%g", 4, 2, numbers, harmonics.on_off_even_a, harmonics.on_off_even_b, harmonics.on_off_even_c);

    fresult = f_close(&fil);

    return fresult;
}

Uint16 SD_card_class::read_calibration_data()
{
    memset(&SD_card.calibration, 0, sizeof(SD_card.calibration));

    FIL fil;
    if(fresult = f_open(&fil, "calib.csv", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    {
        Uint16 br;
        Uint16 buff = 0;
        while(buff != '\n')
        {
            if(fresult = f_read(&fil, &buff, 1, &br))
            {
                f_close(&fil);
                return fresult;
            }
        }
    }

    const Uint16 no_columns = 1;

    read_table(&fil, "%g", no_columns, sizeof(struct Measurements_master_gain_offset_struct)/sizeof(float), &calibration.Meas_master_gain);

    {
        Uint16 br;
        Uint16 buff = 0;
        while(buff != '\n')
        {
            if(fresult = f_read(&fil, &buff, 1, &br))
            {
                f_close(&fil);
                return fresult;
            }
        }
    }
    read_table(&fil, "%g", no_columns, sizeof(struct Measurements_master_gain_offset_struct)/sizeof(float), &calibration.Meas_master_offset);

    if(fresult = f_close(&fil)) return fresult;

    calibration.available = 1;

    return fresult;
}

Uint16 SD_card_class::save_calibration_data()
{
    FIL fil;
    if(fresult = f_open(&fil, "calib.csv", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
    {
        f_close(&fil);
        return fresult;
    }

    f_puts("struct Measurements Meas_gain\n", &fil);

    const Uint16 no_columns = 1;
    save_table(&fil, "%g", no_columns, sizeof(struct Measurements_master_gain_offset_struct)/sizeof(float), &calibration.Meas_master_gain);
    f_puts("struct Measurements Meas_offset\n", &fil);
    save_table(&fil, "%g", no_columns, sizeof(struct Measurements_master_gain_offset_struct)/sizeof(float), &calibration.Meas_master_offset);

    fresult = f_close(&fil);
    return fresult;
}

Uint16 SD_card_class::read_meter_data()
{
    memset(&SD_card.meter, 0, sizeof(SD_card.meter));

    FIL fil;
    if(fresult = f_open(&fil, "meter.bin", FA_READ))
    {
        f_close(&fil);
        return fresult;
    }

    if(read_memory(&fil, (Uint16 *)&meter.Energy_meter, sizeof(meter.Energy_meter)))
    {
        fresult = f_close(&fil);
        return fresult;
    }

    if(fresult = f_close(&fil)) return fresult;

    meter.available = 1;

    return fresult;
}

Uint16 SD_card_class::save_meter_data()
{
    FIL fil;
    if(fresult = f_open(&fil, "meter.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS))
    {
        f_close(&fil);
        return fresult;
    }

    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.P_p,  (Uint16 *)&Energy_meter.upper.P_p,  sizeof(meter.Energy_meter.P_p));
    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.P_n,  (Uint16 *)&Energy_meter.upper.P_n,  sizeof(meter.Energy_meter.P_n));
    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.QI,   (Uint16 *)&Energy_meter.upper.QI,   sizeof(meter.Energy_meter.QI));
    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.QII,  (Uint16 *)&Energy_meter.upper.QII,  sizeof(meter.Energy_meter.QII));
    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.QIII, (Uint16 *)&Energy_meter.upper.QIII, sizeof(meter.Energy_meter.QIII));
    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.QIV,  (Uint16 *)&Energy_meter.upper.QIV,  sizeof(meter.Energy_meter.QIV));
    DINT_copy_CPUasm((Uint16 *)&meter.Energy_meter.sum,  (Uint16 *)&Energy_meter.upper.sum,  sizeof(meter.Energy_meter.sum));

    save_memory(&fil, (Uint16 *)&meter.Energy_meter, sizeof(meter.Energy_meter));

    fresult = f_close(&fil);

    return fresult;
}

Uint32 SD_card_class::save_FPGA_scope(FIL* fp, Uint32 index)
{
    Uint32 Scope_depth = EMIF_mem.read.Scope_depth;
    Uint32 Scope_width_mult = EMIF_mem.read.Scope_width_mult;
    Uint32 Scope_index_last = (EMIF_mem.read.Scope_index_last+1) * Scope_width_mult;

    Uint16 cycles = 0;
    for(;index < Scope_depth && cycles < 256; index++, cycles++)
    {
        Uint32 modulo = index%Scope_width_mult;
        Uint32 addr = index + Scope_index_last + (Scope_width_mult-1) - (modulo<<1);
        EMIF_mem.write.Scope_address = addr;
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");
        asm(" NOP");

        register Uint32 low, high;
        low = EMIF_mem.read.Scope_data_out1;
        high = EMIF_mem.read.Scope_data_out2;
        Uint64 bit_36_val = ((Uint64)high << 32) | (Uint64)low;
        snprintf(working_buffer, WBUF_SIZE, "%09llX", bit_36_val);
        f_puts(working_buffer, fp);
        if(modulo) f_putc('\n', fp);
    }
    return index;
}
