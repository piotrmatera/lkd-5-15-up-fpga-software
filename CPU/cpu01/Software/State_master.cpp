/*
 * State.cpp
 *
 *  Created on: 2 maj 2020
 *      Author: MrTea
 */

#include <math.h>
#include <stdlib.h>     /* qsort */
#include <string.h>

#include "stdafx.h"

#include "State_background.h"
#include "State_master.h"

#include "SD_card.h"
#include "FLASH.h"

class Machine_master_class Machine_master;
void (*Machine_master_class::state_pointers[Machine_master_class::state_max])();
struct L_grid_meas_struct L_grid_meas;

class FLASH_class L_grid_FLASH =
{
 .address = {(Uint16 *)&L_grid_meas.L_grid_previous, 0},
 .sector = SectorL,
 .size16_each = {sizeof(L_grid_meas.L_grid_previous), 0},
};

static int compare_float (const void * a, const void * b)
{
    if ( *(float*)a <  *(float*)b ) return -1;
    else if ( *(float*)a >  *(float*)b ) return 1;
    else return 0;
}

void Machine_master_class::Main()
{
    register void (*pointer_temp)() = Machine_master.state_pointers[Machine_master.state];

    if(pointer_temp != NULL && Machine_master.state < sizeof(Machine_master_class::state_pointers)/sizeof(Machine_master_class::state_pointers[0]))
        (*pointer_temp)();
    else
        Machine_master.state = state_idle;
}

void Machine_master_class::idle()
{
    if(Machine_master.state_last != Machine_master.state)
    {
        Machine_master.state_last = Machine_master.state;
    }

    Conv.tangens_range_local_prefilter[0].a.out =
    Conv.tangens_range_local_prefilter[0].b.out =
    Conv.tangens_range_local_prefilter[0].c.out =
    Conv.tangens_range_local_prefilter[1].a.out =
    Conv.tangens_range_local_prefilter[1].b.out =
    Conv.tangens_range_local_prefilter[1].c.out =
    Conv.Q_set_local_prefilter.a.out =
    Conv.Q_set_local_prefilter.b.out =
    Conv.Q_set_local_prefilter.c.out =
    Conv.version_Q_comp_local_prefilter.a.out =
    Conv.version_Q_comp_local_prefilter.b.out =
    Conv.version_Q_comp_local_prefilter.c.out =
    Conv.enable_Q_comp_local_prefilter.a.out =
    Conv.enable_Q_comp_local_prefilter.b.out =
    Conv.enable_Q_comp_local_prefilter.c.out =
    Conv.version_P_sym_local_prefilter.out =
    Conv.enable_P_sym_local_prefilter.out =
    Conv.Q_set_local.a =
    Conv.Q_set_local.b =
    Conv.Q_set_local.c =
    Conv.enable_Q_comp_local.a =
    Conv.enable_Q_comp_local.b =
    Conv.enable_Q_comp_local.c =
    Conv.enable_P_sym_local =
    Conv.enable_H_comp_local = 0.0f;

    if(Conv.master.total.I_lim && !status_ACDC.master_rdy) Machine_master.state = state_start;
}

void Machine_master_class::start()
{
    static Uint32 delay_timer;

    if(Machine_master.state_last != Machine_master.state)
    {
        delay_timer = IpcRegs.IPCCOUNTERL;
        Machine_master.state_last = Machine_master.state;
    }

    if((float)(IpcRegs.IPCCOUNTERL - delay_timer) * (1.0f/200000000.0f) > 0.1f)
    {
        if(status_ACDC.CT_connection_a != 1 || status_ACDC.CT_connection_b != 2 || status_ACDC.CT_connection_c != 3)
            Machine_master.state = state_CT_test;
        else if(status_ACDC.L_grid_measured)
            Machine_master.state = state_operational;
        else
            Machine_master.state = state_Lgrid_meas;
    }

    if(!Conv.master.total.I_lim || status_ACDC.master_rdy) Machine_master.state = state_idle;
}

