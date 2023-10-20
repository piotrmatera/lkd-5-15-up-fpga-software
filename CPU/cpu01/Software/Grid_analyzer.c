// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include <math.h>

#include "stdafx.h"

#pragma CODE_SECTION(CIC1_adaptive2_global_calc_CPU, ".interrupt_code");
void CIC1_adaptive2_global_calc_CPU(struct CIC1_adaptive2_global_struct* CIC_global, float frequency)
{
    CIC_global->read_enable =
    CIC_global->write_enable = 0;

    if (CIC_global->inc_delay)
    {
        CIC_global->inc_delay = 0;
        if (++CIC_global->read_ptr >= CIC_upsample2) CIC_global->read_ptr = 0;
    }
    if (CIC_global->dec_delay)
    {
        CIC_global->dec_delay = 0;
        if (--CIC_global->read_ptr < 0) CIC_global->read_ptr = CIC_upsample2 - 1;
    }

    if (CIC_global->counter_WDIST == CIC_global->counter_WDIST_read_val)
    {
        if (++CIC_global->read_ptr >= CIC_upsample2) CIC_global->read_ptr = 0;
        CIC_global->read_enable = 1;

        CIC_global->OSR_adaptive = CIC_global->new_OSR;
        CIC_global->div_OSR_adaptive = 1.0f / CIC_global->new_OSR;
    }

    if (++CIC_global->counter_WDIST >= CIC_global->WDIST)
    {
        CIC_global->counter_WDIST = 0;
        if (++CIC_global->write_ptr >= CIC_upsample2) CIC_global->write_ptr = 0;
        CIC_global->write_enable = 1;

        float new_osr = (Uint32)(1.0f / (frequency * CIC_global->Ts) + 0.5f);
        new_osr = CIC_global->OSR_adaptive + Saturation(new_osr - CIC_global->OSR_adaptive, -CIC_global->WDIST, CIC_global->WDIST);

        int32 READ_DIST = CIC_global->write_ptr - CIC_global->read_ptr;
        if (READ_DIST < 0) READ_DIST += CIC_upsample2;
        int32 diff = new_osr - 1 - (READ_DIST - 1) * CIC_global->WDIST;
        if (diff < 0)
        {
            CIC_global->inc_delay = 1;
            diff = diff + CIC_global->WDIST;
        }
        if (diff >= CIC_global->WDIST)
        {
            CIC_global->dec_delay = 1;
            diff = diff - CIC_global->WDIST;
        }

        CIC_global->new_OSR = new_osr;
        CIC_global->counter_WDIST_read_val = diff;
    }
}

#pragma CODE_SECTION(CIC1_adaptive2_filter_CPU, ".interrupt_code");
float CIC1_adaptive2_filter_CPU(struct CIC1_adaptive2_global_struct* CIC_global, struct CIC1_adaptive2_struct* CIC, float input)
{
    CIC->integrator += (int32)(input * CIC->range_modifier);
    if (CIC_global->read_enable) CIC->out = (float)(CIC->integrator - CIC->decimator_memory[CIC_global->read_ptr]) * CIC_global->div_OSR_adaptive * CIC->div_range_modifier;;
    if (CIC_global->write_enable) CIC->decimator_memory[CIC_global->write_ptr] = CIC->integrator;
    return CIC->out;
}

#pragma CODE_SECTION(CIC1_filter_local_CPU, ".interrupt_code");
void CIC1_filter_local_CPU(struct CIC1_global_struct* CIC_global, struct CIC1_local_struct* CIC, float input)
{
    if (!CIC_global->enable_int) return;
    CIC->integrator += (int32)(input * CIC->range_modifier);
    if (!CIC_global->enable_diff) return;
    register int32* decimator = (int32*)&CIC->decimator_memory + CIC_global->div_memory;
    CIC->out = (float)(CIC->integrator - *decimator) * CIC_global->div_OSR * CIC->div_range_modifier;
    *decimator = CIC->integrator;
}

