// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

#pragma once
#ifndef Converter_H_
#define Converter_H_

enum Converter_state_enum
{
    CONV_softstart,
    CONV_grid_relay,
    CONV_active,
    __dummybig_CONV = 300000
};

struct abc_Filter1_struct
{
    struct Filter1_struct a;
    struct Filter1_struct b;
    struct Filter1_struct c;
};

struct Converter_struct
{
    float P_conv;
    struct CIC1_struct P_conv_filter;
    float I_dc;
    float U_dc_100Hz_sqr;
    float Q_100Hz;
    float C_dc_meas;
    struct CIC1_struct C_dc_filter;
    float C_dc_measured;

    struct trigonometric_struct I_grid_rot[3];
    struct trigonometric_struct zero_rot;

    struct Resonant_struct Resonant_U_grid[3];
    struct Resonant_struct Resonant_I_grid[3];
    struct Resonant_struct Resonant_I_conv[3];

    //////////////////////////////

    struct
    {
        struct
        {
            struct abc_struct P_conv_1h;
            struct abc_struct Q_conv_1h;
            float I_lim;
            struct abc_struct P_conv_1h_filter;
            struct abc_struct Q_conv_1h_filter;
            float C_conv;
        }from_slave[4];
        struct
        {
            float ratio;
            struct Filter1_struct I_lim_prefilter;
        }slave[5];
        struct
        {
            struct abcn_struct id_lim, iq_lim;
            struct abc_struct P_conv_1h, Q_conv_1h;
            struct abc_struct P_conv_1h_filter, Q_conv_1h_filter;
            float I_lim;
            float I_lim_prefilter;
            float C_conv;
        }total;
    }master;

    struct abc_struct id_conv, iq_conv;
    struct abc_struct id_load, iq_load;
    struct abc_struct id_grid, iq_grid;

    struct abc_struct tangens_range_local[2];
    struct abc_Filter1_struct tangens_range_local_prefilter[2];
    struct abc_struct version_Q_comp_local, enable_Q_comp_local;
    struct abc_Filter1_struct version_Q_comp_local_prefilter, enable_Q_comp_local_prefilter;
    struct abc_struct Q_set_local;
    struct abc_Filter1_struct Q_set_local_prefilter;
    struct abc_struct iq_load_ref;
    float Iq_x, Iq_y;

    float version_P_sym_local, enable_P_sym_local;
    struct Filter1_struct version_P_sym_local_prefilter, enable_P_sym_local_prefilter;
    float Id_x, Id_y;

    //////////////////////////////

    struct
    {
        struct
        {
            struct abc_struct id_lim;
            struct abc_struct iq_lim;
            float ratio[4];
        }from_master;
        float ratio_local;
    }slave;

    //////////////////////////////

    float U_grid_phph_max;

    float U_dc_ref;
    struct CIC1_adaptive_struct CIC1_U_dc;
    float U_dc_filter;
    float U_dc_kalman;
    struct PI_struct PI_U_dc;

    struct Filter1_struct master_slave_prefilter;
    struct abc_struct id_ref, iq_ref;
    struct PI_struct PI_Iq[3];
    struct PI_struct PI_Id[3];

    float sag;
    float enable_H_comp_local;
    struct abcn_struct I_conv_max;
    struct PI_struct PI_I_harm_ratio[4];

    float resonant_even_number;
    float resonant_odd_number;
    float Kp_I;
    float Kr_I;
    float compensation;
    float compensation2;
    float I_lim, I_lim_nominal;
    float select_modulation;

    float w_filter;
    float f_filter;
    float sign;
    float PLL_RDY;

    float Ts;
    float Ts_rate;
    float C_dc;
    float L_conv;
    float C_conv;

    float RDY, RDY2;

    float enable;
    enum Converter_state_enum state, state_last;

    float range_modifier_Resonant_coefficients;
    float div_range_modifier_Resonant_coefficients;
    float range_modifier_Resonant_values;
    float div_range_modifier_Resonant_values;

    float range_modifier_Kalman_coefficients;
    float div_range_modifier_Kalman_coefficients;
    float range_modifier_Kalman_values;
    float div_range_modifier_Kalman_values;
    float div_range_modifier_Kalman_values_square;
};

extern struct Converter_struct Conv;

extern float on_off_odd_a[25];
extern float on_off_odd_b[25];
extern float on_off_odd_c[25];

extern float on_off_even_a[25];
extern float on_off_even_b[25];
extern float on_off_even_c[25];

void Converter_calc_slave();
void Converter_calc_master();

#endif /* Converter_H_ */
