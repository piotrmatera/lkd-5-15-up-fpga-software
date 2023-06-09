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

    float range_modifier_Resonant_coefficients;
    float div_range_modifier_Resonant_coefficients;
    float range_modifier_Resonant_values;
    float div_range_modifier_Resonant_values;

    float range_modifier_Kalman_coefficients;
    float div_range_modifier_Kalman_coefficients;
    float range_modifier_Kalman_values;
    float div_range_modifier_Kalman_values;

    float Kp_I;
    float L_conv;
    float Ts;

    float zero_error;
    float sag;
    struct abc_struct Kalman_U_grid;
    struct abc_struct Kalman_U_grid_diff;
    struct abc_struct MR_ref;
    struct abcn_struct U_ref;
    float cycle_period;
    float duty_float[4];
    int16 duty[4];

    float correction;
    float correction_switch;

    float RDY;

    float enable;
};

extern struct Converter_struct Conv;

void Converter_calc();

#endif /* Converter_H_ */