#pragma CODE_SECTION(CIC1_filter_global_CPU, ".interrupt_code");
void CIC1_filter_global_CPU(struct CIC1_global_struct* CIC_global)
{
    CIC_global->enable_diff =
    CIC_global->enable_int = 0;
    if (CIC_global->decimation_counter-- <= 0) CIC_global->decimation_counter = CIC_global->decimation_ratio - 1;
    else return;
    CIC_global->enable_int = 1;
    register float counter_temp = CIC_global->counter;
    if (counter_temp-- <= 0) counter_temp = CIC_global->OSR - 1;
    CIC_global->counter = counter_temp;
    register int32 div_memory_new = (int32)(counter_temp * CIC_upsample1 * CIC_global->div_OSR);
    if (CIC_global->div_memory != div_memory_new)
    {
        CIC_global->div_memory = div_memory_new;
        CIC_global->enable_diff = 1;
    }
}

#pragma CODE_SECTION(Grid_analyzer_calc, ".interrupt_code_unsecure");
void Grid_analyzer_calc()
{
    CIC1_adaptive2_global_calc_CPU(&CIC1_adaptive2_global__50Hz, Conv.f_filter);

    Grid_params.Resonant_U_grid[0].x0 = Conv.Resonant_U_grid[0].x0;
    Grid_params.Resonant_U_grid[1].x0 = Conv.Resonant_U_grid[1].x0;
    Grid_params.Resonant_U_grid[2].x0 = Conv.Resonant_U_grid[2].x0;
    Grid_params.Resonant_U_grid[0].x1 = Conv.Resonant_U_grid[0].x1;
    Grid_params.Resonant_U_grid[1].x1 = Conv.Resonant_U_grid[1].x1;
    Grid_params.Resonant_U_grid[2].x1 = Conv.Resonant_U_grid[2].x1;

    Grid_params.Resonant_I_grid[0].y0 = Conv.Resonant_I_grid[0].y0;
    Grid_params.Resonant_I_grid[1].y0 = Conv.Resonant_I_grid[1].y0;
    Grid_params.Resonant_I_grid[2].y0 = Conv.Resonant_I_grid[2].y0;
    Grid_params.Resonant_I_grid[0].y1 = Conv.Resonant_I_grid[0].y1;
    Grid_params.Resonant_I_grid[1].y1 = Conv.Resonant_I_grid[1].y1;
    Grid_params.Resonant_I_grid[2].y1 = Conv.Resonant_I_grid[2].y1;

    Grid_params.Resonant_I_conv[0].y0 = Conv.Resonant_I_conv[0].y0;
    Grid_params.Resonant_I_conv[1].y0 = Conv.Resonant_I_conv[1].y0;
    Grid_params.Resonant_I_conv[2].y0 = Conv.Resonant_I_conv[2].y0;
    Grid_params.Resonant_I_conv[0].y1 = Conv.Resonant_I_conv[0].y1;
    Grid_params.Resonant_I_conv[1].y1 = Conv.Resonant_I_conv[1].y1;
    Grid_params.Resonant_I_conv[2].y1 = Conv.Resonant_I_conv[2].y1;

    ///////////////////////////////////////////////////////////////////

    Grid.U_grid.a = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_U_grid[0], Meas_ACDC.U_grid.a*Meas_ACDC.U_grid.a));
    Grid.U_grid.b = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_U_grid[1], Meas_ACDC.U_grid.b*Meas_ACDC.U_grid.b));
    Grid.U_grid.c = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_U_grid[2], Meas_ACDC.U_grid.c*Meas_ACDC.U_grid.c));

    Grid.I_grid.a = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_grid[0], Meas_ACDC.I_grid.a*Meas_ACDC.I_grid.a));
    Grid.I_grid.b = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_grid[1], Meas_ACDC.I_grid.b*Meas_ACDC.I_grid.b));
    Grid.I_grid.c = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_grid[2], Meas_ACDC.I_grid.c*Meas_ACDC.I_grid.c));

    Grid.I_conv.a = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_conv[0], Meas_ACDC.I_conv.a*Meas_ACDC.I_conv.a));
    Grid.I_conv.b = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_conv[1], Meas_ACDC.I_conv.b*Meas_ACDC.I_conv.b));
    Grid.I_conv.c = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_conv[2], Meas_ACDC.I_conv.c*Meas_ACDC.I_conv.c));
    Grid.I_conv.n = sqrtf(CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_conv[3], Meas_ACDC.I_conv.n*Meas_ACDC.I_conv.n));

    ///////////////////////////////////////////////////////////////////

    Grid.U_grid_1h.a = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_U_grid_1h[0], sqrtf(0.5f * (Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_U_grid[0].x0 + Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_U_grid[0].x1)) );
    Grid.U_grid_1h.b = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_U_grid_1h[1], sqrtf(0.5f * (Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_U_grid[1].x0 + Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_U_grid[1].x1)) );
    Grid.U_grid_1h.c = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_U_grid_1h[2], sqrtf(0.5f * (Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_U_grid[2].x0 + Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_U_grid[2].x1)) );
    Grid.average.U_grid_1h = (Grid.U_grid_1h.a + Grid.U_grid_1h.b + Grid.U_grid_1h.c) * MATH_1_3;

    Grid.I_grid_1h.a = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_grid_1h[0], sqrtf(0.5f * (Grid_params.Resonant_I_grid[0].y0 * Grid_params.Resonant_I_grid[0].y0 + Grid_params.Resonant_I_grid[0].y1 * Grid_params.Resonant_I_grid[0].y1)) );
    Grid.I_grid_1h.b = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_grid_1h[1], sqrtf(0.5f * (Grid_params.Resonant_I_grid[1].y0 * Grid_params.Resonant_I_grid[1].y0 + Grid_params.Resonant_I_grid[1].y1 * Grid_params.Resonant_I_grid[1].y1)) );
    Grid.I_grid_1h.c = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_I_grid_1h[2], sqrtf(0.5f * (Grid_params.Resonant_I_grid[2].y0 * Grid_params.Resonant_I_grid[2].y0 + Grid_params.Resonant_I_grid[2].y1 * Grid_params.Resonant_I_grid[2].y1)) );

    ///////////////////////////////////////////////////////////////////

    static struct abc_struct Q_conv_temp;
    Q_conv_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_conv[0].y0 - Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_conv[0].y1);
    Q_conv_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_conv[1].y0 - Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_conv[1].y1);
    Q_conv_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_conv[2].y0 - Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_conv[2].y1);
    Grid.Q_conv_1h.a = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_Q_conv_1h[0], Q_conv_temp.a);
    Grid.Q_conv_1h.b = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_Q_conv_1h[1], Q_conv_temp.b);
    Grid.Q_conv_1h.c = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_Q_conv_1h[2], Q_conv_temp.c);

    static struct abc_struct Q_grid_temp;
    Q_grid_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_grid[0].y0 - Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_grid[0].y1);
    Q_grid_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_grid[1].y0 - Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_grid[1].y1);
    Q_grid_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_grid[2].y0 - Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_grid[2].y1);
    Grid.Q_grid_1h.a = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_Q_grid_1h[0], Q_grid_temp.a);
    Grid.Q_grid_1h.b = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_Q_grid_1h[1], Q_grid_temp.b);
    Grid.Q_grid_1h.c = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_Q_grid_1h[2], Q_grid_temp.c);
    Grid.average.Q_grid_1h = Grid.Q_grid_1h.a + Grid.Q_grid_1h.b + Grid.Q_grid_1h.c;

    Grid.Q_load_1h.a = Grid.Q_grid_1h.a + Conv.master.total.Q_conv_1h.a;
    Grid.Q_load_1h.b = Grid.Q_grid_1h.b + Conv.master.total.Q_conv_1h.b;
    Grid.Q_load_1h.c = Grid.Q_grid_1h.c + Conv.master.total.Q_conv_1h.c;
    Grid.average.Q_load_1h = Grid.Q_load_1h.a + Grid.Q_load_1h.b + Grid.Q_load_1h.c;


    static struct abc_struct P_conv_temp;
    P_conv_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_conv[0].y0 + Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_conv[0].y1);
    P_conv_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_conv[1].y0 + Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_conv[1].y1);
    P_conv_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_conv[2].y0 + Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_conv[2].y1);
    Grid.P_conv_1h.a = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_P_conv_1h[0], P_conv_temp.a);
    Grid.P_conv_1h.b = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_P_conv_1h[1], P_conv_temp.b);
    Grid.P_conv_1h.c = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_P_conv_1h[2], P_conv_temp.c);

    static struct abc_struct P_grid_temp;
    P_grid_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_grid[0].y0 + Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_grid[0].y1);
    P_grid_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_grid[1].y0 + Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_grid[1].y1);
    P_grid_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_grid[2].y0 + Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_grid[2].y1);
    Grid.P_grid_1h.a = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_P_grid_1h[0], P_grid_temp.a);
    Grid.P_grid_1h.b = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_P_grid_1h[1], P_grid_temp.b);
    Grid.P_grid_1h.c = CIC1_adaptive2_filter_CPU(&CIC1_adaptive2_global__50Hz, &Grid_params.CIC1_P_grid_1h[2], P_grid_temp.c);
    Grid.average.P_grid_1h = Grid.P_grid_1h.a + Grid.P_grid_1h.b + Grid.P_grid_1h.c;

    Grid.P_load_1h.a = Grid.P_grid_1h.a + Conv.master.total.P_conv_1h.a;
    Grid.P_load_1h.b = Grid.P_grid_1h.b + Conv.master.total.P_conv_1h.b;
    Grid.P_load_1h.c = Grid.P_grid_1h.c + Conv.master.total.P_conv_1h.c;
    Grid.average.P_load_1h = Grid.P_load_1h.a + Grid.P_load_1h.b + Grid.P_load_1h.c;

    ///////////////////////////////////////////////////////////////////

    Grid.S_load_1h.a = sqrtf(Grid.P_load_1h.a * Grid.P_load_1h.a + Grid.Q_load_1h.a * Grid.Q_load_1h.a);
    Grid.S_load_1h.b = sqrtf(Grid.P_load_1h.b * Grid.P_load_1h.b + Grid.Q_load_1h.b * Grid.Q_load_1h.b);
    Grid.S_load_1h.c = sqrtf(Grid.P_load_1h.c * Grid.P_load_1h.c + Grid.Q_load_1h.c * Grid.Q_load_1h.c);
    Grid.average.S_load_1h = Grid.S_load_1h.a + Grid.S_load_1h.b + Grid.S_load_1h.c;
    Grid.S_grid_1h.a = sqrtf(Grid.P_grid_1h.a * Grid.P_grid_1h.a + Grid.Q_grid_1h.a * Grid.Q_grid_1h.a);
    Grid.S_grid_1h.b = sqrtf(Grid.P_grid_1h.b * Grid.P_grid_1h.b + Grid.Q_grid_1h.b * Grid.Q_grid_1h.b);
    Grid.S_grid_1h.c = sqrtf(Grid.P_grid_1h.c * Grid.P_grid_1h.c + Grid.Q_grid_1h.c * Grid.Q_grid_1h.c);
    Grid.average.S_grid_1h = Grid.S_grid_1h.a + Grid.S_grid_1h.b + Grid.S_grid_1h.c;
    Grid.S_conv_1h.a = sqrtf(Grid.P_conv_1h.a * Grid.P_conv_1h.a + Grid.Q_conv_1h.a * Grid.Q_conv_1h.a);
    Grid.S_conv_1h.b = sqrtf(Grid.P_conv_1h.b * Grid.P_conv_1h.b + Grid.Q_conv_1h.b * Grid.Q_conv_1h.b);
    Grid.S_conv_1h.c = sqrtf(Grid.P_conv_1h.c * Grid.P_conv_1h.c + Grid.Q_conv_1h.c * Grid.Q_conv_1h.c);

    Grid.S_grid.a = Grid.U_grid.a * Grid.I_grid.a;
    Grid.S_grid.b = Grid.U_grid.b * Grid.I_grid.b;
    Grid.S_grid.c = Grid.U_grid.c * Grid.I_grid.c;
    Grid.S_conv.a = Grid.U_grid.a * Grid.I_conv.a;
    Grid.S_conv.b = Grid.U_grid.b * Grid.I_conv.b;
    Grid.S_conv.c = Grid.U_grid.c * Grid.I_conv.c;

    Grid.PF_grid_1h.a = Grid.P_grid_1h.a / fmaxf(Grid.S_grid_1h.a, 1.0f);
    Grid.PF_grid_1h.b = Grid.P_grid_1h.b / fmaxf(Grid.S_grid_1h.b, 1.0f);
    Grid.PF_grid_1h.c = Grid.P_grid_1h.c / fmaxf(Grid.S_grid_1h.c, 1.0f);
    Grid.average.PF_grid_1h = (fabsf(Grid.P_grid_1h.a) + fabsf(Grid.P_grid_1h.b) + fabsf(Grid.P_grid_1h.c)) / fmaxf(Grid.average.S_grid_1h, 1.0f);

    register float div_I_lim = 1.0f / fmaxf(Conv.I_lim_nominal, 1.0f);
    Grid.Used_resources.a = Grid.I_conv.a * div_I_lim;
    Grid.Used_resources.b = Grid.I_conv.b * div_I_lim;
    Grid.Used_resources.c = Grid.I_conv.c * div_I_lim;
    Grid.Used_resources.n = Grid.I_conv.n * div_I_lim;
}

