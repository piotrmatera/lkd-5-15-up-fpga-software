/*
 * stdafx.c
 *
 *  Created on: 13 paz 2019
 *      Author: Mr.Tea
 */

#include "stdafx.h"

#pragma SET_DATA_SECTION("CLAData")

struct CLA1toCLA2_struct CLA1toCLA2;
struct Grid_analyzer_struct Grid;
struct Grid_analyzer_filter_struct Grid_filter;

struct Timer_PWM_struct Timer_PWM;

struct Measurements_master_struct Meas_master;
struct Measurements_slave_struct Meas_slave;

struct trigonometric_struct sincos_table[SINCOS_HARMONICS];
struct CIC1_adaptive_global_struct CIC1_adaptive_global__50Hz;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma SET_DATA_SECTION("CPU1TOCPU2")

struct CPU1toCPU2_struct CPU1toCPU2;

#pragma SET_DATA_SECTION("CPU2TOCPU1")

struct CPU2toCPU1_struct CPU2toCPU1;

#pragma SET_DATA_SECTION("KALMAN")

struct Kalman_struct Kalman_I_grid[3];
struct Kalman_struct Kalman_U_grid[3];

#pragma SET_DATA_SECTION()

struct trigonometric_struct sincos_kalman_table[KALMAN_HARMONICS];
