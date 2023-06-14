// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h" 

#pragma once 

#ifndef Kalman_H_ 
#define Kalman_H_ 

struct Kalman_struct
{
    float states[2 * 2];
    float rms_values[FPGA_KALMAN_STATES];
    float THD_individual[FPGA_KALMAN_STATES];
    float estimate;
    float harmonic_RMS;
    float total_RMS;
    float THD_total;
};

extern struct trigonometric_struct sincos_kalman_table[SINCOS_HARMONICS];

extern struct Kalman_struct Kalman_I_grid[3];
extern struct Kalman_struct Kalman_U_grid[3];

extern float Kalman_gain[2 * FPGA_KALMAN_STATES];
extern float Kalman_gain_dc[2 * FPGA_KALMAN_DC_STATES];

#endif /* Kalman_H_ */
