/*
 * stdafx.c
 *
 *  Created on: 13 paz 2019
 *      Author: Mr.Tea
 */

#include "stdafx.h"

#pragma SET_DATA_SECTION("CLAData")

struct Converter_struct Conv;
struct Grid_parameters_struct Grid;
struct Grid_parameters_struct Grid_filter;
struct Thermistor_struct Therm;
struct Thermistor_struct Therm_module;

union CONTROL_EXT_MODBUS control_ext_modbus;

struct CONTROL_ACDC control_ACDC;
struct STATUS_ACDC status_ACDC;
struct STATUS_ACDC status_ACDC_master;
union ALARM_ACDC alarm_ACDC;
union ALARM_ACDC alarm_ACDC_snapshot;

struct Timer_PWM_struct Timer_PWM;

struct CIC2_struct CIC2_calibration;
CLA_FPTR CIC2_calibration_input;
struct CIC1_adaptive_global_struct CIC1_adaptive_global__50Hz;
struct CIC1_adaptive2_global_struct CIC1_adaptive2_global__50Hz;
struct CIC1_global_struct CIC1_global__50Hz;

struct Measurements_ACDC_struct Meas_ACDC;
struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain_error;
struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset_error;
struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain;
struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset;
struct Measurements_ACDC_alarm_struct Meas_ACDC_alarm_H;
struct Measurements_ACDC_alarm_struct Meas_ACDC_alarm_L;

struct trigonometric_struct sincos_table[SINCOS_HARMONICS];
struct trigonometric_struct sincos_table_comp[SINCOS_HARMONICS];
struct trigonometric_struct sincos_table_comp2[SINCOS_HARMONICS];
struct trigonometric_struct sincos_table_Kalman[SINCOS_HARMONICS];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma SET_DATA_SECTION("CPUTOCLA")

struct EMIF_SD_struct EMIF_CLA;

#pragma SET_DATA_SECTION("CPU1TOCPU2")

struct CPU1toCPU2_struct CPU1toCPU2;

#pragma SET_DATA_SECTION("CPU2TOCPU1")

struct CPU2toCPU1_struct CPU2toCPU1;

#pragma SET_DATA_SECTION("EMIF_mem")

volatile union EMIF_union EMIF_mem;

#pragma SET_DATA_SECTION("Grid")

struct Grid_analyzer_struct Grid_params;
struct Grid_analyzer_filter_struct Grid_filter_params;
struct Energy_meter_params_struct Energy_meter_params;

#pragma SET_DATA_SECTION()

struct Kalman_struct Kalman_I_grid[3];
struct Kalman_struct Kalman_U_grid[3];

struct Energy_meter_struct Energy_meter;

float on_off_odd_a[25];
float on_off_odd_b[25];
float on_off_odd_c[25];

float on_off_even_a[25];
float on_off_even_b[25];
float on_off_even_c[25];
