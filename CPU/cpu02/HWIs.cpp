/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <math.h>

#include "stdafx.h"
#include "HWIs.h"

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void IPC3_INT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

    register Uint32 *src = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.Meas_master.U_grid_avg;
    register Uint32 *dest = (Uint32 *)&Meas_master.U_grid_avg;

    Cla1ForceTask1();

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    src = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.Meas_slave.I_conv_avg;
    dest = (Uint32 *)&Meas_slave.I_conv_avg;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    src = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.I_lim;
    dest = (Uint32 *)&CLA1toCLA2.I_lim;
    *dest++ = *src++;
    *dest++ = *src++;

    Timer_PWM.CPU_MEAS = TIMESTAMP_PWM;

    Grid.parameters.THD_U_grid.a = Kalman_U_grid[0].THD_total;
    Grid.parameters.THD_U_grid.b = Kalman_U_grid[1].THD_total;
    Grid.parameters.THD_U_grid.c = Kalman_U_grid[2].THD_total;
    Grid.parameters.THD_I_grid.a = Kalman_I_grid[0].THD_total;
    Grid.parameters.THD_I_grid.b = Kalman_I_grid[1].THD_total;
    Grid.parameters.THD_I_grid.c = Kalman_I_grid[2].THD_total;

    Kalman_calc_CPUasm(&Kalman_U_grid[0], sincos_kalman_table, Meas_master.U_grid_avg.a);
    Kalman_THD_calc_CPUasm(&Kalman_U_grid[0]);
    Kalman_calc_CPUasm(&Kalman_U_grid[1], sincos_kalman_table, Meas_master.U_grid_avg.b);
    Kalman_THD_calc_CPUasm(&Kalman_U_grid[1]);
    Kalman_calc_CPUasm(&Kalman_U_grid[2], sincos_kalman_table, Meas_master.U_grid_avg.c);
    Kalman_THD_calc_CPUasm(&Kalman_U_grid[2]);
    Timer_PWM.CPU_KALMAN_U = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    Fast_copy21_CPUasm();

    Timer_PWM.CPU_GRID = TIMESTAMP_PWM;

    Kalman_calc_CPUasm(&Kalman_I_grid[0], sincos_kalman_table, Meas_master.I_grid_avg.a);
    Kalman_THD_calc_CPUasm(&Kalman_I_grid[0]);
    Kalman_calc_CPUasm(&Kalman_I_grid[1], sincos_kalman_table, Meas_master.I_grid_avg.b);
    Kalman_THD_calc_CPUasm(&Kalman_I_grid[1]);
    Kalman_calc_CPUasm(&Kalman_I_grid[2], sincos_kalman_table, Meas_master.I_grid_avg.c);
    Kalman_THD_calc_CPUasm(&Kalman_I_grid[2]);

    Timer_PWM.CPU_SD_end = TIMESTAMP_PWM;


    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}
