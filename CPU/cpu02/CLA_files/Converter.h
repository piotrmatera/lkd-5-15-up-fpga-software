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

    float range_modifier_Resonant;
    float div_range_modifier_Resonant;

    float Kp_I;
    float L_conv;
    float Ts;

    float zero_error;
    float sag;
    struct abc_struct Kalman_U_grid;
    struct abc_struct Kalman_U_grid_diff;
    struct abc_struct MR_ref;
    float div_U_dc;
    struct abcn_struct U_ref;
    float cycle_period;
    int32 duty[4];

    float RDY;

    float enable;
};

extern struct Converter_struct Conv;

void Converter_calc();

#endif /* Converter_H_ */
