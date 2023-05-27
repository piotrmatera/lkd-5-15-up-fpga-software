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
    float U_grid_phph_max;

    float div_U_dc;
    float U_dc_ref;
    struct CIC1_adaptive_struct CIC1_U_dc;
    float U_dc_filter;
    float U_dc_kalman;
    struct PI_struct PI_U_dc;

    float enable_H_comp_local;
    struct abcn_struct I_conv_max;
    struct PI_struct PI_I_harm_ratio[4];

    struct abc_struct Kalman_U_grid;
    struct abc_struct Kalman_U_grid_diff;
    float sag;

    struct abc_struct id_conv, iq_conv;
    struct abc_struct id_load, iq_load;

    struct abc_struct tangens_range_local[2];
    struct abc_struct version_Q_comp_local, enable_Q_comp_local;
    struct abc_struct Q_set_local;
    float version_P_sym_local, enable_P_sym_local;
    struct abc_struct I_ref;
    struct abc_struct U_coupl;

    float Kp_I;
    float Kr_I;
    float Ts_rate;

    float range_modifier_Resonant_coefficients;
    float div_range_modifier_Resonant_coefficients;
    float range_modifier_Resonant_values;
    float div_range_modifier_Resonant_values;

    float range_modifier_Kalman_coefficients;
    float div_range_modifier_Kalman_coefficients;
    float range_modifier_Kalman_values;
    float div_range_modifier_Kalman_values;

    float w_filter;
    float f_filter;
    float compensation2;
    float Ts;
    float I_lim, I_lim_nominal;
    float C_dc;
    float L_conv;

    float C_conv;
    float RDY, RDY2;

    float enable;
    enum Converter_state_enum state, state_last;
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