#pragma CODE_SECTION(Energy_meter_calc, ".interrupt_code");
void Energy_meter_calc()
{
    register Uint32 float_one = 0x3f800000;
    register Uint32 sign_bit = 0x80000000;
    static volatile union {float f32; Uint32 u32;}sign_temp[4];
    sign_temp[0].u32 = float_one | (*(Uint32 *)&Grid.P_grid_1h.a & sign_bit);
    sign_temp[1].u32 = float_one | (*(Uint32 *)&Grid.P_grid_1h.b & sign_bit);
    sign_temp[2].u32 = float_one | (*(Uint32 *)&Grid.P_grid_1h.c & sign_bit);
    sign_temp[3].u32 = float_one | (*(Uint32 *)&Grid.average.P_grid_1h & sign_bit);

    register float gain = Energy_meter_params.Accumulator_gain;
    register float temp;
    register float sign;
    sign = sign_temp[0].f32 * gain;
    Energy_meter_params.input_P_p[0] = fabsf(gain * fmaxf(Grid.P_grid_1h.a, 0.0f));
    Energy_meter_params.input_P_n[0] = fabsf(gain * fminf(Grid.P_grid_1h.a, 0.0f));
    temp = sign * fmaxf(Grid.Q_grid_1h.a, 0.0f);
    Energy_meter_params.input_QI[0] = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.input_QII[0] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.Q_grid_1h.a, 0.0f);
    Energy_meter_params.input_QIII[0] = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.input_QIV[0] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[1].f32 * gain;
    Energy_meter_params.input_P_p[1] = fabsf(gain * fmaxf(Grid.P_grid_1h.b, 0.0f));
    Energy_meter_params.input_P_n[1] = fabsf(gain * fminf(Grid.P_grid_1h.b, 0.0f));
    temp = sign * fmaxf(Grid.Q_grid_1h.b, 0.0f);
    Energy_meter_params.input_QI[1] = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.input_QII[1] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.Q_grid_1h.b, 0.0f);
    Energy_meter_params.input_QIII[1] = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.input_QIV[1] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[2].f32 * gain;
    Energy_meter_params.input_P_p[2] = fabsf(gain * fmaxf(Grid.P_grid_1h.c, 0.0f));
    Energy_meter_params.input_P_n[2] = fabsf(gain * fminf(Grid.P_grid_1h.c, 0.0f));
    temp = sign * fmaxf(Grid.Q_grid_1h.c, 0.0f);
    Energy_meter_params.input_QI[2] = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.input_QII[2] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.Q_grid_1h.c, 0.0f);
    Energy_meter_params.input_QIII[2] = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.input_QIV[2] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[3].f32 * gain;
    Energy_meter_params.sum.input_P_p = fabsf(gain * fmaxf(Grid.average.P_grid_1h, 0.0f));
    Energy_meter_params.sum.input_P_n = fabsf(gain * fminf(Grid.average.P_grid_1h, 0.0f));
    temp = sign * fmaxf(Grid.average.Q_grid_1h, 0.0f);
    Energy_meter_params.sum.input_QI = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.sum.input_QII = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.average.Q_grid_1h, 0.0f);
    Energy_meter_params.sum.input_QIII = fabsf(fmaxf(temp, 0.0f));
    Energy_meter_params.sum.input_QIV = fabsf(fminf(temp, 0.0f));
}

