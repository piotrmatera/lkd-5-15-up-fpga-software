/*
 * CPU_shared.h
 *
 *  Created on: 16 sie 2020
 *      Author: MrTea
 */

#ifndef NODE_SHARED_H_
#define NODE_SHARED_H_

#include "stdafx.h"

struct harmonic_odd_struct
{
    Uint32 rsrvd1:1;
    Uint32 harm3:1;
    Uint32 harm5:1;
    Uint32 harm7:1;
    Uint32 harm9:1;
    Uint32 harm11:1;
    Uint32 harm13:1;
    Uint32 harm15:1;
    Uint32 harm17:1;
    Uint32 harm19:1;
    Uint32 harm21:1;
    Uint32 harm23:1;
    Uint32 harm25:1;
    Uint32 harm27:1;
    Uint32 harm29:1;
    Uint32 harm31:1;
    Uint32 harm33:1;
    Uint32 harm35:1;
    Uint32 harm37:1;
    Uint32 harm39:1;
    Uint32 harm41:1;
    Uint32 harm43:1;
    Uint32 harm45:1;
    Uint32 harm47:1;
    Uint32 harm49:1;
    Uint32 rsvd2:7;
};

struct harmonic_even_struct
{
    Uint32 harm2:1;
    Uint32 harm4:1;
    Uint32 rsvd1:30;
};

union ALARM_master
{
    Uint32 all;
    struct
    {
        Uint16 rx1_crc_error:1;
        Uint16 rx1_overrun_error:1;
        Uint16 rx1_frame_error:1;
        Uint16 rx2_crc_error:1;
        Uint16 rx2_overrun_error:1;
        Uint16 rx2_frame_error:1;
        Uint16 rx1_port_nrdy:1;
        Uint16 rx2_port_nrdy:1;

        Uint16 Not_enough_data_master : 1;
        Uint16 CT_char_error : 1;
        Uint16 PLL_UNSYNC : 1;
        Uint16 FLT_SUPPLY_MASTER:1;

        Uint16 sed_err:1;
        Uint16 no_sync:1;
        Uint16 U_grid_rms_a_L:1;
        Uint16 U_grid_rms_b_L:1;
        Uint16 U_grid_rms_c_L:1;

        Uint16 U_grid_abs_a_H:1;
        Uint16 U_grid_abs_b_H:1;
        Uint16 U_grid_abs_c_H:1;

        Uint16 rsvd1:12;
    }bit;
};

struct STATUS_master
{
    Uint16 Init_done:1;
    Uint16 ONOFF:1;
    Uint16 DS1_switch_SD_CT:1;
    Uint16 DS2_enable_Q_comp:1;
    Uint16 DS3_enable_P_sym:1;
    Uint16 DS4_enable_H_comp:1;
    Uint16 DS5_limit_to_9odd_harmonics:1;
    Uint16 DS6_limit_to_14odd_harmonics:1;
    Uint16 DS7_limit_to_19odd_harmonics:1;
    Uint16 DS8_DS_override:1;
    Uint16 calibration_procedure_error:1;
    Uint16 L_grid_measured:1;
    Uint16 Scope_snapshot_pending:1;
    Uint16 Scope_snapshot_error:1;
    Uint16 SD_card_not_enough_data:1;
    Uint16 SD_no_CT_characteristic : 1;

    Uint16 SD_no_calibration : 1;
    Uint16 SD_no_harmonic_settings : 1;
    Uint16 SD_no_settings : 1;
    Uint16 FLASH_not_enough_data : 1;
    Uint16 FLASH_no_CT_characteristic : 1;
    Uint16 FLASH_no_calibration : 1;
    Uint16 FLASH_no_harmonic_settings : 1;
    Uint16 FLASH_no_settings : 1;
    Uint16 in_limit_Q : 1;
    Uint16 in_limit_P : 1;
    Uint16 in_limit_H : 1;
    Uint16 Conv_active : 1;
    Uint16 PLL_sync : 1;
    Uint16 Grid_present : 1;
    Uint16 SD_no_meter : 1;
    Uint16 wifi_on : 1;

