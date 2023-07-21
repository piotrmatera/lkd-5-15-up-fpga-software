// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include <math.h>

#include "stdafx.h"

#pragma CODE_SECTION(CIC1_adaptive_filter_CPU, ".TI.ramfunc");
float CIC1_adaptive_filter_CPU(struct CIC1_adaptive_global_struct *CIC_global, struct CIC1_adaptive_struct *CIC, float input)
{
    CIC->integrator += (int32)(input * CIC->range_modifier);

    if (*(Uint32 *)&CIC_global->cycle_enable[0])
    {
        register int32 *decimator = &(CIC->decimator_memory[0][CIC_global->div_memory[0] >> 1]);
        CIC->out_temp[0] = (float)(CIC->integrator - *decimator) * CIC_global->div_OSR_adaptive[0] * CIC->div_range_modifier;
        *decimator = CIC->integrator;
    }
    if (*(Uint32 *)&CIC_global->cycle_enable[1])
    {
        register int32 *decimator = &(CIC->decimator_memory[1][CIC_global->div_memory[1] >> 1]);
        CIC->out_temp[1] = (float)(CIC->integrator - *decimator) * CIC_global->div_OSR_adaptive[1] * CIC->div_range_modifier;
        *decimator = CIC->integrator;
    }
    return CIC->out = CIC_global->select_output ? CIC->out_temp[1] : CIC->out_temp[0];
}

#pragma CODE_SECTION(CIC1_filter_CPU, ".TI.ramfunc");
void CIC1_filter_CPU(struct CIC1_struct *CIC, float input)
{
    if(CIC->decimation_counter-- <= 0.0f) CIC->decimation_counter = CIC->decimation_ratio - 1.0f;
    else return;
    CIC->integrator += (int32)(input * CIC->range_modifier);
    register float counter_temp = CIC->counter;
    if (counter_temp-- <= 0.0f) counter_temp = CIC->OSR - 1.0f;
    CIC->counter = counter_temp;
    register int32 div_memory_new = (int32)(counter_temp*CIC_upsample1*CIC->div_OSR);
    if (CIC->div_memory != div_memory_new)
    {
        CIC->div_memory = div_memory_new;
        register int32 *decimator = (int32 *)&CIC->decimator_memory + div_memory_new;
        CIC->out = (float)(CIC->integrator - *decimator) * CIC->div_OSR * CIC->div_range_modifier;
        *decimator = CIC->integrator;
    }
}

#pragma CODE_SECTION(Resonant_filter_calc_CPU, ".TI.ramfunc");
void Resonant_filter_calc_CPU(struct Resonant_struct* Resonant, float input)
{
    register float error = (input - Resonant->x0) * Resonant->gain;
    register float cosine = Resonant->trigonometric.ptr->cosine;
    register float sine = Resonant->trigonometric.ptr->sine;
    register float temp0 = cosine * Resonant->x0 - sine * Resonant->x1 + sine * error;
    register float temp1 = sine * Resonant->x0 + cosine * Resonant->x1 + cosine * error - error;
    Resonant->x0 = temp0;
    Resonant->x1 = temp1;
    cosine = Resonant->trigonometric_comp.ptr->cosine;
    sine = Resonant->trigonometric_comp.ptr->sine;
    Resonant->y0 = cosine * Resonant->x0 - sine * Resonant->x1;
    Resonant->y1 = sine * Resonant->x0 + cosine * Resonant->x1;
}