#pragma CODE_SECTION(Grid_analyzer_filter_calc, ".interrupt_code");
void Grid_analyzer_filter_calc()
{
    CIC1_filter_global_CPU(&CIC1_global__50Hz);

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_U_grid[0], Grid.U_grid.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_U_grid[1], Grid.U_grid.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_U_grid[2], Grid.U_grid.c);
    Grid_filter.U_grid.a = Grid_filter_params.CIC1_U_grid[0].out;
    Grid_filter.U_grid.b = Grid_filter_params.CIC1_U_grid[1].out;
    Grid_filter.U_grid.c = Grid_filter_params.CIC1_U_grid[2].out;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_grid[0], Grid.I_grid.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_grid[1], Grid.I_grid.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_grid[2], Grid.I_grid.c);
    Grid_filter.I_grid.a = Grid_filter_params.CIC1_I_grid[0].out;
    Grid_filter.I_grid.b = Grid_filter_params.CIC1_I_grid[1].out;
    Grid_filter.I_grid.c = Grid_filter_params.CIC1_I_grid[2].out;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_conv[0], Grid.I_conv.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_conv[1], Grid.I_conv.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_conv[2], Grid.I_conv.c);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_conv[3], Grid.I_conv.n);
    Grid_filter.I_conv.a = Grid_filter_params.CIC1_I_conv[0].out;
    Grid_filter.I_conv.b = Grid_filter_params.CIC1_I_conv[1].out;
    Grid_filter.I_conv.c = Grid_filter_params.CIC1_I_conv[2].out;
    Grid_filter.I_conv.n = Grid_filter_params.CIC1_I_conv[3].out;

    ///////////////////////////////////////////////////////////////////

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_U_grid_1h[0], Grid.U_grid_1h.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_U_grid_1h[1], Grid.U_grid_1h.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_U_grid_1h[2], Grid.U_grid_1h.c);
    Grid_filter.U_grid_1h.a = Grid_filter_params.CIC1_U_grid_1h[0].out;
    Grid_filter.U_grid_1h.b = Grid_filter_params.CIC1_U_grid_1h[1].out;
    Grid_filter.U_grid_1h.c = Grid_filter_params.CIC1_U_grid_1h[2].out;
    Grid_filter.average.U_grid_1h = (Grid_filter.U_grid_1h.a + Grid_filter.U_grid_1h.b + Grid_filter.U_grid_1h.c) * MATH_1_3;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_grid_1h[0], Grid.I_grid_1h.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_grid_1h[1], Grid.I_grid_1h.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_I_grid_1h[2], Grid.I_grid_1h.c);
    Grid_filter.I_grid_1h.a = Grid_filter_params.CIC1_I_grid_1h[0].out;
    Grid_filter.I_grid_1h.b = Grid_filter_params.CIC1_I_grid_1h[1].out;
    Grid_filter.I_grid_1h.c = Grid_filter_params.CIC1_I_grid_1h[2].out;

    ///////////////////////////////////////////////////////////////////

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_Q_conv_1h[0], Grid.Q_conv_1h.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_Q_conv_1h[1], Grid.Q_conv_1h.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_Q_conv_1h[2], Grid.Q_conv_1h.c);
    Grid_filter.Q_conv_1h.a = Grid_filter_params.CIC1_Q_conv_1h[0].out;
    Grid_filter.Q_conv_1h.b = Grid_filter_params.CIC1_Q_conv_1h[1].out;
    Grid_filter.Q_conv_1h.c = Grid_filter_params.CIC1_Q_conv_1h[2].out;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_Q_grid_1h[0], Grid.Q_grid_1h.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_Q_grid_1h[1], Grid.Q_grid_1h.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_Q_grid_1h[2], Grid.Q_grid_1h.c);
    Grid_filter.Q_grid_1h.a = Grid_filter_params.CIC1_Q_grid_1h[0].out;
    Grid_filter.Q_grid_1h.b = Grid_filter_params.CIC1_Q_grid_1h[1].out;
    Grid_filter.Q_grid_1h.c = Grid_filter_params.CIC1_Q_grid_1h[2].out;
    Grid_filter.average.Q_grid_1h = Grid_filter.Q_grid_1h.a + Grid_filter.Q_grid_1h.b + Grid_filter.Q_grid_1h.c;

    Grid_filter.Q_load_1h.a = Grid_filter.Q_grid_1h.a;
    Grid_filter.Q_load_1h.b = Grid_filter.Q_grid_1h.b;
    Grid_filter.Q_load_1h.c = Grid_filter.Q_grid_1h.c;
    Grid_filter.average.Q_load_1h = Grid_filter.Q_load_1h.a + Grid_filter.Q_load_1h.b + Grid_filter.Q_load_1h.c;


    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_P_conv_1h[0], Grid.P_conv_1h.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_P_conv_1h[1], Grid.P_conv_1h.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_P_conv_1h[2], Grid.P_conv_1h.c);
    Grid_filter.P_conv_1h.a = Grid_filter_params.CIC1_P_conv_1h[0].out;
    Grid_filter.P_conv_1h.b = Grid_filter_params.CIC1_P_conv_1h[1].out;
    Grid_filter.P_conv_1h.c = Grid_filter_params.CIC1_P_conv_1h[2].out;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_P_grid_1h[0], Grid.P_grid_1h.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_P_grid_1h[1], Grid.P_grid_1h.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_P_grid_1h[2], Grid.P_grid_1h.c);
    Grid_filter.P_grid_1h.a = Grid_filter_params.CIC1_P_grid_1h[0].out;
    Grid_filter.P_grid_1h.b = Grid_filter_params.CIC1_P_grid_1h[1].out;
    Grid_filter.P_grid_1h.c = Grid_filter_params.CIC1_P_grid_1h[2].out;
    Grid_filter.average.P_grid_1h = Grid_filter.P_grid_1h.a + Grid_filter.P_grid_1h.b + Grid_filter.P_grid_1h.c;

    Grid_filter.P_load_1h.a = Grid_filter.P_grid_1h.a;
    Grid_filter.P_load_1h.b = Grid_filter.P_grid_1h.b;
    Grid_filter.P_load_1h.c = Grid_filter.P_grid_1h.c;
    Grid_filter.average.P_load_1h = Grid_filter.P_load_1h.a + Grid_filter.P_load_1h.b + Grid_filter.P_load_1h.c;

    ///////////////////////////////////////////////////////////////////

    Grid_filter.S_load_1h.a = sqrtf(Grid_filter.P_load_1h.a * Grid_filter.P_load_1h.a + Grid_filter.Q_load_1h.a * Grid_filter.Q_load_1h.a);
    Grid_filter.S_load_1h.b = sqrtf(Grid_filter.P_load_1h.b * Grid_filter.P_load_1h.b + Grid_filter.Q_load_1h.b * Grid_filter.Q_load_1h.b);
    Grid_filter.S_load_1h.c = sqrtf(Grid_filter.P_load_1h.c * Grid_filter.P_load_1h.c + Grid_filter.Q_load_1h.c * Grid_filter.Q_load_1h.c);
    Grid_filter.average.S_load_1h = Grid_filter.S_load_1h.a + Grid_filter.S_load_1h.b + Grid_filter.S_load_1h.c;
    Grid_filter.S_grid_1h.a = sqrtf(Grid_filter.P_grid_1h.a * Grid_filter.P_grid_1h.a + Grid_filter.Q_grid_1h.a * Grid_filter.Q_grid_1h.a);
    Grid_filter.S_grid_1h.b = sqrtf(Grid_filter.P_grid_1h.b * Grid_filter.P_grid_1h.b + Grid_filter.Q_grid_1h.b * Grid_filter.Q_grid_1h.b);
    Grid_filter.S_grid_1h.c = sqrtf(Grid_filter.P_grid_1h.c * Grid_filter.P_grid_1h.c + Grid_filter.Q_grid_1h.c * Grid_filter.Q_grid_1h.c);
    Grid_filter.average.S_grid_1h = Grid_filter.S_grid_1h.a + Grid_filter.S_grid_1h.b + Grid_filter.S_grid_1h.c;
    Grid_filter.S_conv_1h.a = sqrtf(Grid_filter.P_conv_1h.a * Grid_filter.P_conv_1h.a + Grid_filter.Q_conv_1h.a * Grid_filter.Q_conv_1h.a);
    Grid_filter.S_conv_1h.b = sqrtf(Grid_filter.P_conv_1h.b * Grid_filter.P_conv_1h.b + Grid_filter.Q_conv_1h.b * Grid_filter.Q_conv_1h.b);
    Grid_filter.S_conv_1h.c = sqrtf(Grid_filter.P_conv_1h.c * Grid_filter.P_conv_1h.c + Grid_filter.Q_conv_1h.c * Grid_filter.Q_conv_1h.c);

    Grid_filter.S_grid.a = Grid_filter.U_grid.a * Grid_filter.I_grid.a;
    Grid_filter.S_grid.b = Grid_filter.U_grid.b * Grid_filter.I_grid.b;
    Grid_filter.S_grid.c = Grid_filter.U_grid.c * Grid_filter.I_grid.c;
    Grid_filter.S_conv.a = Grid_filter.U_grid.a * Grid_filter.I_conv.a;
    Grid_filter.S_conv.b = Grid_filter.U_grid.b * Grid_filter.I_conv.b;
    Grid_filter.S_conv.c = Grid_filter.U_grid.c * Grid_filter.I_conv.c;

    Grid_filter.PF_grid_1h.a = Grid_filter.P_grid_1h.a / fmaxf(Grid_filter.S_grid_1h.a, 1.0f);
    Grid_filter.PF_grid_1h.b = Grid_filter.P_grid_1h.b / fmaxf(Grid_filter.S_grid_1h.b, 1.0f);
    Grid_filter.PF_grid_1h.c = Grid_filter.P_grid_1h.c / fmaxf(Grid_filter.S_grid_1h.c, 1.0f);
    Grid_filter.average.PF_grid_1h = (fabsf(Grid_filter.P_grid_1h.a) + fabsf(Grid_filter.P_grid_1h.b) + fabsf(Grid_filter.P_grid_1h.c)) / fmaxf(Grid_filter.average.S_grid_1h, 1.0f);

    register float div_I_lim = 1.0f / fmaxf(Conv.I_lim, 1.0f);
    Grid_filter.Used_resources.a = Grid_filter.I_conv.a * div_I_lim;
    Grid_filter.Used_resources.b = Grid_filter.I_conv.b * div_I_lim;
    Grid_filter.Used_resources.c = Grid_filter.I_conv.c * div_I_lim;
    Grid_filter.Used_resources.n = Grid_filter.I_conv.n * div_I_lim;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_THD_U_grid[0], Grid.THD_U_grid.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_THD_U_grid[1], Grid.THD_U_grid.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_THD_U_grid[2], Grid.THD_U_grid.c);
    Grid_filter.THD_U_grid.a = Grid_filter_params.CIC1_THD_U_grid[0].out;
    Grid_filter.THD_U_grid.b = Grid_filter_params.CIC1_THD_U_grid[1].out;
    Grid_filter.THD_U_grid.c = Grid_filter_params.CIC1_THD_U_grid[2].out;

    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_THD_I_grid[0], Grid.THD_I_grid.a);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_THD_I_grid[1], Grid.THD_I_grid.b);
    CIC1_filter_local_CPU(&CIC1_global__50Hz, &Grid_filter_params.CIC1_THD_I_grid[2], Grid.THD_I_grid.c);
    Grid_filter.THD_I_grid.a = Grid_filter_params.CIC1_THD_I_grid[0].out;
    Grid_filter.THD_I_grid.b = Grid_filter_params.CIC1_THD_I_grid[1].out;
    Grid_filter.THD_I_grid.c = Grid_filter_params.CIC1_THD_I_grid[2].out;
}
