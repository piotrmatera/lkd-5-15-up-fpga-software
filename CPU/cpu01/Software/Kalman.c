// Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

#include <math.h>
#include "stdafx.h" 

float Kalman_gain[2 * FPGA_KALMAN_STATES] = {0};
float Kalman_gain_dc[2 * FPGA_KALMAN_DC_STATES] = {0};
