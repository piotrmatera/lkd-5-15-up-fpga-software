// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

#pragma once
#ifndef GRID_ANALYZER_H_
#define GRID_ANALYZER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct Grid_parameters_struct
{
    struct abc_struct I_grid_1h;
    struct abc_struct U_grid_1h;
    struct abc_struct P_grid_1h;
    struct abc_struct P_load_1h;
    struct abc_struct P_conv_1h;
    struct abc_struct Q_grid_1h;
    struct abc_struct Q_load_1h;
    struct abc_struct Q_conv_1h;
    struct abc_struct S_grid_1h;
    struct abc_struct S_load_1h;
    struct abc_struct S_conv_1h;
    struct abc_struct PF_grid_1h;
    struct abc_struct THD_I_grid;
    struct abc_struct THD_U_grid;
    struct abc_struct U_grid;
    struct abc_struct I_grid;
    struct abcn_struct I_conv;
    struct abc_struct S_grid;
    struct abc_struct S_conv;
    struct abcn_struct Used_resources;
    struct
    {
        float PF_grid_1h;
        float P_load_1h;
        float Q_load_1h;
        float S_load_1h;
        float P_grid_1h;
        float Q_grid_1h;
        float S_grid_1h;
        float U_grid_1h;
    }average;
};

struct Grid_analyzer_struct
{
    float Ts;
//    struct trigonometric_struct I_grid_rot[3];
//    struct trigonometric_struct zero_rot;
//
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
};

extern struct Grid_analyzer_struct Grid_params;
extern struct Grid_parameters_struct Grid;

void Grid_analyzer_calc();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
};

extern struct Grid_analyzer_filter_struct Grid_filter_params;
extern struct Grid_parameters_struct Grid_filter;

void Grid_analyzer_filter_calc();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Energy_meter_upper_struct
{
    Uint64 P_p[3];
    Uint64 P_n[3];
    Uint64 QI[3];
    Uint64 QII[3];
    Uint64 QIII[3];
    Uint64 QIV[3];
    struct
    {
        Uint64 P_p;
        Uint64 P_n;
        Uint64 QI;
        Uint64 QII;
        Uint64 QIII;
        Uint64 QIV;
    }sum;
};

struct Energy_meter_lower_struct
{
    Uint32 P_p[3];
    Uint32 P_n[3];
    Uint32 QI[3];
    Uint32 QII[3];
    Uint32 QIII[3];
    Uint32 QIV[3];
    struct
    {
        Uint32 P_p;
        Uint32 P_n;
        Uint32 QI;
        Uint32 QII;
        Uint32 QIII;
        Uint32 QIV;
    }sum;
};

struct Energy_meter_struct
{
    struct Energy_meter_upper_struct upper;
    struct Energy_meter_lower_struct lower;
};

struct Energy_meter_params_struct
{
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

extern struct Energy_meter_struct Energy_meter;
extern struct Energy_meter_params_struct Energy_meter_params;

void Energy_meter_calc();

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* GRID_ANALYZER_H_ */