    Uint16 no_CT_connected_a : 1;
    Uint16 no_CT_connected_b : 1;
    Uint16 no_CT_connected_c : 1;
    Uint16 CT_connection_a : 2;
    Uint16 CT_connection_b : 2;
    Uint16 CT_connection_c : 2;

    Uint16 slave_rdy_0 : 1;
    Uint16 slave_rdy_1 : 1;
    Uint16 slave_rdy_2 : 1;
    Uint16 slave_rdy_3 : 1;

    //kolejne slowo od tego miejsca, inaczej pole bitowe error_retry by sie podzielilo na dwa slowa Uint16

    Uint16 error_retry : 4;
    Uint16 expected_number_of_slaves : 4;

    Uint16 scope_trigger_request : 1;
    Uint16 slave_any_sync : 1;
    Uint16 incorrect_number_of_slaves : 1;

    Uint16 rsvd2 : 5;
};

struct CONTROL_master
{
    struct harmonic_odd_struct H_odd_a, H_odd_b, H_odd_c;
    struct harmonic_even_struct H_even_a, H_even_b, H_even_c;
    struct abc_struct Q_set;
    union control_flags_master_struct
    {
        Uint16 all;
        struct
        {
            Uint16 Modbus_FatFS_repeat:1;
            Uint16 enable_Q_comp_a:1;
            Uint16 enable_Q_comp_b:1;
            Uint16 enable_Q_comp_c:1;
            Uint16 enable_P_sym:1;
            Uint16 enable_H_comp:1;
            Uint16 version_Q_comp_a:1;
            Uint16 version_Q_comp_b:1;
            Uint16 version_Q_comp_c:1;
            Uint16 version_P_sym:1;
            Uint16 rsvd2:6;
        }bit;
    }flags;
    union control_triggers_master_struct
    {
        Uint16 all;
        struct
        {
            Uint16 Scope_snapshot:1;
            Uint16 save_to_FLASH:1;
            Uint16 SD_save_H_settings:1;
            Uint16 SD_save_settings:1;
            Uint16 CPU_reset:1;
            Uint16 SD_reset_energy_meter:1;
            Uint16 ONOFF_set:1;
            Uint16 ONOFF_reset:1;
            Uint16 rsvd1:8;
        }bit;
    }triggers;
    struct abc_struct tangens_range[2];
};

struct Measurements_slave_struct
{
    float U_dc_avg;
    float U_dc_n_avg;
    struct abcn_struct I_conv_avg;
    float U_dc;
    float U_dc_n;
    struct abcn_struct I_conv;
    struct abcn_struct Temperature;
    struct abc_struct ExTemperature;
};

struct Measurements_master_struct
{
    struct abc_struct U_grid_avg;
    struct abc_struct I_grid_avg;
    struct abc_struct U_grid;
    struct abc_struct I_grid;
};

struct Measurements_master_gain_offset_struct
{
    struct abc_struct U_grid;
    struct abc_struct I_grid;
};

struct Measurements_slave_gain_offset_struct
{
    float U_dc;
    float U_dc_n;
    struct abcn_struct I_conv;
};

struct timestamp_struct
{
    int16 tx1;
    int16 rx1;
    int16 tx2;
    int16 rx2;
};

struct Slave_meas_struct
{
    int16 U_dc;
    int16 U_dc_n;
    int16 I_conv_a;
    int16 I_conv_b;
    int16 I_conv_c;
    int16 I_conv_n;
};

union ALARM_slave
{
    Uint32 all[2];
    struct
    {
        Uint16 Driver_FLT_a_A : 1;
        Uint16 Driver_FLT_a_B : 1;
        Uint16 Driver_FLT_b_A : 1;
        Uint16 Driver_FLT_b_B : 1;

        Uint16 Driver_FLT_c_A : 1;
        Uint16 Driver_FLT_c_B : 1;
        Uint16 Driver_FLT_n_A : 1;
        Uint16 Driver_FLT_n_B : 1;

