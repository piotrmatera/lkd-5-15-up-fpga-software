/*
 * CPU_shared.h
 *
 *  Created on: 16 sie 2020
 *      Author: MrTea
 */

#ifndef CPU_SHARED_H_
#define CPU_SHARED_H_

#include "IO.h"
#include "Node_shared.h"

struct Measurements_ACDC_alarm_struct
{
    float U_grid_abs;
    float U_grid_rms;
    float Temp;
    float I_conv;
    float I_conv_rms;
    float U_dc;
    float U_dc_balance;
};

struct EMIF_SD_struct
{
    int16 U_grid_a;
    int16 U_grid_b;
    int16 U_grid_c;
    int16 I_grid_a;
    int16 I_grid_b;
    int16 I_grid_c;
    int16 U_dc;
    int16 U_dc_n;
    int16 I_conv_a;
    int16 I_conv_b;
    int16 I_conv_c;
    int16 I_conv_n;
};

union FPGA_ACDC_sync_flags_union
{
    Uint32 all;
    struct
    {
        Uint16 rx_ok_0:1;
        Uint16 rx_ok_1:1;
        Uint16 rx_ok_2:1;
        Uint16 rx_ok_3:1;
        Uint16 rx_ok_4:1;
        Uint16 rx_ok_5:1;
        Uint16 rx_ok_6:1;
        Uint16 rx_ok_7:1;
        Uint16 sync_ok_0:1;
        Uint16 sync_ok_1:1;
        Uint16 sync_ok_2:1;
        Uint16 sync_ok_3:1;
        Uint16 sync_ok_4:1;
        Uint16 sync_ok_5:1;
        Uint16 sync_ok_6:1;
        Uint16 sync_ok_7:1;
        Uint16 slave_rdy_0:1;
        Uint16 slave_rdy_1:1;
        Uint16 slave_rdy_2:1;
        Uint16 slave_rdy_3:1;
        Uint16 slave_rdy_4:1;
        Uint16 slave_rdy_5:1;
        Uint16 slave_rdy_6:1;
        Uint16 slave_rdy_7:1;
        Uint16 node_number:3;
        Uint16 node_number_rdy:1;
        Uint16 sync_rdy:1;
        Uint16 master_rdy:1;
        Uint16 rx1_port_rdy:1;
        Uint16 rx2_port_rdy:1;
    }bit;
};

#define FPGA_RESONANT_STATES 25
#define FPGA_RESONANT_SERIES 3

struct FPGA_Resonant_states_M0_struct
{
    int32 x1;
    int32 x2;
};

struct FPGA_Resonant_series_M0_struct
{
    struct FPGA_Resonant_states_M0_struct states[FPGA_RESONANT_STATES];
    int32 sum;
    int32 error;
};

struct FPGA_Resonant_M0_struct
{
    struct FPGA_Resonant_series_M0_struct series[FPGA_RESONANT_SERIES];
    Uint32 rsvd[512 - FPGA_RESONANT_SERIES*sizeof(struct FPGA_Resonant_series_M0_struct)/sizeof(Uint32)];
};

struct FPGA_Resonant_states_M1_struct
{
    int32 cos_A;
    int32 sin_A;
    int32 cos_B;
    int32 sin_B;
    int32 cos_C;
    int32 sin_C;
};

struct FPGA_Resonant_series_M1_struct
{
    int32 error;
    Uint32 harmonics_number;
};

struct FPGA_Resonant_M1_struct
{
    struct FPGA_Resonant_states_M1_struct states[FPGA_RESONANT_STATES];
    struct FPGA_Resonant_series_M1_struct series[FPGA_RESONANT_SERIES];
    Uint32 rsvd[512 - FPGA_RESONANT_STATES*sizeof(struct FPGA_Resonant_states_M1_struct)/sizeof(Uint32) - FPGA_RESONANT_SERIES*sizeof(struct FPGA_Resonant_series_M1_struct)/sizeof(Uint32)];
};

#define FPGA_RESONANT_GRID_STATES 25
#define FPGA_RESONANT_GRID_SERIES 1

struct FPGA_Resonant_grid_states_M0_struct
{
    int32 GX1;
    int32 GX2;
    int32 CX1;
    int32 CX2;
    int32 ERR;
};

struct FPGA_Resonant_grid_series_M0_struct
{
    struct FPGA_Resonant_grid_states_M0_struct states[FPGA_RESONANT_GRID_STATES];
    int32 SUM;
    int32 IC;
};

