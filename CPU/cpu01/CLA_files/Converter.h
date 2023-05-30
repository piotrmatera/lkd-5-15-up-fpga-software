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
    struct Filter1_struct P_conv_filter;

    struct trigonometric_struct I_grid_rot[3];
    struct trigonometric_struct zero_rot;

    struct Resonant_struct Resonant_U_grid[3];
    struct Resonant_struct Resonant_I_grid[3];
    struct Resonant_struct Resonant_I_conv[3];

    //////////////////////////////

    float U_grid_phph_max;

    float div_U_dc;
    float U_dc_ref;
    struct CIC1_adaptive_struct CIC1_U_dc;
    float U_dc_filter;
    float U_dc_kalman;
    struct PI_struct PI_U_dc;

    struct abc_struct id_conv, iq_conv;
    struct abc_struct id_load, iq_load;

    struct abc_struct tangens_range_local[2];
    struct abc_Filter1_struct tangens_range_local_prefilter[2];
    struct abc_struct version_Q_comp_local, enable_Q_comp_local;
    struct abc_Filter1_struct version_Q_comp_local_prefilter, enable_Q_comp_local_prefilter;
    struct abc_struct Q_set_local;
    struct abc_Filter1_struct Q_set_local_prefilter;
    struct abc_struct iq_load_ref;
    struct abcn_struct iq_lim;
    float Iq_x, Iq_y;

    float version_P_sym_local, enable_P_sym_local;
    struct Filter1_struct version_P_sym_local_prefilter, enable_P_sym_local_prefilter;
    struct abcn_struct id_lim;
    float Id_x, Id_y;

    struct PI_struct PI_Iq[3];
    struct PI_struct PI_Id[3];

    float sag;
    float enable_H_comp_local;
    struct abcn_struct I_conv_max;
    struct PI_struct PI_I_harm_ratio[4];

    Uint32 resonant_even_number;
    Uint32 resonant_odd_number;
    float Kp_I;
    float Kr_I;
    float compensation2;
    float I_lim, I_lim_nominal;

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
};

extern struct Converter_struct Conv;

extern float on_off_odd_a[25];
extern float on_off_odd_b[25];
extern float on_off_odd_c[25];

extern float on_off_even_a[25];
extern float on_off_even_b[25];
extern float on_off_even_c[25];

void Converter_calc();

#endif /* Converter_H_ */
