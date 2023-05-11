// Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

#pragma once
#ifndef GRID_ANALYZER_H_
#define GRID_ANALYZER_H_

struct Grid_analyzer_struct
{
    float Ts;
    struct trigonometric_struct I_grid_rot[3];
    struct trigonometric_struct zero_rot;

    struct Resonant_struct Resonant_U_grid[3];
    struct Resonant_struct Resonant_I_grid[3];
    struct Resonant_struct Resonant_I_conv[3];

    struct CIC1_adaptive_struct CIC1_U_grid[3];
    struct CIC1_adaptive_struct CIC1_I_grid[3];
    struct CIC1_adaptive_struct CIC1_I_conv[4];
    struct CIC1_adaptive_struct CIC1_U_grid_1h[3];
    struct CIC1_adaptive_struct CIC1_I_grid_1h[3];

    struct CIC1_adaptive_struct CIC1_P_grid_1h[3];
    struct CIC1_adaptive_struct CIC1_P_conv_1h[3];
    struct CIC1_adaptive_struct CIC1_Q_grid_1h[3];
    struct CIC1_adaptive_struct CIC1_Q_conv_1h[3];

    struct Grid_parameters_struct parameters;

    float Accumulator_gain;
    Uint32 input_P_p[3];
    Uint32 input_P_n[3];
    Uint32 input_QI[3];
    Uint32 input_QII[3];
    Uint32 input_QIII[3];
    Uint32 input_QIV[3];
    struct
    {
        Uint32 input_P_p;
        Uint32 input_P_n;
        Uint32 input_QI;
        Uint32 input_QII;
        Uint32 input_QIII;
        Uint32 input_QIV;
    }sum;
};

struct Grid_analyzer_filter_struct
{
    struct CIC1_struct CIC1_U_grid[3];
    struct CIC1_struct CIC1_I_grid[3];
    struct CIC1_struct CIC1_I_conv[4];

    struct CIC1_struct CIC1_I_grid_1h[3];
    struct CIC1_struct CIC1_U_grid_1h[3];

    struct CIC1_struct CIC1_P_grid_1h[3];
    struct CIC1_struct CIC1_P_conv_1h[3];
    struct CIC1_struct CIC1_Q_grid_1h[3];
    struct CIC1_struct CIC1_Q_conv_1h[3];

    struct CIC1_struct CIC1_THD_U_grid[3];
    struct CIC1_struct CIC1_THD_I_grid[3];

    struct Grid_parameters_struct parameters;
};

extern struct Grid_analyzer_struct Grid;
extern struct Grid_analyzer_filter_struct Grid_filter;

void Grid_analyzer_calc();
void Grid_analyzer_filter_calc();

#endif /* GRID_ANALYZER_H_ */