void Machine_master_class::CT_test()
{
    static struct CT_test_struct
    {
        struct abc_struct Iq_pos[3], Iq_neg[3], Iq_diff[3];
        struct abc_struct Id_pos[3], Id_neg[3], Id_diff[3];
        struct abc_struct CT_gain[3];
    }CT_test_startup;
    static float C_dc_meas[6];

    static Uint64 timer_old;
    static Uint16 CT_test_state;
    static Uint16 CT_test_state_last;
    static Uint16 repeat_counter;
    if(Machine_master.state_last != Machine_master.state)
    {
        Machine_master.state_last = Machine_master.state;

        timer_old = ReadIpcTimer();
        CT_test_state_last = 1;
        CT_test_state = 0;
        repeat_counter = 0;

        Conv.Q_set_local.a =
        Conv.Q_set_local.b =
        Conv.Q_set_local.c =
        Conv.enable_P_sym_local =
        Conv.enable_H_comp_local = 0.0f;
        Conv.enable_Q_comp_local.a =
        Conv.enable_Q_comp_local.b =
        Conv.enable_Q_comp_local.c =
        Conv.version_Q_comp_local.a =
        Conv.version_Q_comp_local.b =
        Conv.version_Q_comp_local.c = 1.0f;
    }
    Uint64 elapsed_time = ReadIpcTimer() - timer_old;

    switch(CT_test_state)
    {
    case 0:
    {
        if(CT_test_state_last != CT_test_state)
        {
            CT_test_state_last = CT_test_state;
            if(repeat_counter == 0) Conv.Q_set_local.a = Grid.U_grid_1h.a * 8.0f;
            else if(repeat_counter == 1) Conv.Q_set_local.b = Grid.U_grid_1h.b * 8.0f;
            else Conv.Q_set_local.c = Grid.U_grid_1h.c * 8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            CT_test_startup.Iq_pos[repeat_counter].a = Conv.iq_grid.a;
            CT_test_startup.Iq_pos[repeat_counter].b = Conv.iq_grid.b;
            CT_test_startup.Iq_pos[repeat_counter].c = Conv.iq_grid.c;
            CT_test_startup.Id_pos[repeat_counter].a = Conv.id_grid.a;
            CT_test_startup.Id_pos[repeat_counter].b = Conv.id_grid.b;
            CT_test_startup.Id_pos[repeat_counter].c = Conv.id_grid.c;
            C_dc_meas[repeat_counter] = Conv.C_dc_meas;
            CT_test_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 1:
    {
        if(CT_test_state_last != CT_test_state)
        {
            CT_test_state_last = CT_test_state;
            if(repeat_counter == 0) Conv.Q_set_local.a = Grid.U_grid_1h.a * -8.0f;
            else if(repeat_counter == 1) Conv.Q_set_local.b = Grid.U_grid_1h.b * -8.0f;
            else Conv.Q_set_local.c = Grid.U_grid_1h.c * -8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            CT_test_startup.Iq_neg[repeat_counter].a = Conv.iq_grid.a;
            CT_test_startup.Iq_neg[repeat_counter].b = Conv.iq_grid.b;
            CT_test_startup.Iq_neg[repeat_counter].c = Conv.iq_grid.c;
            CT_test_startup.Id_neg[repeat_counter].a = Conv.id_grid.a;
            CT_test_startup.Id_neg[repeat_counter].b = Conv.id_grid.b;
            CT_test_startup.Id_neg[repeat_counter].c = Conv.id_grid.c;
            C_dc_meas[repeat_counter+3] = Conv.C_dc_filter.out;
            Conv.Q_set_local.a =
            Conv.Q_set_local.b =
            Conv.Q_set_local.c = 0.0f;
            if(++repeat_counter < 3) CT_test_state = 0;
            else CT_test_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 2:
    {
        if(CT_test_state_last != CT_test_state)
        {
            CT_test_state_last = CT_test_state;

            qsort(C_dc_meas, 6, sizeof(float), compare_float);
            Conv.C_dc_measured = (C_dc_meas[2]+C_dc_meas[3]) * 0.5f;

            CT_test_startup.Iq_diff[0].a = fabsf(CT_test_startup.Iq_pos[0].a - CT_test_startup.Iq_neg[0].a);
            CT_test_startup.Iq_diff[0].b = fabsf(CT_test_startup.Iq_pos[0].b - CT_test_startup.Iq_neg[0].b);
            CT_test_startup.Iq_diff[0].c = fabsf(CT_test_startup.Iq_pos[0].c - CT_test_startup.Iq_neg[0].c);
            CT_test_startup.Iq_diff[1].a = fabsf(CT_test_startup.Iq_pos[1].a - CT_test_startup.Iq_neg[1].a);
            CT_test_startup.Iq_diff[1].b = fabsf(CT_test_startup.Iq_pos[1].b - CT_test_startup.Iq_neg[1].b);
            CT_test_startup.Iq_diff[1].c = fabsf(CT_test_startup.Iq_pos[1].c - CT_test_startup.Iq_neg[1].c);
            CT_test_startup.Iq_diff[2].a = fabsf(CT_test_startup.Iq_pos[2].a - CT_test_startup.Iq_neg[2].a);
            CT_test_startup.Iq_diff[2].b = fabsf(CT_test_startup.Iq_pos[2].b - CT_test_startup.Iq_neg[2].b);
            CT_test_startup.Iq_diff[2].c = fabsf(CT_test_startup.Iq_pos[2].c - CT_test_startup.Iq_neg[2].c);

            CT_test_startup.Id_diff[0].a = fabsf(CT_test_startup.Id_pos[0].a - CT_test_startup.Id_neg[0].a);
            CT_test_startup.Id_diff[0].b = fabsf(CT_test_startup.Id_pos[0].b - CT_test_startup.Id_neg[0].b);
            CT_test_startup.Id_diff[0].c = fabsf(CT_test_startup.Id_pos[0].c - CT_test_startup.Id_neg[0].c);
            CT_test_startup.Id_diff[1].a = fabsf(CT_test_startup.Id_pos[1].a - CT_test_startup.Id_neg[1].a);
            CT_test_startup.Id_diff[1].b = fabsf(CT_test_startup.Id_pos[1].b - CT_test_startup.Id_neg[1].b);
            CT_test_startup.Id_diff[1].c = fabsf(CT_test_startup.Id_pos[1].c - CT_test_startup.Id_neg[1].c);
            CT_test_startup.Id_diff[2].a = fabsf(CT_test_startup.Id_pos[2].a - CT_test_startup.Id_neg[2].a);
            CT_test_startup.Id_diff[2].b = fabsf(CT_test_startup.Id_pos[2].b - CT_test_startup.Id_neg[2].b);
            CT_test_startup.Id_diff[2].c = fabsf(CT_test_startup.Id_pos[2].c - CT_test_startup.Id_neg[2].c);

            CT_test_startup.CT_gain[0].a = 16.0f / CT_test_startup.Iq_diff[0].a;
            CT_test_startup.CT_gain[0].b = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[0].b + 0.5f * CT_test_startup.Iq_diff[0].b);
            CT_test_startup.CT_gain[0].c = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[0].c + 0.5f * CT_test_startup.Iq_diff[0].c);
            CT_test_startup.CT_gain[1].a = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[1].a + 0.5f * CT_test_startup.Iq_diff[1].a);
            CT_test_startup.CT_gain[1].b = 16.0f / CT_test_startup.Iq_diff[1].b;
            CT_test_startup.CT_gain[1].c = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[1].c + 0.5f * CT_test_startup.Iq_diff[1].c);
            CT_test_startup.CT_gain[2].a = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[2].a + 0.5f * CT_test_startup.Iq_diff[2].a);
            CT_test_startup.CT_gain[2].b = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[2].b + 0.5f * CT_test_startup.Iq_diff[2].b);
            CT_test_startup.CT_gain[2].c = 16.0f / CT_test_startup.Iq_diff[2].c;

            if(fabsf(CT_test_startup.CT_gain[0].a-1.0f) < 0.5f)
                status_ACDC.CT_connection_a = 1;
            else if(fabsf(CT_test_startup.CT_gain[1].a-1.0f) < 0.5f)
                status_ACDC.CT_connection_a = 2;
            else if(fabsf(CT_test_startup.CT_gain[2].a-1.0f) < 0.5f)
                status_ACDC.CT_connection_a = 3;
            else
                status_ACDC.CT_connection_a = 0;

            if(fabsf(CT_test_startup.CT_gain[0].b-1.0f) < 0.5f)
                status_ACDC.CT_connection_b = 1;
            else if(fabsf(CT_test_startup.CT_gain[1].b-1.0f) < 0.5f)
                status_ACDC.CT_connection_b = 2;
            else if(fabsf(CT_test_startup.CT_gain[2].b-1.0f) < 0.5f)
                status_ACDC.CT_connection_b = 3;
            else
                status_ACDC.CT_connection_b = 0;

            if(fabsf(CT_test_startup.CT_gain[0].c-1.0f) < 0.5f)
                status_ACDC.CT_connection_c = 1;
            else if(fabsf(CT_test_startup.CT_gain[1].c-1.0f) < 0.5f)
                status_ACDC.CT_connection_c = 2;
            else if(fabsf(CT_test_startup.CT_gain[2].c-1.0f) < 0.5f)
                status_ACDC.CT_connection_c = 3;
            else
                status_ACDC.CT_connection_c = 0;


            if(status_ACDC.CT_connection_a == 1) status_ACDC.no_CT_connected_a = 0;
            else status_ACDC.no_CT_connected_a = 1;
            if(status_ACDC.CT_connection_b == 2) status_ACDC.no_CT_connected_b = 0;
            else status_ACDC.no_CT_connected_b = 1;
            if(status_ACDC.CT_connection_c == 3) status_ACDC.no_CT_connected_c = 0;
            else status_ACDC.no_CT_connected_c = 1;
        }

        if(elapsed_time > 50000000ULL)
        {
            Conv.compensation2 = 4000.0f * Saturation(L_grid_meas.L_grid_previous[0], 50e-6, 800e-6) + 1.8f;
            if(status_ACDC.L_grid_measured || status_ACDC.CT_connection_a != 1 || status_ACDC.CT_connection_b != 2 || status_ACDC.CT_connection_c != 3)
                Machine_master.state = state_operational;
            else
                Machine_master.state = state_Lgrid_meas;
        }
        break;
    }
    }

    if(!Conv.master.total.I_lim || status_ACDC.master_rdy) Machine_master.state = state_idle;
}