struct FPGA_Resonant_grid_M0_struct
{
    struct FPGA_Resonant_grid_series_M0_struct series[FPGA_RESONANT_GRID_SERIES];
    Uint32 rsvd[512 - FPGA_RESONANT_GRID_SERIES*sizeof(struct FPGA_Resonant_grid_series_M0_struct)/sizeof(Uint32)];
};

struct FPGA_Resonant_grid_states_M1_struct
{
    int32 CA;
    int32 SA;
    int32 GCB;
    int32 GSB;
    int32 CCB;
    int32 CSB;
    int32 GCC;
    int32 GSC;
    int32 CCC;
    int32 CSC;
};

struct FPGA_Resonant_grid_series_M1_struct
{
    Uint32 HC;
    Uint32 HG;
    Uint32 ZR;
    int32 RT;
    int32 IC;
    int32 IG;
};

struct FPGA_Resonant_grid_M1_struct
{
    struct FPGA_Resonant_grid_states_M1_struct states[FPGA_RESONANT_GRID_STATES];
    struct FPGA_Resonant_grid_series_M1_struct series[FPGA_RESONANT_GRID_SERIES];
    Uint32 rsvd[512 - FPGA_RESONANT_GRID_STATES*sizeof(struct FPGA_Resonant_grid_states_M1_struct)/sizeof(Uint32) - FPGA_RESONANT_GRID_SERIES*sizeof(struct FPGA_Resonant_grid_series_M1_struct)/sizeof(Uint32)];
};

#define FPGA_KALMAN_STATES 26
#define FPGA_KALMAN_SERIES 6

#define FPGA_KALMAN_DC_STATES 50
#define FPGA_KALMAN_DC_SERIES 1

struct FPGA_Kalman_states_M0_struct
{
    int32 x1;
    int32 x2;
    int32 A;
};

struct FPGA_Kalman_series_M0_struct
{
    struct FPGA_Kalman_states_M0_struct states[FPGA_KALMAN_STATES];
    int32 estimate;
    int32 error;
};

struct FPGA_Kalman_DC_series_M0_struct
{
    struct FPGA_Kalman_states_M0_struct states[FPGA_KALMAN_DC_STATES];
    int32 estimate;
    int32 error;
};

struct FPGA_Kalman_M0_struct
{
    struct FPGA_Kalman_series_M0_struct series[FPGA_KALMAN_SERIES];
    Uint32 rsvd[512 - FPGA_KALMAN_SERIES*sizeof(struct FPGA_Kalman_series_M0_struct)/sizeof(Uint32)];
};

struct FPGA_Kalman_DC_M0_struct
{
    struct FPGA_Kalman_DC_series_M0_struct series[FPGA_KALMAN_DC_SERIES];
    Uint32 rsvd[512 - FPGA_KALMAN_DC_SERIES*sizeof(struct FPGA_Kalman_DC_series_M0_struct)/sizeof(Uint32)];
};

struct FPGA_Kalman_states_M1_struct
{
    int32 cos_K;
    int32 sin_K;
    int32 K1;
    int32 K2;
};

struct FPGA_Kalman_M1_struct
{
    struct FPGA_Kalman_states_M1_struct states[FPGA_KALMAN_STATES];
    int32 input[FPGA_KALMAN_SERIES];
    Uint32 rsvd[512 - FPGA_KALMAN_SERIES - FPGA_KALMAN_STATES*sizeof(struct FPGA_Kalman_states_M1_struct)/sizeof(Uint32)];
};

struct FPGA_Kalman_DC_M1_struct
{
    struct FPGA_Kalman_states_M1_struct states[FPGA_KALMAN_DC_STATES];
    int32 input[FPGA_KALMAN_DC_SERIES];
    Uint32 rsvd[512 - FPGA_KALMAN_DC_SERIES - FPGA_KALMAN_DC_STATES*sizeof(struct FPGA_Kalman_states_M1_struct)/sizeof(Uint32)];
};

struct Grid_analyzer_FPGA_struct
{
    int32 CIC1_U_grid[3];
    int32 CIC1_I_grid[3];
    int32 CIC1_I_conv[4];
    int32 CIC1_U_grid_1h[3];
    int32 CIC1_I_grid_1h[3];

    int32 CIC1_P_grid_1h[3];
    int32 CIC1_P_conv_1h[3];
    int32 CIC1_Q_grid_1h[3];
    int32 CIC1_Q_conv_1h[3];
};

