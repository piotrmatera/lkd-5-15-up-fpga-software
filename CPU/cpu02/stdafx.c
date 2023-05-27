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
struct CLA1toCLA2_struct CLA1toCLA2;

struct Timer_PWM_struct Timer_PWM;

struct Measurements_master_struct Meas_master;
struct Measurements_master_gain_offset_struct Meas_master_gain;
struct Measurements_master_gain_offset_struct Meas_master_offset;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma SET_DATA_SECTION("CPUTOCLA")

struct EMIF_SD_struct EMIF_CLA;

#pragma SET_DATA_SECTION("CPU1TOCPU2")

struct CPU1toCPU2_struct CPU1toCPU2;

#pragma SET_DATA_SECTION("CPU2TOCPU1")

struct CPU2toCPU1_struct CPU2toCPU1;

#pragma SET_DATA_SECTION("CPU_shared")

#pragma SET_DATA_SECTION("EMIF_mem")

volatile union EMIF_union EMIF_mem;

#pragma SET_DATA_SECTION()