        Uint16 Driver_nRDY_a_A : 1;
        Uint16 Driver_nRDY_a_B : 1;
        Uint16 Driver_nRDY_b_A : 1;
        Uint16 Driver_nRDY_b_B : 1;

        Uint16 Driver_nRDY_c_A : 1;
        Uint16 Driver_nRDY_c_B : 1;
        Uint16 Driver_nRDY_n_A : 1;
        Uint16 Driver_nRDY_n_B : 1;
        //16bits
        Uint16 I_conv_a_H:1;
        Uint16 I_conv_a_L:1;
        Uint16 I_conv_b_H:1;
        Uint16 I_conv_b_L:1;

        Uint16 I_conv_c_H:1;
        Uint16 I_conv_c_L:1;
        Uint16 I_conv_n_H:1;
        Uint16 I_conv_n_L:1;

        Uint16 I_conv_rms_a:1;
        Uint16 I_conv_rms_b:1;
        Uint16 I_conv_rms_c:1;
        Uint16 I_conv_rms_n:1;

        Uint16 U_dc_H:1;
        Uint16 U_dc_L:1;
        Uint16 Temperature_H:1;
        Uint16 Temperature_L:1;
        //32bits
        Uint16 Not_enough_data_slave : 1;
        Uint16 CONV_SOFTSTART : 1;
        Uint16 FUSE_BROKEN : 1;
        Uint16 FLT_SUPPLY_SLAVE : 1;

        Uint16 TZ_FPGA_FLT : 1;
        Uint16 TZ_CLOCKFAIL : 1;
        Uint16 TZ_EMUSTOP : 1;
        Uint16 TZ : 1;

        Uint16 sync_error : 1;
        Uint16 rx1_crc_error:1;
        Uint16 rx1_overrun_error:1;
        Uint16 rx1_frame_error:1;
        Uint16 rx2_crc_error:1;
        Uint16 rx2_overrun_error:1;
        Uint16 rx2_frame_error:1;
        Uint16 rx1_port_nrdy:1;
        //48bits
        Uint16 rx2_port_nrdy:1;
        Uint16 sed_err:1;

        Uint16 U_dc_balance:1;
        Uint16 lopri_timeout:1;
        Uint16 lopri_error:1;
        Uint16 msg2_error:1;
        Uint16 msg0_error:1;
        Uint16 rsvd2:9;
    }bit;
};

struct STATUS_slave
{
    Uint16 comm_sync:1;
    Uint16 comm_msg2:1;
    Uint16 Init_done:1;
    Uint16 in_limit_H:1;
    Uint16 Conv_active:1;
    Uint16 calibration_procedure_error:1;
    Uint16 ONOFF_current:1;

    Uint16 SD_card_not_enough_data:1;
    Uint16 SD_no_calibration:1;
    Uint16 SD_no_settings:1;

    Uint16 error_retry:4;

    Uint16 recent_error:1;
    Uint16 Conv_enable:1;
    Uint16 rsvd2:16;
};


struct CONTROL_slave
{
    struct harmonic_odd_struct H_odd_a, H_odd_b, H_odd_c;
    struct harmonic_even_struct H_even_a, H_even_b, H_even_c;
    union control_flags_slave_struct
    {
        Uint16 all;
        struct
        {
            Uint16 enable:1;
            Uint16 enable_H_comp:1;
            Uint16 rsvd:14;
        }bit;
    }flags;
    union control_triggers_slave_struct
    {
        Uint16 all;
        struct
        {
            Uint16 save_to_FLASH:1;
            Uint16 SD_save_settings:1;
            Uint16 CPU_reset:1;
            Uint16 ONOFF_set:1;
            Uint16 ONOFF_reset:1;
            Uint16 rsvd:11;
        }bit;
    }triggers;
};