union EMIF_union
{
    struct
    {
        union COMM_flags_union tx_wip;
        union COMM_flags_union rx_wip;
        union COMM_flags_union rx_rdy;
        struct EMIF_SD_struct SD_new;
        struct EMIF_SD_struct SD_avg;
        union FPGA_master_flags_union FPGA_flags;
        union FPGA_ACDC_sync_flags_union Sync_flags;
        //16 Uint32
        Uint32 SD_sync_val;
        struct
        {
            Uint32 version:16;
            Uint32 power:12;
            Uint32 type:4;
        }id_number;
        Uint32 Scope_data_out1;
        Uint32 Scope_data_out2;
        Uint32 Scope_depth;
        Uint32 Scope_width_mult;
        Uint32 Scope_rdy;
        Uint32 Scope_index_last;
        Uint16 cycle_period;
        Uint16 control_rate;
        Uint16 def_osr;
        Uint16 sd_shift;
        struct
        {
            Uint32 Resonant1_WIP:1;
            Uint32 Resonant2_WIP:1;
            Uint32 Resonant3_WIP:1;
            Uint32 Resonant4_WIP:1;
            Uint32 Resonant5_WIP:1;
            Uint32 Resonant6_WIP:1;
            Uint32 Kalman_DC_WIP:1;
            Uint32 Kalman_WIP:1;
            Uint32 sync_phase:1;
            Uint32 rsvd:20;
            Uint32 ext_miller:1;
            Uint32 double_pulse:1;
            Uint32 calibration:1;
        }flags;
        Uint32 next_period;
        float Thermistor_module[4];
        int16 Temperature_FPGA;
        Uint16 dummy;
        Uint32 mux_rsvd[512-34];
        Uint32 rx1_lopri_msg[8][32];
        Uint32 rx1_hipri_msg[8][32];
        Uint32 rx2_lopri_msg[8][32];
        Uint32 rx2_hipri_msg[8][32];
        struct FPGA_Resonant_grid_M0_struct Resonant[6];
        struct FPGA_Kalman_DC_M0_struct Kalman_DC;
        struct FPGA_Kalman_M0_struct Kalman;
        struct Grid_analyzer_FPGA_struct Grid;
    }read;
    struct
    {
        union COMM_flags_union tx_start;
        union COMM_flags_union rx_ack;
        Uint32 SD_sync_val;
        Uint32 Scope_address;
        Uint32 Scope_acquire_before_trigger;
        Uint32 Scope_trigger;
        int16 duty[4];
        Uint32 double_pulse;
        Uint32 DSP_start;
        Uint32 PWM_control;
        Uint32 I_conv_a_lim;
        Uint32 I_conv_b_lim;
        Uint32 I_conv_c_lim;
        Uint32 I_conv_n_lim;
        Uint32 U_grid_a_lim;
        Uint32 U_grid_b_lim;
        Uint32 U_grid_c_lim;
        int32 local_counter_phase_shift;
        Uint32 mux_rsvd[512-19];
        Uint32 tx1_lopri_msg[8][32];
        Uint32 tx1_hipri_msg[8][32];
        Uint32 tx2_lopri_msg[8][32];
        Uint32 tx2_hipri_msg[8][32];
        struct FPGA_Resonant_grid_M1_struct Resonant[6];
        struct FPGA_Kalman_DC_M1_struct Kalman_DC;
        struct FPGA_Kalman_M1_struct Kalman;
        struct Grid_analyzer_FPGA_struct Grid;
    }write;
};

struct CLA1toCLA2_struct
{
    struct abc_struct id_ref, iq_ref;
    float Ts;
    float Kp_I;
    float L_conv;
    float enable;
    float select_modulation;
    float no_neutral;
    float range_modifier_Resonant_values;
    float range_modifier_Kalman_values;
    float range_modifier_Resonant_coefficients;
    float range_modifier_Kalman_coefficients;
};

struct CLA2toCLA1_struct
{
    float dummy;
};

struct CPU1toCPU2_struct
{
    struct CLA1toCLA2_struct CLA1toCLA2;
    struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain;
    struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset;
    float clear_alarms;
};

struct CPU2toCPU1_struct
{
    struct CLA2toCLA1_struct CLA2toCLA1;
    float w_filter;
    float f_filter;
    float sign;
    float PLL_RDY;
    float sag;
    union ALARM_ACDC alarm_master;
    int32 Resonant_error[6];
    Uint32 ZR;
    Uint32 Resonant_start;
    int16 duty[4];
};

extern struct CPU1toCPU2_struct CPU1toCPU2;
extern struct CPU2toCPU1_struct CPU2toCPU1;

extern volatile union EMIF_union EMIF_mem;

#endif /* CPU_SHARED_H_ */
