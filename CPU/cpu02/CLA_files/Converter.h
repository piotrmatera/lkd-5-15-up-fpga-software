// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

#pragma once
#ifndef Converter_H_
#define Converter_H_

struct Converter_struct
{
    struct abc_struct id_ref, iq_ref;
    struct abc_struct I_ref, I_err;
    struct abc_struct U_coupl;

    float range_modifier_Resonant_values;
    float div_range_modifier_Resonant_values;

    float range_modifier_Kalman_values;
    float div_range_modifier_Kalman_values;

    float Kp_I;
    float Kr_I;
    float L_conv;
    float Ts;

    float zero_error;
    float sag;
    struct abc_struct Kalman_U_grid;
    struct abc_struct Kalman_U_grid_diff;
    struct abc_struct MR_ref;
    struct abc_struct MR_ref_CPU;
    struct abcn_struct U_ref;
    float cycle_period;
    float duty_float[4];
    int16 duty[4];

    float correction;
    float correction_switch;

    float compensation;
    float resonant_odd_number;
    float resonant_even_number;
    struct Resonant_struct Resonant_I_a_odd[25];
    struct Resonant_struct Resonant_I_b_odd[25];
    struct Resonant_struct Resonant_I_c_odd[25];

    struct Resonant_struct Resonant_I_a_even[2];
    struct Resonant_struct Resonant_I_b_even[2];
    struct Resonant_struct Resonant_I_c_even[2];

    float RDY;

    float enable;
};

extern struct Converter_struct Conv;

void Converter_calc();

#endif /* Converter_H_ */