void Machine_master_class::Lgrid_meas()
{
    static Uint64 timer_old;
    static Uint16 Lgrid_meas_state;
    static Uint16 Lgrid_meas_state_last;
    static Uint16 repeat_counter;
    if(Machine_master.state_last != Machine_master.state)
    {
        Machine_master.state_last = Machine_master.state;

        timer_old = ReadIpcTimer();
        Lgrid_meas_state_last = 1;
        Lgrid_meas_state = 0;
        repeat_counter = 0;

        Conv.enable_P_sym_local =
        Conv.enable_H_comp_local = 0.0f;
        Conv.enable_Q_comp_local.a =
        Conv.enable_Q_comp_local.b =
        Conv.enable_Q_comp_local.c =
        Conv.version_Q_comp_local.a =
        Conv.version_Q_comp_local.b =
        Conv.version_Q_comp_local.c = 1.0f;
    }
    Uint64 elapsed_time = ReadIpcTimer() - timer_old;

    switch(Lgrid_meas_state)
    {
    case 0:
    {
        if(Lgrid_meas_state_last != Lgrid_meas_state)
        {
            Lgrid_meas_state_last = Lgrid_meas_state;
            Conv.Q_set_local.a = Grid.U_grid_1h.a * 8.0f;
            Conv.Q_set_local.b = Grid.U_grid_1h.b * 8.0f;
            Conv.Q_set_local.c = Grid.U_grid_1h.c * 8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            L_grid_meas.Iq_pos[repeat_counter].a = Conv.iq_grid.a;
            L_grid_meas.Iq_pos[repeat_counter].b = Conv.iq_grid.b;
            L_grid_meas.Iq_pos[repeat_counter].c = Conv.iq_grid.c;
            L_grid_meas.U_pos[repeat_counter].a = Grid.U_grid.a;
            L_grid_meas.U_pos[repeat_counter].b = Grid.U_grid.b;
            L_grid_meas.U_pos[repeat_counter].c = Grid.U_grid.c;
            Lgrid_meas_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 1:
    {
        if(Lgrid_meas_state_last != Lgrid_meas_state)
        {
            Lgrid_meas_state_last = Lgrid_meas_state;
            Conv.Q_set_local.a = Grid.U_grid_1h.a * -8.0f;
            Conv.Q_set_local.b = Grid.U_grid_1h.b * -8.0f;
            Conv.Q_set_local.c = Grid.U_grid_1h.c * -8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            L_grid_meas.Iq_neg[repeat_counter].a = Conv.iq_grid.a;
            L_grid_meas.Iq_neg[repeat_counter].b = Conv.iq_grid.b;
            L_grid_meas.Iq_neg[repeat_counter].c = Conv.iq_grid.c;
            L_grid_meas.U_neg[repeat_counter].a = Grid.U_grid.a;
            L_grid_meas.U_neg[repeat_counter].b = Grid.U_grid.b;
            L_grid_meas.U_neg[repeat_counter].c = Grid.U_grid.c;
            if(++repeat_counter < 5) Lgrid_meas_state = 0;
            else Lgrid_meas_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 2:
    {
        Conv.Q_set_local.a =
        Conv.Q_set_local.b =
        Conv.Q_set_local.c = 0.0f;
        L_grid_meas.Iq_diff_a[0] = L_grid_meas.Iq_pos[0].a - L_grid_meas.Iq_neg[0].a;
        L_grid_meas.Iq_diff_b[0] = L_grid_meas.Iq_pos[0].b - L_grid_meas.Iq_neg[0].b;
        L_grid_meas.Iq_diff_c[0] = L_grid_meas.Iq_pos[0].c - L_grid_meas.Iq_neg[0].c;
        L_grid_meas.Iq_diff_a[1] = L_grid_meas.Iq_pos[1].a - L_grid_meas.Iq_neg[1].a;
        L_grid_meas.Iq_diff_b[1] = L_grid_meas.Iq_pos[1].b - L_grid_meas.Iq_neg[1].b;
        L_grid_meas.Iq_diff_c[1] = L_grid_meas.Iq_pos[1].c - L_grid_meas.Iq_neg[1].c;
        L_grid_meas.Iq_diff_a[2] = L_grid_meas.Iq_pos[2].a - L_grid_meas.Iq_neg[2].a;
        L_grid_meas.Iq_diff_b[2] = L_grid_meas.Iq_pos[2].b - L_grid_meas.Iq_neg[2].b;
        L_grid_meas.Iq_diff_c[2] = L_grid_meas.Iq_pos[2].c - L_grid_meas.Iq_neg[2].c;
        L_grid_meas.Iq_diff_a[3] = L_grid_meas.Iq_pos[3].a - L_grid_meas.Iq_neg[3].a;
        L_grid_meas.Iq_diff_b[3] = L_grid_meas.Iq_pos[3].b - L_grid_meas.Iq_neg[3].b;
        L_grid_meas.Iq_diff_c[3] = L_grid_meas.Iq_pos[3].c - L_grid_meas.Iq_neg[3].c;
        L_grid_meas.Iq_diff_a[4] = L_grid_meas.Iq_pos[4].a - L_grid_meas.Iq_neg[4].a;
        L_grid_meas.Iq_diff_b[4] = L_grid_meas.Iq_pos[4].b - L_grid_meas.Iq_neg[4].b;
        L_grid_meas.Iq_diff_c[4] = L_grid_meas.Iq_pos[4].c - L_grid_meas.Iq_neg[4].c;
        L_grid_meas.L_grid[0].a = (L_grid_meas.U_pos[0].a - L_grid_meas.U_neg[0].a) / (L_grid_meas.Iq_diff_a[0] * Conv.w_filter);
        L_grid_meas.L_grid[0].b = (L_grid_meas.U_pos[0].b - L_grid_meas.U_neg[0].b) / (L_grid_meas.Iq_diff_b[0] * Conv.w_filter);
        L_grid_meas.L_grid[0].c = (L_grid_meas.U_pos[0].c - L_grid_meas.U_neg[0].c) / (L_grid_meas.Iq_diff_c[0] * Conv.w_filter);
        L_grid_meas.L_grid[1].a = (L_grid_meas.U_pos[1].a - L_grid_meas.U_neg[1].a) / (L_grid_meas.Iq_diff_a[1] * Conv.w_filter);
        L_grid_meas.L_grid[1].b = (L_grid_meas.U_pos[1].b - L_grid_meas.U_neg[1].b) / (L_grid_meas.Iq_diff_b[1] * Conv.w_filter);
        L_grid_meas.L_grid[1].c = (L_grid_meas.U_pos[1].c - L_grid_meas.U_neg[1].c) / (L_grid_meas.Iq_diff_c[1] * Conv.w_filter);
        L_grid_meas.L_grid[2].a = (L_grid_meas.U_pos[2].a - L_grid_meas.U_neg[2].a) / (L_grid_meas.Iq_diff_a[2] * Conv.w_filter);
        L_grid_meas.L_grid[2].b = (L_grid_meas.U_pos[2].b - L_grid_meas.U_neg[2].b) / (L_grid_meas.Iq_diff_b[2] * Conv.w_filter);
        L_grid_meas.L_grid[2].c = (L_grid_meas.U_pos[2].c - L_grid_meas.U_neg[2].c) / (L_grid_meas.Iq_diff_c[2] * Conv.w_filter);
        L_grid_meas.L_grid[3].a = (L_grid_meas.U_pos[3].a - L_grid_meas.U_neg[3].a) / (L_grid_meas.Iq_diff_a[3] * Conv.w_filter);
        L_grid_meas.L_grid[3].b = (L_grid_meas.U_pos[3].b - L_grid_meas.U_neg[3].b) / (L_grid_meas.Iq_diff_b[3] * Conv.w_filter);
        L_grid_meas.L_grid[3].c = (L_grid_meas.U_pos[3].c - L_grid_meas.U_neg[3].c) / (L_grid_meas.Iq_diff_c[3] * Conv.w_filter);
        L_grid_meas.L_grid[4].a = (L_grid_meas.U_pos[4].a - L_grid_meas.U_neg[4].a) / (L_grid_meas.Iq_diff_a[4] * Conv.w_filter);
        L_grid_meas.L_grid[4].b = (L_grid_meas.U_pos[4].b - L_grid_meas.U_neg[4].b) / (L_grid_meas.Iq_diff_b[4] * Conv.w_filter);
        L_grid_meas.L_grid[4].c = (L_grid_meas.U_pos[4].c - L_grid_meas.U_neg[4].c) / (L_grid_meas.Iq_diff_c[4] * Conv.w_filter);
        memcpy(L_grid_meas.L_grid_sorted, L_grid_meas.L_grid, sizeof(L_grid_meas.L_grid_sorted));
        qsort(L_grid_meas.L_grid_sorted, 15, sizeof(float), compare_float);
        L_grid_meas.L_grid_new = fabsf(L_grid_meas.L_grid_sorted[7]);
        Conv.compensation2 = 2.0f * 4000.0f * Saturation(L_grid_meas.L_grid_new, 50e-6, 800e-6) + 1.8f;

        qsort(L_grid_meas.Iq_diff_a, 5, sizeof(float), compare_float);
        qsort(L_grid_meas.Iq_diff_b, 5, sizeof(float), compare_float);
        qsort(L_grid_meas.Iq_diff_c, 5, sizeof(float), compare_float);
        L_grid_meas.CT_gain.a = 16.0f / L_grid_meas.Iq_diff_a[2];
        L_grid_meas.CT_gain.b = 16.0f / L_grid_meas.Iq_diff_b[2];
        L_grid_meas.CT_gain.c = 16.0f / L_grid_meas.Iq_diff_c[2];
        L_grid_meas.CT_gain_rounded.a = (float)__f32toi16r(L_grid_meas.CT_gain.a * 1.0f) * 1.0f;
        L_grid_meas.CT_gain_rounded.b = (float)__f32toi16r(L_grid_meas.CT_gain.b * 1.0f) * 1.0f;
        L_grid_meas.CT_gain_rounded.c = (float)__f32toi16r(L_grid_meas.CT_gain.c * 1.0f) * 1.0f;

        Uint16 save = 0;
        if(L_grid_meas.CT_gain_rounded.a < 0.0f) CT_char_vars.calibration.Meas_ACDC_gain.I_grid.a *= -1.0f, save = 1;
        if(L_grid_meas.CT_gain_rounded.b < 0.0f) CT_char_vars.calibration.Meas_ACDC_gain.I_grid.b *= -1.0f, save = 1;
        if(L_grid_meas.CT_gain_rounded.c < 0.0f) CT_char_vars.calibration.Meas_ACDC_gain.I_grid.c *= -1.0f, save = 1;

        if(save)
        {
            memcpy(&SD_card.calibration.Meas_ACDC_gain, &CT_char_vars.calibration.Meas_ACDC_gain, sizeof(SD_card.calibration.Meas_ACDC_gain));
            SD_card.save_calibration_data();
        }

        L_grid_meas.L_grid_previous[9] = L_grid_meas.L_grid_previous[8];
        L_grid_meas.L_grid_previous[8] = L_grid_meas.L_grid_previous[7];
        L_grid_meas.L_grid_previous[7] = L_grid_meas.L_grid_previous[6];
        L_grid_meas.L_grid_previous[6] = L_grid_meas.L_grid_previous[5];
        L_grid_meas.L_grid_previous[5] = L_grid_meas.L_grid_previous[4];
        L_grid_meas.L_grid_previous[4] = L_grid_meas.L_grid_previous[3];
        L_grid_meas.L_grid_previous[3] = L_grid_meas.L_grid_previous[2];
        L_grid_meas.L_grid_previous[2] = L_grid_meas.L_grid_previous[1];
        L_grid_meas.L_grid_previous[1] = L_grid_meas.L_grid_previous[0];
        L_grid_meas.L_grid_previous[0] = L_grid_meas.L_grid_new;
        L_grid_FLASH.save();

        Lgrid_meas_state++;
        break;
    }

    case 3:
    {
        if(elapsed_time > 75000000ULL)
        {
            Machine_master.state = state_operational;
            status_ACDC.L_grid_measured = 1;
        }
        break;
    }
    }

    if(!Conv.master.total.I_lim || status_ACDC.master_rdy) Machine_master.state = state_idle;
}

void Machine_master_class::operational()
{
    static struct test_CT_struct
    {
        Uint16 state, state_last;
        float I_grid_val;
        float test_delay_timer_Kahan[4];
        float test_delay_timer[4];
        float test_delay_timer_compare;
        float test_request_integrator[7];
        float test_request_filtered[7];
        float test_request_period_counter;
        float test_request_period;
        struct abc_struct Q_grid_last;
        struct abc_struct Q_conv_step;
        struct abc_struct tested_current;
        struct abc_struct I_grid_filter_last;
        union
        {
            Uint16 all;
            struct
            {
                Uint16 I_grid_back_a : 1;
                Uint16 I_grid_back_b : 1;
                Uint16 I_grid_back_c : 1;
                Uint16 I_grid_under_val_a : 1;
                Uint16 I_grid_under_val_b : 1;
                Uint16 I_grid_under_val_c : 1;
                Uint16 In_limit : 1;
            }bit;
        }test_request_flags;
    }CT_test_online;

    Uint64 timer_new = ReadIpcTimer();
    Uint32 timer_new32 = timer_new;
    static Uint32 timer_last32;

    if(Machine_master.state_last != Machine_master.state)
    {
        Machine_master.state_last = Machine_master.state;

        timer_last32 = timer_new32;

        memset(&CT_test_online, 0, sizeof(CT_test_online));
        CT_test_online.state_last = 1;
        CT_test_online.I_grid_val = 1.0f;
        CT_test_online.test_request_period = 2.0f;
        CT_test_online.test_delay_timer_compare = 900.0f;
        CT_test_online.test_delay_timer[0] =
        CT_test_online.test_delay_timer[1] =
        CT_test_online.test_delay_timer[2] =
        CT_test_online.test_delay_timer[3] = CT_test_online.test_delay_timer_compare;
    }

    switch (CT_test_online.state)
    {
    case 0:
    {
        if (CT_test_online.state != CT_test_online.state_last)
        {
            CT_test_online.state_last = CT_test_online.state;
            CT_test_online.test_request_flags.all = 0;
        }

        float time_delay = (float)(timer_new32 - timer_last32) * (1.0f/200000000.0f);
        timer_last32 = timer_new32;
        CT_test_online.test_request_period_counter += time_delay;

        float timer_last;
        float y;
        CT_test_online.test_delay_timer[0] = (timer_last = CT_test_online.test_delay_timer[0]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[0]);
        CT_test_online.test_delay_timer_Kahan[0] = (CT_test_online.test_delay_timer[0] - timer_last) - y;

        CT_test_online.test_delay_timer[1] = (timer_last = CT_test_online.test_delay_timer[1]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[1]);
        CT_test_online.test_delay_timer_Kahan[1] = (CT_test_online.test_delay_timer[1] - timer_last) - y;

        CT_test_online.test_delay_timer[2] = (timer_last = CT_test_online.test_delay_timer[2]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[2]);
        CT_test_online.test_delay_timer_Kahan[2] = (CT_test_online.test_delay_timer[2] - timer_last) - y;

        CT_test_online.test_delay_timer[3] = (timer_last = CT_test_online.test_delay_timer[3]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[3]);
        CT_test_online.test_delay_timer_Kahan[3] = (CT_test_online.test_delay_timer[3] - timer_last) - y;

        if (Grid_filter.I_grid_1h.a < CT_test_online.I_grid_val && CT_test_online.test_delay_timer[0] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[0] += time_delay;
        if (Grid_filter.I_grid_1h.b < CT_test_online.I_grid_val && CT_test_online.test_delay_timer[1] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[1] += time_delay;
        if (Grid_filter.I_grid_1h.c < CT_test_online.I_grid_val && CT_test_online.test_delay_timer[2] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[2] += time_delay;

        Uint16 in_limit = status_ACDC.in_limit_Q && (Conv.version_Q_comp_local.a + Conv.version_Q_comp_local.b + Conv.version_Q_comp_local.c < 3.0f);
        in_limit = in_limit || status_ACDC.in_limit_H || status_ACDC.in_limit_P;
        Uint16 test_val =
                Grid_filter.I_grid_1h.a < CT_test_online.I_grid_val &&
                Grid_filter.I_grid_1h.b < CT_test_online.I_grid_val &&
                Grid_filter.I_grid_1h.c < CT_test_online.I_grid_val;

        if (test_val && in_limit && CT_test_online.test_delay_timer[3] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[3] += time_delay;

        if (Grid_filter.I_grid_1h.a > CT_test_online.I_grid_val && status_ACDC.no_CT_connected_a)
            CT_test_online.test_request_integrator[4] += time_delay;
        if (Grid_filter.I_grid_1h.b > CT_test_online.I_grid_val && status_ACDC.no_CT_connected_b)
            CT_test_online.test_request_integrator[5] += time_delay;
        if (Grid_filter.I_grid_1h.c > CT_test_online.I_grid_val && status_ACDC.no_CT_connected_c)
            CT_test_online.test_request_integrator[6] += time_delay;

        if(CT_test_online.test_request_period_counter > CT_test_online.test_request_period)
        {
            CT_test_online.test_request_filtered[0] = CT_test_online.test_request_integrator[0] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[1] = CT_test_online.test_request_integrator[1] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[2] = CT_test_online.test_request_integrator[2] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[3] = CT_test_online.test_request_integrator[3] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[4] = CT_test_online.test_request_integrator[4] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[5] = CT_test_online.test_request_integrator[5] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[6] = CT_test_online.test_request_integrator[6] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_integrator[0] =
            CT_test_online.test_request_integrator[1] =
            CT_test_online.test_request_integrator[2] =
            CT_test_online.test_request_integrator[3] =
            CT_test_online.test_request_integrator[4] =
            CT_test_online.test_request_integrator[5] =
            CT_test_online.test_request_integrator[6] =
            CT_test_online.test_request_period_counter = 0.0f;
        }

        if (CT_test_online.test_request_filtered[4] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_back_a = 1;
        else if (CT_test_online.test_request_filtered[5] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_back_b = 1;
        else if (CT_test_online.test_request_filtered[6] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_back_c = 1;
        else if (CT_test_online.test_request_filtered[0] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_under_val_a = 1;
        else if (CT_test_online.test_request_filtered[1] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_under_val_b = 1;
        else if (CT_test_online.test_request_filtered[2] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_under_val_c = 1;
        else if (CT_test_online.test_request_filtered[3] > 0.9f)
            CT_test_online.test_request_flags.bit.In_limit = 1;

        if(CT_test_online.test_request_flags.all)
            CT_test_online.state = 1;

        ////////////////////////////////////////////////////////////////////

        Conv.tangens_range_local[0].a = Saturation(control_ACDC.tangens_range[0].a, -1.0f, 1.0f);
        Conv.tangens_range_local[0].b = Saturation(control_ACDC.tangens_range[0].b, -1.0f, 1.0f);
        Conv.tangens_range_local[0].c = Saturation(control_ACDC.tangens_range[0].c, -1.0f, 1.0f);
        Conv.tangens_range_local[1].a = Saturation(control_ACDC.tangens_range[1].a, -1.0f, 1.0f);
        Conv.tangens_range_local[1].b = Saturation(control_ACDC.tangens_range[1].b, -1.0f, 1.0f);
        Conv.tangens_range_local[1].c = Saturation(control_ACDC.tangens_range[1].c, -1.0f, 1.0f);

        Conv.version_P_sym_local = control_ACDC.flags.bit.version_P_sym;
        Conv.enable_Q_comp_local.a = control_ACDC.flags.bit.enable_Q_comp_a;
        Conv.enable_Q_comp_local.b = control_ACDC.flags.bit.enable_Q_comp_b;
        Conv.enable_Q_comp_local.c = control_ACDC.flags.bit.enable_Q_comp_c;

        if(status_ACDC.no_CT_connected_a)
        {
            Conv.Q_set_local.a = control_ACDC.Q_set.a + Grid.U_grid_1h.a * Grid.U_grid_1h.a * Conv.w_filter * Conv.C_conv;
            Conv.version_Q_comp_local.a = 1.0f;
        }
        else
        {
            Conv.Q_set_local.a = control_ACDC.Q_set.a;
            Conv.version_Q_comp_local.a = control_ACDC.flags.bit.version_Q_comp_a;
        }

        if(status_ACDC.no_CT_connected_b)
        {
            Conv.Q_set_local.b = control_ACDC.Q_set.b + Grid.U_grid_1h.b * Grid.U_grid_1h.b * Conv.w_filter * Conv.C_conv;
            Conv.version_Q_comp_local.b = 1.0f;
        }
        else
        {
            Conv.Q_set_local.b = control_ACDC.Q_set.b;
            Conv.version_Q_comp_local.b = control_ACDC.flags.bit.version_Q_comp_b;
        }

        if(status_ACDC.no_CT_connected_c)
        {
            Conv.Q_set_local.c = control_ACDC.Q_set.c + Grid.U_grid_1h.c * Grid.U_grid_1h.c * Conv.w_filter * Conv.C_conv;
            Conv.version_Q_comp_local.c = 1.0f;
        }
        else
        {
            Conv.Q_set_local.c = control_ACDC.Q_set.c;
            Conv.version_Q_comp_local.c = control_ACDC.flags.bit.version_Q_comp_c;
        }

        if (status_ACDC.no_CT_connected_a || status_ACDC.no_CT_connected_b || status_ACDC.no_CT_connected_c)
        {
            Conv.enable_P_sym_local = 0.0f;
            Conv.enable_H_comp_local = 0.0f;
        }
        else
        {
            Conv.enable_P_sym_local = control_ACDC.flags.bit.enable_P_sym;
            Conv.enable_H_comp_local = control_ACDC.flags.bit.enable_H_comp;
        }

        break;
    }
    case 1:
    {
        static Uint32 timer_last32;
        static float step;

        if (CT_test_online.state != CT_test_online.state_last)
        {
            CT_test_online.state_last = CT_test_online.state;

            timer_last32 = timer_new32;

            CT_test_online.I_grid_filter_last.a = Grid_filter.I_grid_1h.a;
            CT_test_online.I_grid_filter_last.b = Grid_filter.I_grid_1h.b;
            CT_test_online.I_grid_filter_last.c = Grid_filter.I_grid_1h.c;

            CT_test_online.Q_grid_last.a = Grid.Q_grid_1h.a;
            CT_test_online.Q_grid_last.b = Grid.Q_grid_1h.b;
            CT_test_online.Q_grid_last.c = Grid.Q_grid_1h.c;

            step = fminf(2.0f * CT_test_online.I_grid_val, Conv.I_lim);
            CT_test_online.Q_conv_step.a = Grid.U_grid_1h.a * step;
            CT_test_online.Q_conv_step.b = Grid.U_grid_1h.b * step;
            CT_test_online.Q_conv_step.c = Grid.U_grid_1h.c * step;
            if (Grid.Q_conv_1h.a < 0.0f) CT_test_online.Q_conv_step.a = -CT_test_online.Q_conv_step.a;
            if (Grid.Q_conv_1h.b < 0.0f) CT_test_online.Q_conv_step.b = -CT_test_online.Q_conv_step.b;
            if (Grid.Q_conv_1h.c < 0.0f) CT_test_online.Q_conv_step.c = -CT_test_online.Q_conv_step.c;

            Conv.Q_set_local.a = CT_test_online.Q_conv_step.a - Grid.Q_conv_1h.a;
            Conv.Q_set_local.b = CT_test_online.Q_conv_step.b - Grid.Q_conv_1h.b;
            Conv.Q_set_local.c = CT_test_online.Q_conv_step.c - Grid.Q_conv_1h.c;
            Conv.enable_Q_comp_local.a =
            Conv.enable_Q_comp_local.b =
            Conv.enable_Q_comp_local.c =
            Conv.version_Q_comp_local.a =
            Conv.version_Q_comp_local.b =
            Conv.version_Q_comp_local.c = 1.0f;
        }

        if (timer_new32 - timer_last32 > 30000000UL)
        {
            CT_test_online.tested_current.a = fabsf(Grid.Q_grid_1h.a - CT_test_online.Q_grid_last.a - CT_test_online.Q_conv_step.a) / Grid.U_grid_1h.a;
            CT_test_online.tested_current.b = fabsf(Grid.Q_grid_1h.b - CT_test_online.Q_grid_last.b - CT_test_online.Q_conv_step.b) / Grid.U_grid_1h.b;
            CT_test_online.tested_current.c = fabsf(Grid.Q_grid_1h.c - CT_test_online.Q_grid_last.c - CT_test_online.Q_conv_step.c) / Grid.U_grid_1h.c;

            Uint16 test_result[3];
            test_result[0] = CT_test_online.tested_current.a > step * 0.5f;
            test_result[1] = CT_test_online.tested_current.b > step * 0.5f;
            test_result[2] = CT_test_online.tested_current.c > step * 0.5f;

            CT_test_online.test_request_filtered[0] =
            CT_test_online.test_request_filtered[1] =
            CT_test_online.test_request_filtered[2] =
            CT_test_online.test_request_filtered[3] =
            CT_test_online.test_request_filtered[4] =
            CT_test_online.test_request_filtered[5] =
            CT_test_online.test_request_filtered[6] =
            CT_test_online.test_request_period_counter = 0.0f;

            if (CT_test_online.test_request_flags.bit.I_grid_back_a | CT_test_online.test_request_flags.bit.I_grid_back_b | CT_test_online.test_request_flags.bit.I_grid_back_c)
            {
                if(CT_test_online.I_grid_filter_last.a < CT_test_online.I_grid_val)
                {
                    status_ACDC.no_CT_connected_a = test_result[0];
                    CT_test_online.test_delay_timer[0] = 0.0f;
                }
                else
                {
                    status_ACDC.no_CT_connected_a = 0;
                    CT_test_online.test_delay_timer[0] = CT_test_online.test_delay_timer_compare;
                }

                if(CT_test_online.I_grid_filter_last.b < CT_test_online.I_grid_val)
                {
                    status_ACDC.no_CT_connected_b = test_result[1];
                    CT_test_online.test_delay_timer[1] = 0.0f;
                }
                else
                {
                    status_ACDC.no_CT_connected_b = 0;
                    CT_test_online.test_delay_timer[1] = CT_test_online.test_delay_timer_compare;
                }

                if(CT_test_online.I_grid_filter_last.c < CT_test_online.I_grid_val)
                {
                    status_ACDC.no_CT_connected_c = test_result[2];
                    CT_test_online.test_delay_timer[2] = 0.0f;
                }
                else
                {
                    status_ACDC.no_CT_connected_c = 0;
                    CT_test_online.test_delay_timer[2] = CT_test_online.test_delay_timer_compare;
                }

                CT_test_online.test_delay_timer[3] = CT_test_online.test_delay_timer_compare;
            }
            else if (CT_test_online.test_request_flags.bit.I_grid_under_val_a || CT_test_online.test_request_flags.bit.I_grid_under_val_b || CT_test_online.test_request_flags.bit.I_grid_under_val_c)
            {
                if(CT_test_online.I_grid_filter_last.a < CT_test_online.I_grid_val || CT_test_online.test_request_flags.bit.I_grid_under_val_a)
                {
                    status_ACDC.no_CT_connected_a = test_result[0];
                    CT_test_online.test_delay_timer[0] = 0.0f;
                }

                if(CT_test_online.I_grid_filter_last.b < CT_test_online.I_grid_val || CT_test_online.test_request_flags.bit.I_grid_under_val_b)
                {
                    status_ACDC.no_CT_connected_b = test_result[1];
                    CT_test_online.test_delay_timer[1] = 0.0f;
                }

                if(CT_test_online.I_grid_filter_last.c < CT_test_online.I_grid_val || CT_test_online.test_request_flags.bit.I_grid_under_val_c)
                {
                    status_ACDC.no_CT_connected_c = test_result[2];
                    CT_test_online.test_delay_timer[2] = 0.0f;
                }
            }
            else if (CT_test_online.test_request_flags.bit.In_limit)
            {
                if(test_result[0])
                    status_ACDC.no_CT_connected_a = 1;
                if(test_result[1])
                    status_ACDC.no_CT_connected_b = 1;
                if(test_result[2])
                    status_ACDC.no_CT_connected_c = 1;

                if(test_result[0] + test_result[1] + test_result[2])
                    CT_test_online.test_delay_timer[3] = CT_test_online.test_delay_timer_compare;
                else
                    CT_test_online.test_delay_timer[3] = 0.0f;
            }

            CT_test_online.state = 0;
        }
        break;
    }
    }

    if(!Conv.master.total.I_lim || status_ACDC.master_rdy) Machine_master.state = state_idle;
}