struct SCOPE_global
{
    Uint16 decimation_ratio:13;
    Uint16 scope_trigger:1;
    Uint16 zero_counter:1;
    Uint16 sync_index:1;
    Uint16 acquire_before_trigger;
};

///////////////////////////////////////////////////////////

struct COMM_header_struct
{
    Uint16 destination_mailbox : 4;
    Uint16 rsvd : 4;
    Uint16 length : 8;
};

union COMM_flags_union
{
    Uint32 all;
    struct
    {
        Uint16 port1_lopri_msg:8;
        Uint16 port1_hipri_msg:8;
        Uint16 port2_lopri_msg:8;
        Uint16 port2_hipri_msg:8;
    }bit;
};

struct COMM_slave_flags_struct
{
    Uint16 sync_rdy : 1;
    Uint16 PWM_EN : 1;
    Uint16 scope_trigger_request:1;
    Uint16 rsvd : 13;
};

enum comm_func_enum
{
    comm_func_modbus_request_mosi,
    comm_func_modbus_request_miso,
    comm_func_modbus_response_mosi,
    comm_func_modbus_response_miso,
    comm_func_scope_mosi,
    comm_func_scope_miso,
    comm_func_async_data_mosi,
    comm_func_async_data_miso,
    comm_func_max,
};

struct modbus_header_struct
{
    Uint16 start_address : 8;
    Uint16 packet_length : 8;
    Uint16 total_length : 16;
};

struct LOG_slave
{
    struct abcn_struct Temperature;
    struct abc_struct Q_conv_1h;
    struct abc_struct P_conv_1h;
};

///////////////////////////////////////////////////////////

struct COMM_slave_sync_msg_struct
{
    struct COMM_header_struct header;
    struct COMM_slave_flags_struct flags;
    struct timestamp_struct timestamp;
    struct Slave_meas_struct slave_meas;
    Uint16 crc;
};

union COMM_master_sync_msg0_union
{
    Uint32 all[32];
    struct
    {
        struct COMM_header_struct header;
        int16 U_grid_a;
        int16 U_grid_b;
        int16 U_grid_c;
        int16 I_grid_a;
        int16 I_grid_b;
        int16 I_grid_c;
        Uint16 crc;
    }fields;
};

union COMM_master_sync_msg2_union
{
    Uint32 all[32];
    struct
    {
        struct COMM_header_struct header;
        Uint16 dummy;
        Uint32 dsc;
        union ALARM_master alarm_master;
        struct SCOPE_global scope_global;
        float ratio[4];
        struct abc_struct I_ref;
        struct abc_struct U_coupl;
        struct abc_struct cosine;
        struct abc_struct Kalman_U_grid;
        Uint16 crc;
    }fields;
};

union COMM_async_msg_union
{
    Uint32 all[32];
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
    }any_frame;
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
        struct modbus_header_struct modbus_header;
        Uint16 data[59];
        Uint16 crc;
    }modbus_data_frame;
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
        struct modbus_header_struct modbus_header;
        Uint16 crc;
    }modbus_request_response;
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
        Uint16 start_address;
        Uint16 crc;
    }scope_master;
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
        Uint32 data[30];
        Uint16 start_address;
        Uint16 crc;
    }scope_slave;
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
        Uint32 code_version;
        Uint32 FatFS_time;
        struct CONTROL_slave control_slave;
        float w_filter;
        struct Measurements_master_gain_offset_struct Meas_master_gain;
        struct Measurements_master_gain_offset_struct Meas_master_offset;
        float compensation2;
        Uint16 crc;
    }async_master;
    struct
    {
        struct COMM_header_struct comm_header;
        enum comm_func_enum comm_func;
        Uint32 code_version;
        struct LOG_slave log_slave;
        struct Measurements_slave_gain_offset_struct Meas_slave_gain;
        struct Measurements_slave_gain_offset_struct Meas_slave_offset;
        union ALARM_slave alarm_slave;
        struct STATUS_slave status_slave;
        float I_lim;
        Uint16 crc;
    }async_slave;
};

#endif /* NODE_SHARED_H_ */
