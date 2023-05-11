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

    float enable_H_comp_local;

    struct PI_struct PI_Iq[3];
    struct PI_struct PI_Id[3];

    struct abc_struct I_ref;
    struct abc_struct U_coupl;

    float operational_var;

    float compensation2;
    float Ts;
    float I_lim_avg;
    float I_lim_avg_prefilter;
    float I_lim_slave[4];
    struct Filter1_struct I_lim_slave_prefilter[4];
    float ratio[4];

    struct abc_struct Id;
    struct abc_struct Iq;
    struct abc_Filter1_struct Id_prefilter;
    struct abc_Filter1_struct Iq_prefilter;
    float control_type;
    float C_conv;

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
