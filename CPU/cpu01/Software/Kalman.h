// Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h" 

#pragma once 

#ifndef Kalman_H_ 
#define Kalman_H_ 

#define KALMAN_HARMONICS 26

struct Kalman_struct
{
    float states[2 * 2];
    float rms_values[KALMAN_HARMONICS];
    float THD_individual[KALMAN_HARMONICS];
    float estimate;
    float harmonic_RMS;
    float total_RMS;
    float THD_total;
};

extern struct trigonometric_struct sincos_kalman_table[KALMAN_HARMONICS];

extern struct Kalman_struct Kalman_I_grid[3];
extern struct Kalman_struct Kalman_U_grid[3];

extern const float Kalman_gain[2 * KALMAN_HARMONICS];

#endif /* Kalman_H_ */