#pragma CODE_SECTION(Grid_analyzer_calc, ".TI.ramfunc");
void Grid_analyzer_calc()
{
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_U_grid[0], Meas_master.U_grid.a);
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_U_grid[1], Meas_master.U_grid.b);
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_U_grid[2], Meas_master.U_grid.c);
//
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_I_grid[0], Meas_master.I_grid.a);
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_I_grid[1], Meas_master.I_grid.b);
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_I_grid[2], Meas_master.I_grid.c);
//
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_I_conv[0], Meas_master.I_conv.a);
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_I_conv[1], Meas_master.I_conv.b);
//    Resonant_filter_calc_CPU(&Grid_params.Resonant_I_conv[2], Meas_master.I_conv.c);

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

    Grid.U_grid.a = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_U_grid[0], Meas_ACDC.U_grid.a*Meas_ACDC.U_grid.a));
    Grid.U_grid.b = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_U_grid[1], Meas_ACDC.U_grid.b*Meas_ACDC.U_grid.b));
    Grid.U_grid.c = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_U_grid[2], Meas_ACDC.U_grid.c*Meas_ACDC.U_grid.c));

    Grid.I_grid.a = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_grid[0], Meas_ACDC.I_grid.a*Meas_ACDC.I_grid.a));
    Grid.I_grid.b = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_grid[1], Meas_ACDC.I_grid.b*Meas_ACDC.I_grid.b));
    Grid.I_grid.c = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_grid[2], Meas_ACDC.I_grid.c*Meas_ACDC.I_grid.c));

    Grid.I_conv.a = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_conv[0], Meas_ACDC.I_conv.a*Meas_ACDC.I_conv.a));
    Grid.I_conv.b = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_conv[1], Meas_ACDC.I_conv.b*Meas_ACDC.I_conv.b));
    Grid.I_conv.c = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_conv[2], Meas_ACDC.I_conv.c*Meas_ACDC.I_conv.c));
    Grid.I_conv.n = sqrtf(CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_conv[3], Meas_ACDC.I_conv.n*Meas_ACDC.I_conv.n));

    ///////////////////////////////////////////////////////////////////

    Grid.U_grid_1h.a = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_U_grid_1h[0], sqrtf(0.5f * (Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_U_grid[0].x0 + Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_U_grid[0].x1)) );
    Grid.U_grid_1h.b = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_U_grid_1h[1], sqrtf(0.5f * (Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_U_grid[1].x0 + Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_U_grid[1].x1)) );
    Grid.U_grid_1h.c = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_U_grid_1h[2], sqrtf(0.5f * (Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_U_grid[2].x0 + Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_U_grid[2].x1)) );
    Grid.average.U_grid_1h = (Grid.U_grid_1h.a + Grid.U_grid_1h.b + Grid.U_grid_1h.c) * MATH_1_3;

    Grid.I_grid_1h.a = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_grid_1h[0], sqrtf(0.5f * (Grid_params.Resonant_I_grid[0].x0 * Grid_params.Resonant_I_grid[0].x0 + Grid_params.Resonant_I_grid[0].x1 * Grid_params.Resonant_I_grid[0].x1)) );
    Grid.I_grid_1h.b = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_grid_1h[1], sqrtf(0.5f * (Grid_params.Resonant_I_grid[1].x0 * Grid_params.Resonant_I_grid[1].x0 + Grid_params.Resonant_I_grid[1].x1 * Grid_params.Resonant_I_grid[1].x1)) );
    Grid.I_grid_1h.c = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_I_grid_1h[2], sqrtf(0.5f * (Grid_params.Resonant_I_grid[2].x0 * Grid_params.Resonant_I_grid[2].x0 + Grid_params.Resonant_I_grid[2].x1 * Grid_params.Resonant_I_grid[2].x1)) );

    ///////////////////////////////////////////////////////////////////

    static struct abc_struct Q_conv_temp;
    Q_conv_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_conv[0].y0 - Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_conv[0].y1);
    Q_conv_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_conv[1].y0 - Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_conv[1].y1);
    Q_conv_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_conv[2].y0 - Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_conv[2].y1);
    Grid.Q_conv_1h.a = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_Q_conv_1h[0], Q_conv_temp.a);
    Grid.Q_conv_1h.b = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_Q_conv_1h[1], Q_conv_temp.b);
    Grid.Q_conv_1h.c = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_Q_conv_1h[2], Q_conv_temp.c);

    static struct abc_struct Q_grid_temp;
    Q_grid_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_grid[0].y0 - Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_grid[0].y1);
    Q_grid_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_grid[1].y0 - Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_grid[1].y1);
    Q_grid_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_grid[2].y0 - Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_grid[2].y1);
    Grid.Q_grid_1h.a = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_Q_grid_1h[0], Q_grid_temp.a);
    Grid.Q_grid_1h.b = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_Q_grid_1h[1], Q_grid_temp.b);
    Grid.Q_grid_1h.c = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_Q_grid_1h[2], Q_grid_temp.c);
    Grid.average.Q_grid_1h = Grid.Q_grid_1h.a + Grid.Q_grid_1h.b + Grid.Q_grid_1h.c;

    Grid.Q_load_1h.a = Grid.Q_grid_1h.a + Conv.master.total.Q_conv_1h.a;
    Grid.Q_load_1h.b = Grid.Q_grid_1h.b + Conv.master.total.Q_conv_1h.b;
    Grid.Q_load_1h.c = Grid.Q_grid_1h.c + Conv.master.total.Q_conv_1h.c;
    Grid.average.Q_load_1h = Grid.Q_load_1h.a + Grid.Q_load_1h.b + Grid.Q_load_1h.c;


    static struct abc_struct P_conv_temp;
    P_conv_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_conv[0].y0 + Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_conv[0].y1);
    P_conv_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_conv[1].y0 + Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_conv[1].y1);
    P_conv_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_conv[2].y0 + Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_conv[2].y1);
    Grid.P_conv_1h.a = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_P_conv_1h[0], P_conv_temp.a);
    Grid.P_conv_1h.b = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_P_conv_1h[1], P_conv_temp.b);
    Grid.P_conv_1h.c = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_P_conv_1h[2], P_conv_temp.c);

    static struct abc_struct P_grid_temp;
    P_grid_temp.a = 0.5f*(Grid_params.Resonant_U_grid[0].x0 * Grid_params.Resonant_I_grid[0].y0 + Grid_params.Resonant_U_grid[0].x1 * Grid_params.Resonant_I_grid[0].y1);
    P_grid_temp.b = 0.5f*(Grid_params.Resonant_U_grid[1].x0 * Grid_params.Resonant_I_grid[1].y0 + Grid_params.Resonant_U_grid[1].x1 * Grid_params.Resonant_I_grid[1].y1);
    P_grid_temp.c = 0.5f*(Grid_params.Resonant_U_grid[2].x0 * Grid_params.Resonant_I_grid[2].y0 + Grid_params.Resonant_U_grid[2].x1 * Grid_params.Resonant_I_grid[2].y1);
    Grid.P_grid_1h.a = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_P_grid_1h[0], P_grid_temp.a);
    Grid.P_grid_1h.b = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_P_grid_1h[1], P_grid_temp.b);
    Grid.P_grid_1h.c = CIC1_adaptive_filter_CPU(&CIC1_adaptive_global__50Hz, &Grid_params.CIC1_P_grid_1h[2], P_grid_temp.c);
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

    register Uint32 float_one = 0x3f800000;
    register Uint32 sign_bit = 0x80000000;
    static volatile union {float f32; Uint32 u32;}sign_temp[4];
    sign_temp[0].u32 = float_one | (*(Uint32 *)&Grid.P_grid_1h.a & sign_bit);
    sign_temp[1].u32 = float_one | (*(Uint32 *)&Grid.P_grid_1h.b & sign_bit);
    sign_temp[2].u32 = float_one | (*(Uint32 *)&Grid.P_grid_1h.c & sign_bit);
    sign_temp[3].u32 = float_one | (*(Uint32 *)&Grid.average.P_grid_1h & sign_bit);

    register float gain = Grid_params.Accumulator_gain;
    register float temp;
    register float sign;
    sign = sign_temp[0].f32 * gain;
    Grid_params.input_P_p[0] = fabsf(gain * fmaxf(Grid.P_grid_1h.a, 0.0f));
    Grid_params.input_P_n[0] = fabsf(gain * fminf(Grid.P_grid_1h.a, 0.0f));
    temp = sign * fmaxf(Grid.Q_grid_1h.a, 0.0f);
    Grid_params.input_QI[0] = fabsf(fmaxf(temp, 0.0f));
    Grid_params.input_QII[0] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.Q_grid_1h.a, 0.0f);
    Grid_params.input_QIII[0] = fabsf(fmaxf(temp, 0.0f));
    Grid_params.input_QIV[0] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[1].f32 * gain;
    Grid_params.input_P_p[1] = fabsf(gain * fmaxf(Grid.P_grid_1h.b, 0.0f));
    Grid_params.input_P_n[1] = fabsf(gain * fminf(Grid.P_grid_1h.b, 0.0f));
    temp = sign * fmaxf(Grid.Q_grid_1h.b, 0.0f);
    Grid_params.input_QI[1] = fabsf(fmaxf(temp, 0.0f));
    Grid_params.input_QII[1] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.Q_grid_1h.b, 0.0f);
    Grid_params.input_QIII[1] = fabsf(fmaxf(temp, 0.0f));
    Grid_params.input_QIV[1] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[2].f32 * gain;
    Grid_params.input_P_p[2] = fabsf(gain * fmaxf(Grid.P_grid_1h.c, 0.0f));
    Grid_params.input_P_n[2] = fabsf(gain * fminf(Grid.P_grid_1h.c, 0.0f));
    temp = sign * fmaxf(Grid.Q_grid_1h.c, 0.0f);
    Grid_params.input_QI[2] = fabsf(fmaxf(temp, 0.0f));
    Grid_params.input_QII[2] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.Q_grid_1h.c, 0.0f);
    Grid_params.input_QIII[2] = fabsf(fmaxf(temp, 0.0f));
    Grid_params.input_QIV[2] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[3].f32 * gain;
    Grid_params.sum.input_P_p = fabsf(gain * fmaxf(Grid.average.P_grid_1h, 0.0f));
    Grid_params.sum.input_P_n = fabsf(gain * fminf(Grid.average.P_grid_1h, 0.0f));
    temp = sign * fmaxf(Grid.average.Q_grid_1h, 0.0f);
    Grid_params.sum.input_QI = fabsf(fmaxf(temp, 0.0f));
    Grid_params.sum.input_QII = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.average.Q_grid_1h, 0.0f);
    Grid_params.sum.input_QIII = fabsf(fmaxf(temp, 0.0f));
    Grid_params.sum.input_QIV = fabsf(fminf(temp, 0.0f));
}

