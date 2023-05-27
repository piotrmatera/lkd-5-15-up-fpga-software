// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

#pragma once
#ifndef Converter_H_
#define Converter_H_

struct abc_Filter1_struct
{
    struct Filter1_struct a;
    struct Filter1_struct b;
    struct Filter1_struct c;
};

struct Converter_struct
{
    struct abc_struct id_conv, iq_conv;
    struct abc_struct id_load, iq_load;

    struct abc_struct tangens_range_local[2];
    struct abc_struct version_Q_comp_local, enable_Q_comp_local;
    struct abc_struct Q_set_local;
    float version_P_sym_local, enable_P_sym_local;
    float enable_H_comp_local;
    struct abc_struct I_ref;
    struct abc_struct U_coupl;

    float Kp_I;
    float Kr_I;
    float range_modifier_Resonant;
    float div_range_modifier_Resonant;
    float range_modifier_Kalman;
    float div_range_modifier_Kalman;
    float Ts_rate;

    float w_filter;
    float f_filter;
    float compensation2;
    float Ts;
    float I_lim_nominal;
    float I_lim;
    float C_dc;
    float L_conv;

    float C_conv;
    float RDY;

    float enable;
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
