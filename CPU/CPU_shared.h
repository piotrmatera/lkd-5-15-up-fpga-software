/*
 * CPU_shared.h
 *
 *  Created on: 16 sie 2020
 *      Author: MrTea
 */

#ifndef CPU_SHARED_H_
#define CPU_SHARED_H_

#include "IO.h"
#include "Node_shared.h"

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

struct CLA1toCLA2_struct
{
    float I_lim;
    float w_filter;
    struct Measurements_master_struct Meas_master;
    struct Measurements_slave_struct Meas_slave;
};

struct CLA2toCLA1_struct
{
    struct
    {
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
    }Energy_meter_input;
    struct Grid_parameters_struct Grid;
    struct Grid_parameters_struct Grid_filter;
};

struct CPU1toCPU2_struct
{
    float CT_phase[3];
    float CT_ratio[3];
    struct CLA1toCLA2_struct CLA1toCLA2;
};

struct CPU2toCPU1_struct
{
    struct CLA2toCLA1_struct CLA2toCLA1;
};

#endif /* CPU_SHARED_H_ */