//#pragma CODE_SECTION(Grid_analyzer_filter_calc, ".TI.ramfunc");
//void Grid_analyzer_filter_calc()
//{
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_U_grid[0], Grid.U_grid.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_U_grid[1], Grid.U_grid.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_U_grid[2], Grid.U_grid.c);
//    Grid_filter.U_grid.a = Grid_filter_params.CIC1_U_grid[0].out;
//    Grid_filter.U_grid.b = Grid_filter_params.CIC1_U_grid[1].out;
//    Grid_filter.U_grid.c = Grid_filter_params.CIC1_U_grid[2].out;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_grid[0], Grid.I_grid.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_grid[1], Grid.I_grid.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_grid[2], Grid.I_grid.c);
//    Grid_filter.I_grid.a = Grid_filter_params.CIC1_I_grid[0].out;
//    Grid_filter.I_grid.b = Grid_filter_params.CIC1_I_grid[1].out;
//    Grid_filter.I_grid.c = Grid_filter_params.CIC1_I_grid[2].out;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_conv[0], Grid.I_conv.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_conv[1], Grid.I_conv.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_conv[2], Grid.I_conv.c);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_conv[3], Grid.I_conv.n);
//    Grid_filter.I_conv.a = Grid_filter_params.CIC1_I_conv[0].out;
//    Grid_filter.I_conv.b = Grid_filter_params.CIC1_I_conv[1].out;
//    Grid_filter.I_conv.c = Grid_filter_params.CIC1_I_conv[2].out;
//    Grid_filter.I_conv.n = Grid_filter_params.CIC1_I_conv[3].out;
//
//    ///////////////////////////////////////////////////////////////////
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_U_grid_1h[0], Grid.U_grid_1h.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_U_grid_1h[1], Grid.U_grid_1h.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_U_grid_1h[2], Grid.U_grid_1h.c);
//    Grid_filter.U_grid_1h.a = Grid_filter_params.CIC1_U_grid_1h[0].out;
//    Grid_filter.U_grid_1h.b = Grid_filter_params.CIC1_U_grid_1h[1].out;
//    Grid_filter.U_grid_1h.c = Grid_filter_params.CIC1_U_grid_1h[2].out;
//    Grid_filter.average.U_grid_1h = (Grid_filter.U_grid_1h.a + Grid_filter.U_grid_1h.b + Grid_filter.U_grid_1h.c) * MATH_1_3;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_grid_1h[0], Grid.I_grid_1h.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_grid_1h[1], Grid.I_grid_1h.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_I_grid_1h[2], Grid.I_grid_1h.c);
//    Grid_filter.I_grid_1h.a = Grid_filter_params.CIC1_I_grid_1h[0].out;
//    Grid_filter.I_grid_1h.b = Grid_filter_params.CIC1_I_grid_1h[1].out;
//    Grid_filter.I_grid_1h.c = Grid_filter_params.CIC1_I_grid_1h[2].out;
//
//    ///////////////////////////////////////////////////////////////////
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_Q_conv_1h[0], Grid.Q_conv_1h.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_Q_conv_1h[1], Grid.Q_conv_1h.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_Q_conv_1h[2], Grid.Q_conv_1h.c);
//    Grid_filter.Q_conv_1h.a = Grid_filter_params.CIC1_Q_conv_1h[0].out;
//    Grid_filter.Q_conv_1h.b = Grid_filter_params.CIC1_Q_conv_1h[1].out;
//    Grid_filter.Q_conv_1h.c = Grid_filter_params.CIC1_Q_conv_1h[2].out;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_Q_grid_1h[0], Grid.Q_grid_1h.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_Q_grid_1h[1], Grid.Q_grid_1h.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_Q_grid_1h[2], Grid.Q_grid_1h.c);
//    Grid_filter.Q_grid_1h.a = Grid_filter_params.CIC1_Q_grid_1h[0].out;
//    Grid_filter.Q_grid_1h.b = Grid_filter_params.CIC1_Q_grid_1h[1].out;
//    Grid_filter.Q_grid_1h.c = Grid_filter_params.CIC1_Q_grid_1h[2].out;
//    Grid_filter.average.Q_grid_1h = Grid_filter.Q_grid_1h.a + Grid_filter.Q_grid_1h.b + Grid_filter.Q_grid_1h.c;
//
//    Grid_filter.Q_load_1h.a = Grid_filter.Q_grid_1h.a + Grid_filter.Q_conv_1h.a;
//    Grid_filter.Q_load_1h.b = Grid_filter.Q_grid_1h.b + Grid_filter.Q_conv_1h.b;
//    Grid_filter.Q_load_1h.c = Grid_filter.Q_grid_1h.c + Grid_filter.Q_conv_1h.c;
//    Grid_filter.average.Q_load_1h = Grid_filter.Q_load_1h.a + Grid_filter.Q_load_1h.b + Grid_filter.Q_load_1h.c;
//
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_P_conv_1h[0], Grid.P_conv_1h.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_P_conv_1h[1], Grid.P_conv_1h.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_P_conv_1h[2], Grid.P_conv_1h.c);
//    Grid_filter.P_conv_1h.a = Grid_filter_params.CIC1_P_conv_1h[0].out;
//    Grid_filter.P_conv_1h.b = Grid_filter_params.CIC1_P_conv_1h[1].out;
//    Grid_filter.P_conv_1h.c = Grid_filter_params.CIC1_P_conv_1h[2].out;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_P_grid_1h[0], Grid.P_grid_1h.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_P_grid_1h[1], Grid.P_grid_1h.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_P_grid_1h[2], Grid.P_grid_1h.c);
//    Grid_filter.P_grid_1h.a = Grid_filter_params.CIC1_P_grid_1h[0].out;
//    Grid_filter.P_grid_1h.b = Grid_filter_params.CIC1_P_grid_1h[1].out;
//    Grid_filter.P_grid_1h.c = Grid_filter_params.CIC1_P_grid_1h[2].out;
//    Grid_filter.average.P_grid_1h = Grid_filter.P_grid_1h.a + Grid_filter.P_grid_1h.b + Grid_filter.P_grid_1h.c;
//
//    Grid_filter.P_load_1h.a = Grid_filter.P_grid_1h.a + Grid_filter.P_conv_1h.a;
//    Grid_filter.P_load_1h.b = Grid_filter.P_grid_1h.b + Grid_filter.P_conv_1h.b;
//    Grid_filter.P_load_1h.c = Grid_filter.P_grid_1h.c + Grid_filter.P_conv_1h.c;
//    Grid_filter.average.P_load_1h = Grid_filter.P_load_1h.a + Grid_filter.P_load_1h.b + Grid_filter.P_load_1h.c;
//
//    ///////////////////////////////////////////////////////////////////
//
//    Grid_filter.S_load_1h.a = sqrtf(Grid_filter.P_load_1h.a * Grid_filter.P_load_1h.a + Grid_filter.Q_load_1h.a * Grid_filter.Q_load_1h.a);
//    Grid_filter.S_load_1h.b = sqrtf(Grid_filter.P_load_1h.b * Grid_filter.P_load_1h.b + Grid_filter.Q_load_1h.b * Grid_filter.Q_load_1h.b);
//    Grid_filter.S_load_1h.c = sqrtf(Grid_filter.P_load_1h.c * Grid_filter.P_load_1h.c + Grid_filter.Q_load_1h.c * Grid_filter.Q_load_1h.c);
//    Grid_filter.average.S_load_1h = Grid_filter.S_load_1h.a + Grid_filter.S_load_1h.b + Grid_filter.S_load_1h.c;
//    Grid_filter.S_grid_1h.a = sqrtf(Grid_filter.P_grid_1h.a * Grid_filter.P_grid_1h.a + Grid_filter.Q_grid_1h.a * Grid_filter.Q_grid_1h.a);
//    Grid_filter.S_grid_1h.b = sqrtf(Grid_filter.P_grid_1h.b * Grid_filter.P_grid_1h.b + Grid_filter.Q_grid_1h.b * Grid_filter.Q_grid_1h.b);
//    Grid_filter.S_grid_1h.c = sqrtf(Grid_filter.P_grid_1h.c * Grid_filter.P_grid_1h.c + Grid_filter.Q_grid_1h.c * Grid_filter.Q_grid_1h.c);
//    Grid_filter.average.S_grid_1h = Grid_filter.S_grid_1h.a + Grid_filter.S_grid_1h.b + Grid_filter.S_grid_1h.c;
//    Grid_filter.S_conv_1h.a = sqrtf(Grid_filter.P_conv_1h.a * Grid_filter.P_conv_1h.a + Grid_filter.Q_conv_1h.a * Grid_filter.Q_conv_1h.a);
//    Grid_filter.S_conv_1h.b = sqrtf(Grid_filter.P_conv_1h.b * Grid_filter.P_conv_1h.b + Grid_filter.Q_conv_1h.b * Grid_filter.Q_conv_1h.b);
//    Grid_filter.S_conv_1h.c = sqrtf(Grid_filter.P_conv_1h.c * Grid_filter.P_conv_1h.c + Grid_filter.Q_conv_1h.c * Grid_filter.Q_conv_1h.c);
//
//    Grid_filter.S_grid.a = Grid_filter.U_grid.a * Grid_filter.I_grid.a;
//    Grid_filter.S_grid.b = Grid_filter.U_grid.b * Grid_filter.I_grid.b;
//    Grid_filter.S_grid.c = Grid_filter.U_grid.c * Grid_filter.I_grid.c;
//    Grid_filter.S_conv.a = Grid_filter.U_grid.a * Grid_filter.I_conv.a;
//    Grid_filter.S_conv.b = Grid_filter.U_grid.b * Grid_filter.I_conv.b;
//    Grid_filter.S_conv.c = Grid_filter.U_grid.c * Grid_filter.I_conv.c;
//
//    Grid_filter.PF_grid_1h.a = Grid_filter.P_grid_1h.a / fmaxf(Grid_filter.S_grid_1h.a, 1.0f);
//    Grid_filter.PF_grid_1h.b = Grid_filter.P_grid_1h.b / fmaxf(Grid_filter.S_grid_1h.b, 1.0f);
//    Grid_filter.PF_grid_1h.c = Grid_filter.P_grid_1h.c / fmaxf(Grid_filter.S_grid_1h.c, 1.0f);
//    Grid_filter.average.PF_grid_1h = (fabsf(Grid_filter.P_grid_1h.a) + fabsf(Grid_filter.P_grid_1h.b) + fabsf(Grid_filter.P_grid_1h.c)) / fmaxf(Grid_filter.average.S_grid_1h, 1.0f);
//
//    register float div_I_lim = 1.0f / fmaxf(Conv.I_lim_nominal, 1.0f);
//    Grid_filter.Used_resources.a = Grid_filter.I_conv.a * div_I_lim;
//    Grid_filter.Used_resources.b = Grid_filter.I_conv.b * div_I_lim;
//    Grid_filter.Used_resources.c = Grid_filter.I_conv.c * div_I_lim;
//    Grid_filter.Used_resources.n = Grid_filter.I_conv.n * div_I_lim;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_THD_U_grid[0], Grid.THD_U_grid.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_THD_U_grid[1], Grid.THD_U_grid.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_THD_U_grid[2], Grid.THD_U_grid.c);
//    Grid_filter.THD_U_grid.a = Grid_filter_params.CIC1_THD_U_grid[0].out;
//    Grid_filter.THD_U_grid.b = Grid_filter_params.CIC1_THD_U_grid[1].out;
//    Grid_filter.THD_U_grid.c = Grid_filter_params.CIC1_THD_U_grid[2].out;
//
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_THD_I_grid[0], Grid.THD_I_grid.a);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_THD_I_grid[1], Grid.THD_I_grid.b);
//    CIC1_filter_CPU(&Grid_filter_params.CIC1_THD_I_grid[2], Grid.THD_I_grid.c);
//    Grid_filter.THD_I_grid.a = Grid_filter_params.CIC1_THD_I_grid[0].out;
//    Grid_filter.THD_I_grid.b = Grid_filter_params.CIC1_THD_I_grid[1].out;
//    Grid_filter.THD_I_grid.c = Grid_filter_params.CIC1_THD_I_grid[2].out;
//}
