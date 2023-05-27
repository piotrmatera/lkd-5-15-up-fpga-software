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

struct Measurements_alarm_struct
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

union FPGA_master_sync_flags_union
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
        Uint16 rsvd:8;
    }bit;
};

#define FPGA_RESONANT_STATES 25
#define FPGA_RESONANT_SERIES 4

struct FPGA_Resonant_M0_struct
{
    int32 x1;
    int32 x2;
};

struct FPGA_Resonant_M1_struct
{
    int32 cos_A;
    int32 sin_A;
    int32 cos_B;
    int32 sin_B;
    int32 cos_C;
    int32 sin_C;
};

#define FPGA_KALMAN_STATES 26
#define FPGA_KALMAN_SERIES 4

struct FPGA_Kalman_M0_struct
{
    int32 x1;
    int32 x2;
};

struct FPGA_Kalman_M1_struct
{
    int32 cos_K;
    int32 sin_K;
    int32 K1;
    int32 K2;
};

union EMIF_union
{
    struct
    {
        union COMM_flags_union tx_wip;
        union COMM_flags_union rx_rdy;
        struct EMIF_SD_struct SD_new;
        struct EMIF_SD_struct SD_avg;
        union FPGA_master_flags_union FPGA_flags;
        union FPGA_master_sync_flags_union Sync_flags;
        int16 clock_offsets[8];
        int16 comm_delays[8];
        Uint32 SD_sync_val;
        Uint32 dsc;
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
            Uint32 sync_phase:1;
            Uint32 Resonant1_WIP:1;
            Uint32 Resonant2_WIP:1;
            Uint32 Kalman1_WIP:1;
            Uint32 Kalman2_WIP:1;
        }flags;
        Uint32 mux_rsvd[1024-35];
        Uint32 rx1_lopri_msg[8][32];
        Uint32 rx1_hipri_msg[8][32];
        Uint32 rx2_lopri_msg[8][32];
        Uint32 rx2_hipri_msg[8][32];
        struct
        {
            struct
            {
                struct FPGA_Resonant_M0_struct harmonic[FPGA_RESONANT_STATES];
                int32 error;
                int32 sum;
                Uint32 rsvd[512/FPGA_RESONANT_SERIES - 2 - FPGA_RESONANT_STATES*sizeof(struct FPGA_Resonant_M0_struct)/sizeof(Uint32)];
            }series[FPGA_RESONANT_SERIES];
        }Resonant[2];
        struct
        {
            struct
            {
                struct FPGA_Kalman_M0_struct harmonic[FPGA_KALMAN_STATES];
                int32 estimate;
                int32 error;
                Uint32 rsvd[512/FPGA_KALMAN_SERIES - 2 - FPGA_KALMAN_STATES*sizeof(struct FPGA_Kalman_M0_struct)/sizeof(Uint32)];
            }series[FPGA_KALMAN_SERIES];
        }Kalman[2];
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
        Uint32 mux_rsvd[1024-18];
        Uint32 tx1_lopri_msg[8][32];
        Uint32 tx1_hipri_msg[8][32];
        Uint32 tx2_lopri_msg[8][32];
        Uint32 tx2_hipri_msg[8][32];
        struct
        {
            struct FPGA_Resonant_M1_struct harmonic[FPGA_RESONANT_STATES];
            struct
            {
                int32 error;
                int32 harmonics;
            }series[FPGA_RESONANT_SERIES];
            Uint32 rsvd[512 - FPGA_RESONANT_SERIES*2 - FPGA_RESONANT_STATES*sizeof(struct FPGA_Resonant_M1_struct)/sizeof(Uint32)];
        }Resonant[2];
        struct
        {
            struct FPGA_Kalman_M1_struct harmonic[FPGA_KALMAN_STATES];
            struct
            {
                int32 input;
            }series[FPGA_KALMAN_SERIES];
            Uint32 rsvd[512 - FPGA_KALMAN_SERIES - FPGA_KALMAN_STATES*sizeof(struct FPGA_Kalman_M1_struct)/sizeof(Uint32)];
        }Kalman[2];
    }write;
};

struct CLA1toCLA2_struct
{
    struct abc_struct id_ref, iq_ref;
    float Kp_I;
    float L_conv;
    float enable;
};

struct CLA2toCLA1_struct
{
    float dummy;
};

struct CPU1toCPU2_struct
{
    struct CLA1toCLA2_struct CLA1toCLA2;
    struct Measurements_master_gain_offset_struct Meas_master_gain;
    struct Measurements_master_gain_offset_struct Meas_master_offset;
    float clear_alarms;
};

struct CPU2toCPU1_struct
{
    struct CLA2toCLA1_struct CLA2toCLA1;
    float w_filter;
    float f_filter;
    float sign;
    float PLL_RDY;
    union ALARM_master alarm_master;
};

extern struct CPU1toCPU2_struct CPU1toCPU2;
extern struct CPU2toCPU1_struct CPU2toCPU1;

extern volatile union EMIF_union EMIF_mem;

#endif /* CPU_SHARED_H_ */
