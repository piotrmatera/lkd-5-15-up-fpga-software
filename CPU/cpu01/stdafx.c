/*
 * stdafx.c
 *
 *  Created on: 13 paz 2019
 *      Author: Mr.Tea
 */

#include "stdafx.h"

#pragma SET_DATA_SECTION("CLAData")

struct PLL_struct PLL;
struct Converter_struct Conv;
struct CLA2toCLA1_struct CLA2toCLA1;

struct SCOPE_global scope_global;

union CONTROL_EXT_MODBUS control_ext_modbus;

struct CONTROL_master control_master;
struct STATUS_master status_master;
union ALARM_master alarm_master;
union ALARM_master alarm_master_snapshot;

struct Timer_PWM_struct Timer_PWM;

struct CIC2_struct CIC2_calibration;
CLA_FPTR CIC2_calibration_input;

struct Measurements_master_struct Meas_master;
struct Measurements_master_gain_offset_struct Meas_master_gain_error;
struct Measurements_master_gain_offset_struct Meas_master_offset_error;
struct Measurements_master_gain_offset_struct Meas_master_gain;
struct Measurements_master_gain_offset_struct Meas_master_offset;
struct Measurements_alarm_struct Meas_alarm_H;
struct Measurements_alarm_struct Meas_alarm_L;

struct trigonometric_struct sincos_table[SINCOS_HARMONICS];

struct abc_struct U_x0, U_x1;
float decimator;

struct EMIF_CLA_struct EMIF_CLA;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma SET_DATA_SECTION("CPUTOCLA")

#pragma SET_DATA_SECTION("CPU1TOCPU2")

struct CPU1toCPU2_struct CPU1toCPU2;

#pragma SET_DATA_SECTION("CPU2TOCPU1")

struct CPU2toCPU1_struct CPU2toCPU1;

#pragma SET_DATA_SECTION("KALMAN")

struct Kalman_struct Kalman_I_grid[3];
struct Kalman_struct Kalman_U_grid[3];

#pragma SET_DATA_SECTION("EMIF_mem")

volatile union EMIF_union EMIF_mem;

#pragma SET_DATA_SECTION()

struct Energy_meter_struct Energy_meter;

struct EMIF_CLA_struct EMIF_DMA;

float on_off_odd_a[25];
float on_off_odd_b[25];
float on_off_odd_c[25];

float on_off_even_a[25];
float on_off_even_b[25];
float on_off_even_c[25];
