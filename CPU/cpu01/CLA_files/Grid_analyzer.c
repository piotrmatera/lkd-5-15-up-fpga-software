// Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

void Grid_analyzer_calc()
{
    Resonant_filter_calc_CLAasm(&Grid.Resonant_U_grid[0], Meas_master.U_grid.a);
    Resonant_filter_calc_CLAasm(&Grid.Resonant_U_grid[1], Meas_master.U_grid.b);
    Resonant_filter_calc_CLAasm(&Grid.Resonant_U_grid[2], Meas_master.U_grid.c);

    Resonant_filter_calc_CLAasm(&Grid.Resonant_I_grid[0], Meas_master.I_grid.a);
    Resonant_filter_calc_CLAasm(&Grid.Resonant_I_grid[1], Meas_master.I_grid.b);
    Resonant_filter_calc_CLAasm(&Grid.Resonant_I_grid[2], Meas_master.I_grid.c);

    Resonant_filter_calc_CLAasm(&Grid.Resonant_I_conv[0], Meas_master.I_conv.a);
    Resonant_filter_calc_CLAasm(&Grid.Resonant_I_conv[1], Meas_master.I_conv.b);
    Resonant_filter_calc_CLAasm(&Grid.Resonant_I_conv[2], Meas_master.I_conv.c);

    ///////////////////////////////////////////////////////////////////

    Grid.parameters.U_grid.a = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_U_grid[0], Meas_master.U_grid.a*Meas_master.U_grid.a));
    Grid.parameters.U_grid.b = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_U_grid[1], Meas_master.U_grid.b*Meas_master.U_grid.b));
    Grid.parameters.U_grid.c = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_U_grid[2], Meas_master.U_grid.c*Meas_master.U_grid.c));

    Grid.parameters.I_grid.a = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_grid[0], Meas_master.I_grid.a*Meas_master.I_grid.a));
    Grid.parameters.I_grid.b = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_grid[1], Meas_master.I_grid.b*Meas_master.I_grid.b));
    Grid.parameters.I_grid.c = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_grid[2], Meas_master.I_grid.c*Meas_master.I_grid.c));

    Grid.parameters.I_conv.a = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_conv[0], Meas_master.I_conv.a*Meas_master.I_conv.a));
    Grid.parameters.I_conv.b = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_conv[1], Meas_master.I_conv.b*Meas_master.I_conv.b));
    Grid.parameters.I_conv.c = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_conv[2], Meas_master.I_conv.c*Meas_master.I_conv.c));
    Grid.parameters.I_conv.n = sqrtf(CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_conv[3], Meas_master.I_conv.n*Meas_master.I_conv.n));

    ///////////////////////////////////////////////////////////////////

    Grid.parameters.U_grid_1h.a = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_U_grid_1h[0], sqrtf(0.5f * (Grid.Resonant_U_grid[0].x0 * Grid.Resonant_U_grid[0].x0 + Grid.Resonant_U_grid[0].x1 * Grid.Resonant_U_grid[0].x1)) );
    Grid.parameters.U_grid_1h.b = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_U_grid_1h[1], sqrtf(0.5f * (Grid.Resonant_U_grid[1].x0 * Grid.Resonant_U_grid[1].x0 + Grid.Resonant_U_grid[1].x1 * Grid.Resonant_U_grid[1].x1)) );
    Grid.parameters.U_grid_1h.c = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_U_grid_1h[2], sqrtf(0.5f * (Grid.Resonant_U_grid[2].x0 * Grid.Resonant_U_grid[2].x0 + Grid.Resonant_U_grid[2].x1 * Grid.Resonant_U_grid[2].x1)) );
    Grid.parameters.average.U_grid_1h = (Grid.parameters.U_grid_1h.a + Grid.parameters.U_grid_1h.b + Grid.parameters.U_grid_1h.c) * MATH_1_3;

    Grid.parameters.I_grid_1h.a = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_grid_1h[0], sqrtf(0.5f * (Grid.Resonant_I_grid[0].x0 * Grid.Resonant_I_grid[0].x0 + Grid.Resonant_I_grid[0].x1 * Grid.Resonant_I_grid[0].x1)) );
    Grid.parameters.I_grid_1h.b = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_grid_1h[1], sqrtf(0.5f * (Grid.Resonant_I_grid[1].x0 * Grid.Resonant_I_grid[1].x0 + Grid.Resonant_I_grid[1].x1 * Grid.Resonant_I_grid[1].x1)) );
    Grid.parameters.I_grid_1h.c = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_I_grid_1h[2], sqrtf(0.5f * (Grid.Resonant_I_grid[2].x0 * Grid.Resonant_I_grid[2].x0 + Grid.Resonant_I_grid[2].x1 * Grid.Resonant_I_grid[2].x1)) );

    ///////////////////////////////////////////////////////////////////

    static struct abc_struct Q_conv_temp;
    Q_conv_temp.a = 0.5f*(Grid.Resonant_U_grid[0].x1 * Grid.Resonant_I_conv[0].y0 - Grid.Resonant_U_grid[0].x0 * Grid.Resonant_I_conv[0].y1);
    Q_conv_temp.b = 0.5f*(Grid.Resonant_U_grid[1].x1 * Grid.Resonant_I_conv[1].y0 - Grid.Resonant_U_grid[1].x0 * Grid.Resonant_I_conv[1].y1);
    Q_conv_temp.c = 0.5f*(Grid.Resonant_U_grid[2].x1 * Grid.Resonant_I_conv[2].y0 - Grid.Resonant_U_grid[2].x0 * Grid.Resonant_I_conv[2].y1);
    Grid.parameters.Q_conv_1h.a = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_Q_conv_1h[0], Q_conv_temp.a);
    Grid.parameters.Q_conv_1h.b = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_Q_conv_1h[1], Q_conv_temp.b);
    Grid.parameters.Q_conv_1h.c = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_Q_conv_1h[2], Q_conv_temp.c);

    static struct abc_struct Q_grid_temp;
    Q_grid_temp.a = 0.5f*(Grid.Resonant_U_grid[0].x1 * Grid.Resonant_I_grid[0].y0 - Grid.Resonant_U_grid[0].x0 * Grid.Resonant_I_grid[0].y1);
    Q_grid_temp.b = 0.5f*(Grid.Resonant_U_grid[1].x1 * Grid.Resonant_I_grid[1].y0 - Grid.Resonant_U_grid[1].x0 * Grid.Resonant_I_grid[1].y1);
    Q_grid_temp.c = 0.5f*(Grid.Resonant_U_grid[2].x1 * Grid.Resonant_I_grid[2].y0 - Grid.Resonant_U_grid[2].x0 * Grid.Resonant_I_grid[2].y1);
    Grid.parameters.Q_grid_1h.a = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_Q_grid_1h[0], Q_grid_temp.a);
    Grid.parameters.Q_grid_1h.b = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_Q_grid_1h[1], Q_grid_temp.b);
    Grid.parameters.Q_grid_1h.c = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_Q_grid_1h[2], Q_grid_temp.c);
    Grid.parameters.average.Q_grid_1h = Grid.parameters.Q_grid_1h.a + Grid.parameters.Q_grid_1h.b + Grid.parameters.Q_grid_1h.c;

    Grid.parameters.Q_load_1h.a = Grid.parameters.Q_grid_1h.a + Grid.parameters.Q_conv_1h.a;
    Grid.parameters.Q_load_1h.b = Grid.parameters.Q_grid_1h.b + Grid.parameters.Q_conv_1h.b;
    Grid.parameters.Q_load_1h.c = Grid.parameters.Q_grid_1h.c + Grid.parameters.Q_conv_1h.c;
    Grid.parameters.average.Q_load_1h = Grid.parameters.Q_load_1h.a + Grid.parameters.Q_load_1h.b + Grid.parameters.Q_load_1h.c;


    static struct abc_struct P_conv_temp;
    P_conv_temp.a = 0.5f*(Grid.Resonant_U_grid[0].x0 * Grid.Resonant_I_conv[0].y0 + Grid.Resonant_U_grid[0].x1 * Grid.Resonant_I_conv[0].y1);
    P_conv_temp.b = 0.5f*(Grid.Resonant_U_grid[1].x0 * Grid.Resonant_I_conv[1].y0 + Grid.Resonant_U_grid[1].x1 * Grid.Resonant_I_conv[1].y1);
    P_conv_temp.c = 0.5f*(Grid.Resonant_U_grid[2].x0 * Grid.Resonant_I_conv[2].y0 + Grid.Resonant_U_grid[2].x1 * Grid.Resonant_I_conv[2].y1);
    Grid.parameters.P_conv_1h.a = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_P_conv_1h[0], P_conv_temp.a);
    Grid.parameters.P_conv_1h.b = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_P_conv_1h[1], P_conv_temp.b);
    Grid.parameters.P_conv_1h.c = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_P_conv_1h[2], P_conv_temp.c);

    static struct abc_struct P_grid_temp;
    P_grid_temp.a = 0.5f*(Grid.Resonant_U_grid[0].x0 * Grid.Resonant_I_grid[0].y0 + Grid.Resonant_U_grid[0].x1 * Grid.Resonant_I_grid[0].y1);
    P_grid_temp.b = 0.5f*(Grid.Resonant_U_grid[1].x0 * Grid.Resonant_I_grid[1].y0 + Grid.Resonant_U_grid[1].x1 * Grid.Resonant_I_grid[1].y1);
    P_grid_temp.c = 0.5f*(Grid.Resonant_U_grid[2].x0 * Grid.Resonant_I_grid[2].y0 + Grid.Resonant_U_grid[2].x1 * Grid.Resonant_I_grid[2].y1);
    Grid.parameters.P_grid_1h.a = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_P_grid_1h[0], P_grid_temp.a);
    Grid.parameters.P_grid_1h.b = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_P_grid_1h[1], P_grid_temp.b);
    Grid.parameters.P_grid_1h.c = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Grid.CIC1_P_grid_1h[2], P_grid_temp.c);
    Grid.parameters.average.P_grid_1h = Grid.parameters.P_grid_1h.a + Grid.parameters.P_grid_1h.b + Grid.parameters.P_grid_1h.c;

    Grid.parameters.P_load_1h.a = Grid.parameters.P_grid_1h.a + Grid.parameters.P_conv_1h.a;
    Grid.parameters.P_load_1h.b = Grid.parameters.P_grid_1h.b + Grid.parameters.P_conv_1h.b;
    Grid.parameters.P_load_1h.c = Grid.parameters.P_grid_1h.c + Grid.parameters.P_conv_1h.c;
    Grid.parameters.average.P_load_1h = Grid.parameters.P_load_1h.a + Grid.parameters.P_load_1h.b + Grid.parameters.P_load_1h.c;

    ///////////////////////////////////////////////////////////////////

    Grid.parameters.S_load_1h.a = sqrtf(Grid.parameters.P_load_1h.a * Grid.parameters.P_load_1h.a + Grid.parameters.Q_load_1h.a * Grid.parameters.Q_load_1h.a);
    Grid.parameters.S_load_1h.b = sqrtf(Grid.parameters.P_load_1h.b * Grid.parameters.P_load_1h.b + Grid.parameters.Q_load_1h.b * Grid.parameters.Q_load_1h.b);
    Grid.parameters.S_load_1h.c = sqrtf(Grid.parameters.P_load_1h.c * Grid.parameters.P_load_1h.c + Grid.parameters.Q_load_1h.c * Grid.parameters.Q_load_1h.c);
    Grid.parameters.average.S_load_1h = Grid.parameters.S_load_1h.a + Grid.parameters.S_load_1h.b + Grid.parameters.S_load_1h.c;
    Grid.parameters.S_grid_1h.a = sqrtf(Grid.parameters.P_grid_1h.a * Grid.parameters.P_grid_1h.a + Grid.parameters.Q_grid_1h.a * Grid.parameters.Q_grid_1h.a);
    Grid.parameters.S_grid_1h.b = sqrtf(Grid.parameters.P_grid_1h.b * Grid.parameters.P_grid_1h.b + Grid.parameters.Q_grid_1h.b * Grid.parameters.Q_grid_1h.b);
    Grid.parameters.S_grid_1h.c = sqrtf(Grid.parameters.P_grid_1h.c * Grid.parameters.P_grid_1h.c + Grid.parameters.Q_grid_1h.c * Grid.parameters.Q_grid_1h.c);
    Grid.parameters.average.S_grid_1h = Grid.parameters.S_grid_1h.a + Grid.parameters.S_grid_1h.b + Grid.parameters.S_grid_1h.c;
    Grid.parameters.S_conv_1h.a = sqrtf(Grid.parameters.P_conv_1h.a * Grid.parameters.P_conv_1h.a + Grid.parameters.Q_conv_1h.a * Grid.parameters.Q_conv_1h.a);
    Grid.parameters.S_conv_1h.b = sqrtf(Grid.parameters.P_conv_1h.b * Grid.parameters.P_conv_1h.b + Grid.parameters.Q_conv_1h.b * Grid.parameters.Q_conv_1h.b);
    Grid.parameters.S_conv_1h.c = sqrtf(Grid.parameters.P_conv_1h.c * Grid.parameters.P_conv_1h.c + Grid.parameters.Q_conv_1h.c * Grid.parameters.Q_conv_1h.c);

    Grid.parameters.S_grid.a = Grid.parameters.U_grid.a * Grid.parameters.I_grid.a;
    Grid.parameters.S_grid.b = Grid.parameters.U_grid.b * Grid.parameters.I_grid.b;
    Grid.parameters.S_grid.c = Grid.parameters.U_grid.c * Grid.parameters.I_grid.c;
    Grid.parameters.S_conv.a = Grid.parameters.U_grid.a * Grid.parameters.I_conv.a;
    Grid.parameters.S_conv.b = Grid.parameters.U_grid.b * Grid.parameters.I_conv.b;
    Grid.parameters.S_conv.c = Grid.parameters.U_grid.c * Grid.parameters.I_conv.c;

    Grid.parameters.PF_grid_1h.a = Grid.parameters.P_grid_1h.a / fmaxf(Grid.parameters.S_grid_1h.a, 1.0f);
    Grid.parameters.PF_grid_1h.b = Grid.parameters.P_grid_1h.b / fmaxf(Grid.parameters.S_grid_1h.b, 1.0f);
    Grid.parameters.PF_grid_1h.c = Grid.parameters.P_grid_1h.c / fmaxf(Grid.parameters.S_grid_1h.c, 1.0f);
    Grid.parameters.average.PF_grid_1h = (fabsf(Grid.parameters.P_grid_1h.a) + fabsf(Grid.parameters.P_grid_1h.b) + fabsf(Grid.parameters.P_grid_1h.c)) / fmaxf(Grid.parameters.average.S_grid_1h, 1.0f);

    register float div_I_lim = 1.0f / fmaxf(Conv.I_lim_nominal, 1.0f);
    Grid.parameters.Used_resources.a = Grid.parameters.I_conv.a * div_I_lim;
    Grid.parameters.Used_resources.b = Grid.parameters.I_conv.b * div_I_lim;
    Grid.parameters.Used_resources.c = Grid.parameters.I_conv.c * div_I_lim;
    Grid.parameters.Used_resources.n = Grid.parameters.I_conv.n * div_I_lim;

    register Uint32 float_one = 0x3f800000;
    register Uint32 sign_bit = 0x80000000;
    static volatile union {float f32; Uint32 u32;}sign_temp[4];
    sign_temp[0].u32 = float_one | (*(Uint32 *)&Grid.parameters.P_grid_1h.a & sign_bit);
    sign_temp[1].u32 = float_one | (*(Uint32 *)&Grid.parameters.P_grid_1h.b & sign_bit);
    sign_temp[2].u32 = float_one | (*(Uint32 *)&Grid.parameters.P_grid_1h.c & sign_bit);
    sign_temp[3].u32 = float_one | (*(Uint32 *)&Grid.parameters.average.P_grid_1h & sign_bit);

    register float gain = Grid.Accumulator_gain;
    register float temp;
    register float sign;
    sign = sign_temp[0].f32 * gain;
    Grid.input_P_p[0] = fabsf(gain * fmaxf(Grid.parameters.P_grid_1h.a, 0.0f));
    Grid.input_P_n[0] = fabsf(gain * fminf(Grid.parameters.P_grid_1h.a, 0.0f));
    temp = sign * fmaxf(Grid.parameters.Q_grid_1h.a, 0.0f);
    Grid.input_QI[0] = fabsf(fmaxf(temp, 0.0f));
    Grid.input_QII[0] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.parameters.Q_grid_1h.a, 0.0f);
    Grid.input_QIII[0] = fabsf(fmaxf(temp, 0.0f));
    Grid.input_QIV[0] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[1].f32 * gain;
    Grid.input_P_p[1] = fabsf(gain * fmaxf(Grid.parameters.P_grid_1h.b, 0.0f));
    Grid.input_P_n[1] = fabsf(gain * fminf(Grid.parameters.P_grid_1h.b, 0.0f));
    temp = sign * fmaxf(Grid.parameters.Q_grid_1h.b, 0.0f);
    Grid.input_QI[1] = fabsf(fmaxf(temp, 0.0f));
    Grid.input_QII[1] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.parameters.Q_grid_1h.b, 0.0f);
    Grid.input_QIII[1] = fabsf(fmaxf(temp, 0.0f));
    Grid.input_QIV[1] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[2].f32 * gain;
    Grid.input_P_p[2] = fabsf(gain * fmaxf(Grid.parameters.P_grid_1h.c, 0.0f));
    Grid.input_P_n[2] = fabsf(gain * fminf(Grid.parameters.P_grid_1h.c, 0.0f));
    temp = sign * fmaxf(Grid.parameters.Q_grid_1h.c, 0.0f);
    Grid.input_QI[2] = fabsf(fmaxf(temp, 0.0f));
    Grid.input_QII[2] = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.parameters.Q_grid_1h.c, 0.0f);
    Grid.input_QIII[2] = fabsf(fmaxf(temp, 0.0f));
    Grid.input_QIV[2] = fabsf(fminf(temp, 0.0f));

    sign = sign_temp[3].f32 * gain;
    Grid.sum.input_P_p = fabsf(gain * fmaxf(Grid.parameters.average.P_grid_1h, 0.0f));
    Grid.sum.input_P_n = fabsf(gain * fminf(Grid.parameters.average.P_grid_1h, 0.0f));
    temp = sign * fmaxf(Grid.parameters.average.Q_grid_1h, 0.0f);
    Grid.sum.input_QI = fabsf(fmaxf(temp, 0.0f));
    Grid.sum.input_QII = fabsf(fminf(temp, 0.0f));
    temp = sign * fminf(Grid.parameters.average.Q_grid_1h, 0.0f);
    Grid.sum.input_QIII = fabsf(fmaxf(temp, 0.0f));
    Grid.sum.input_QIV = fabsf(fminf(temp, 0.0f));
}

void Grid_analyzer_filter_calc()
{
    CIC1_filter_CLAasm(&Grid_filter.CIC1_U_grid[0], Grid.parameters.U_grid.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_U_grid[1], Grid.parameters.U_grid.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_U_grid[2], Grid.parameters.U_grid.c);
    Grid_filter.parameters.U_grid.a = Grid_filter.CIC1_U_grid[0].out;
    Grid_filter.parameters.U_grid.b = Grid_filter.CIC1_U_grid[1].out;
    Grid_filter.parameters.U_grid.c = Grid_filter.CIC1_U_grid[2].out;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_grid[0], Grid.parameters.I_grid.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_grid[1], Grid.parameters.I_grid.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_grid[2], Grid.parameters.I_grid.c);
    Grid_filter.parameters.I_grid.a = Grid_filter.CIC1_I_grid[0].out;
    Grid_filter.parameters.I_grid.b = Grid_filter.CIC1_I_grid[1].out;
    Grid_filter.parameters.I_grid.c = Grid_filter.CIC1_I_grid[2].out;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_conv[0], Grid.parameters.I_conv.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_conv[1], Grid.parameters.I_conv.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_conv[2], Grid.parameters.I_conv.c);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_conv[3], Grid.parameters.I_conv.n);
    Grid_filter.parameters.I_conv.a = Grid_filter.CIC1_I_conv[0].out;
    Grid_filter.parameters.I_conv.b = Grid_filter.CIC1_I_conv[1].out;
    Grid_filter.parameters.I_conv.c = Grid_filter.CIC1_I_conv[2].out;
    Grid_filter.parameters.I_conv.n = Grid_filter.CIC1_I_conv[3].out;

    ///////////////////////////////////////////////////////////////////

    CIC1_filter_CLAasm(&Grid_filter.CIC1_U_grid_1h[0], Grid.parameters.U_grid_1h.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_U_grid_1h[1], Grid.parameters.U_grid_1h.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_U_grid_1h[2], Grid.parameters.U_grid_1h.c);
    Grid_filter.parameters.U_grid_1h.a = Grid_filter.CIC1_U_grid_1h[0].out;
    Grid_filter.parameters.U_grid_1h.b = Grid_filter.CIC1_U_grid_1h[1].out;
    Grid_filter.parameters.U_grid_1h.c = Grid_filter.CIC1_U_grid_1h[2].out;
    Grid_filter.parameters.average.U_grid_1h = (Grid_filter.parameters.U_grid_1h.a + Grid_filter.parameters.U_grid_1h.b + Grid_filter.parameters.U_grid_1h.c) * MATH_1_3;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_grid_1h[0], Grid.parameters.I_grid_1h.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_grid_1h[1], Grid.parameters.I_grid_1h.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_I_grid_1h[2], Grid.parameters.I_grid_1h.c);
    Grid_filter.parameters.I_grid_1h.a = Grid_filter.CIC1_I_grid_1h[0].out;
    Grid_filter.parameters.I_grid_1h.b = Grid_filter.CIC1_I_grid_1h[1].out;
    Grid_filter.parameters.I_grid_1h.c = Grid_filter.CIC1_I_grid_1h[2].out;

    ///////////////////////////////////////////////////////////////////

    CIC1_filter_CLAasm(&Grid_filter.CIC1_Q_conv_1h[0], Grid.parameters.Q_conv_1h.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_Q_conv_1h[1], Grid.parameters.Q_conv_1h.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_Q_conv_1h[2], Grid.parameters.Q_conv_1h.c);
    Grid_filter.parameters.Q_conv_1h.a = Grid_filter.CIC1_Q_conv_1h[0].out;
    Grid_filter.parameters.Q_conv_1h.b = Grid_filter.CIC1_Q_conv_1h[1].out;
    Grid_filter.parameters.Q_conv_1h.c = Grid_filter.CIC1_Q_conv_1h[2].out;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_Q_grid_1h[0], Grid.parameters.Q_grid_1h.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_Q_grid_1h[1], Grid.parameters.Q_grid_1h.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_Q_grid_1h[2], Grid.parameters.Q_grid_1h.c);
    Grid_filter.parameters.Q_grid_1h.a = Grid_filter.CIC1_Q_grid_1h[0].out;
    Grid_filter.parameters.Q_grid_1h.b = Grid_filter.CIC1_Q_grid_1h[1].out;
    Grid_filter.parameters.Q_grid_1h.c = Grid_filter.CIC1_Q_grid_1h[2].out;
    Grid_filter.parameters.average.Q_grid_1h = Grid_filter.parameters.Q_grid_1h.a + Grid_filter.parameters.Q_grid_1h.b + Grid_filter.parameters.Q_grid_1h.c;

    Grid_filter.parameters.Q_load_1h.a = Grid_filter.parameters.Q_grid_1h.a + Grid_filter.parameters.Q_conv_1h.a;
    Grid_filter.parameters.Q_load_1h.b = Grid_filter.parameters.Q_grid_1h.b + Grid_filter.parameters.Q_conv_1h.b;
    Grid_filter.parameters.Q_load_1h.c = Grid_filter.parameters.Q_grid_1h.c + Grid_filter.parameters.Q_conv_1h.c;
    Grid_filter.parameters.average.Q_load_1h = Grid_filter.parameters.Q_load_1h.a + Grid_filter.parameters.Q_load_1h.b + Grid_filter.parameters.Q_load_1h.c;


    CIC1_filter_CLAasm(&Grid_filter.CIC1_P_conv_1h[0], Grid.parameters.P_conv_1h.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_P_conv_1h[1], Grid.parameters.P_conv_1h.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_P_conv_1h[2], Grid.parameters.P_conv_1h.c);
    Grid_filter.parameters.P_conv_1h.a = Grid_filter.CIC1_P_conv_1h[0].out;
    Grid_filter.parameters.P_conv_1h.b = Grid_filter.CIC1_P_conv_1h[1].out;
    Grid_filter.parameters.P_conv_1h.c = Grid_filter.CIC1_P_conv_1h[2].out;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_P_grid_1h[0], Grid.parameters.P_grid_1h.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_P_grid_1h[1], Grid.parameters.P_grid_1h.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_P_grid_1h[2], Grid.parameters.P_grid_1h.c);
    Grid_filter.parameters.P_grid_1h.a = Grid_filter.CIC1_P_grid_1h[0].out;
    Grid_filter.parameters.P_grid_1h.b = Grid_filter.CIC1_P_grid_1h[1].out;
    Grid_filter.parameters.P_grid_1h.c = Grid_filter.CIC1_P_grid_1h[2].out;
    Grid_filter.parameters.average.P_grid_1h = Grid_filter.parameters.P_grid_1h.a + Grid_filter.parameters.P_grid_1h.b + Grid_filter.parameters.P_grid_1h.c;

    Grid_filter.parameters.P_load_1h.a = Grid_filter.parameters.P_grid_1h.a + Grid_filter.parameters.P_conv_1h.a;
    Grid_filter.parameters.P_load_1h.b = Grid_filter.parameters.P_grid_1h.b + Grid_filter.parameters.P_conv_1h.b;
    Grid_filter.parameters.P_load_1h.c = Grid_filter.parameters.P_grid_1h.c + Grid_filter.parameters.P_conv_1h.c;
    Grid_filter.parameters.average.P_load_1h = Grid_filter.parameters.P_load_1h.a + Grid_filter.parameters.P_load_1h.b + Grid_filter.parameters.P_load_1h.c;

    ///////////////////////////////////////////////////////////////////

    Grid_filter.parameters.S_load_1h.a = sqrtf(Grid_filter.parameters.P_load_1h.a * Grid_filter.parameters.P_load_1h.a + Grid_filter.parameters.Q_load_1h.a * Grid_filter.parameters.Q_load_1h.a);
    Grid_filter.parameters.S_load_1h.b = sqrtf(Grid_filter.parameters.P_load_1h.b * Grid_filter.parameters.P_load_1h.b + Grid_filter.parameters.Q_load_1h.b * Grid_filter.parameters.Q_load_1h.b);
    Grid_filter.parameters.S_load_1h.c = sqrtf(Grid_filter.parameters.P_load_1h.c * Grid_filter.parameters.P_load_1h.c + Grid_filter.parameters.Q_load_1h.c * Grid_filter.parameters.Q_load_1h.c);
    Grid_filter.parameters.average.S_load_1h = Grid_filter.parameters.S_load_1h.a + Grid_filter.parameters.S_load_1h.b + Grid_filter.parameters.S_load_1h.c;
    Grid_filter.parameters.S_grid_1h.a = sqrtf(Grid_filter.parameters.P_grid_1h.a * Grid_filter.parameters.P_grid_1h.a + Grid_filter.parameters.Q_grid_1h.a * Grid_filter.parameters.Q_grid_1h.a);
    Grid_filter.parameters.S_grid_1h.b = sqrtf(Grid_filter.parameters.P_grid_1h.b * Grid_filter.parameters.P_grid_1h.b + Grid_filter.parameters.Q_grid_1h.b * Grid_filter.parameters.Q_grid_1h.b);
    Grid_filter.parameters.S_grid_1h.c = sqrtf(Grid_filter.parameters.P_grid_1h.c * Grid_filter.parameters.P_grid_1h.c + Grid_filter.parameters.Q_grid_1h.c * Grid_filter.parameters.Q_grid_1h.c);
    Grid_filter.parameters.average.S_grid_1h = Grid_filter.parameters.S_grid_1h.a + Grid_filter.parameters.S_grid_1h.b + Grid_filter.parameters.S_grid_1h.c;
    Grid_filter.parameters.S_conv_1h.a = sqrtf(Grid_filter.parameters.P_conv_1h.a * Grid_filter.parameters.P_conv_1h.a + Grid_filter.parameters.Q_conv_1h.a * Grid_filter.parameters.Q_conv_1h.a);
    Grid_filter.parameters.S_conv_1h.b = sqrtf(Grid_filter.parameters.P_conv_1h.b * Grid_filter.parameters.P_conv_1h.b + Grid_filter.parameters.Q_conv_1h.b * Grid_filter.parameters.Q_conv_1h.b);
    Grid_filter.parameters.S_conv_1h.c = sqrtf(Grid_filter.parameters.P_conv_1h.c * Grid_filter.parameters.P_conv_1h.c + Grid_filter.parameters.Q_conv_1h.c * Grid_filter.parameters.Q_conv_1h.c);

    Grid_filter.parameters.S_grid.a = Grid_filter.parameters.U_grid.a * Grid_filter.parameters.I_grid.a;
    Grid_filter.parameters.S_grid.b = Grid_filter.parameters.U_grid.b * Grid_filter.parameters.I_grid.b;
    Grid_filter.parameters.S_grid.c = Grid_filter.parameters.U_grid.c * Grid_filter.parameters.I_grid.c;
    Grid_filter.parameters.S_conv.a = Grid_filter.parameters.U_grid.a * Grid_filter.parameters.I_conv.a;
    Grid_filter.parameters.S_conv.b = Grid_filter.parameters.U_grid.b * Grid_filter.parameters.I_conv.b;
    Grid_filter.parameters.S_conv.c = Grid_filter.parameters.U_grid.c * Grid_filter.parameters.I_conv.c;

    Grid_filter.parameters.PF_grid_1h.a = Grid_filter.parameters.P_grid_1h.a / fmaxf(Grid_filter.parameters.S_grid_1h.a, 1.0f);
    Grid_filter.parameters.PF_grid_1h.b = Grid_filter.parameters.P_grid_1h.b / fmaxf(Grid_filter.parameters.S_grid_1h.b, 1.0f);
    Grid_filter.parameters.PF_grid_1h.c = Grid_filter.parameters.P_grid_1h.c / fmaxf(Grid_filter.parameters.S_grid_1h.c, 1.0f);
    Grid_filter.parameters.average.PF_grid_1h = (fabsf(Grid_filter.parameters.P_grid_1h.a) + fabsf(Grid_filter.parameters.P_grid_1h.b) + fabsf(Grid_filter.parameters.P_grid_1h.c)) / fmaxf(Grid_filter.parameters.average.S_grid_1h, 1.0f);

    register float div_I_lim = 1.0f / fmaxf(Conv.I_lim_nominal, 1.0f);
    Grid_filter.parameters.Used_resources.a = Grid_filter.parameters.I_conv.a * div_I_lim;
    Grid_filter.parameters.Used_resources.b = Grid_filter.parameters.I_conv.b * div_I_lim;
    Grid_filter.parameters.Used_resources.c = Grid_filter.parameters.I_conv.c * div_I_lim;
    Grid_filter.parameters.Used_resources.n = Grid_filter.parameters.I_conv.n * div_I_lim;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_THD_U_grid[0], Grid.parameters.THD_U_grid.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_THD_U_grid[1], Grid.parameters.THD_U_grid.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_THD_U_grid[2], Grid.parameters.THD_U_grid.c);
    Grid_filter.parameters.THD_U_grid.a = Grid_filter.CIC1_THD_U_grid[0].out;
    Grid_filter.parameters.THD_U_grid.b = Grid_filter.CIC1_THD_U_grid[1].out;
    Grid_filter.parameters.THD_U_grid.c = Grid_filter.CIC1_THD_U_grid[2].out;

    CIC1_filter_CLAasm(&Grid_filter.CIC1_THD_I_grid[0], Grid.parameters.THD_I_grid.a);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_THD_I_grid[1], Grid.parameters.THD_I_grid.b);
    CIC1_filter_CLAasm(&Grid_filter.CIC1_THD_I_grid[2], Grid.parameters.THD_I_grid.c);
    Grid_filter.parameters.THD_I_grid.a = Grid_filter.CIC1_THD_I_grid[0].out;
    Grid_filter.parameters.THD_I_grid.b = Grid_filter.CIC1_THD_I_grid[1].out;
    Grid_filter.parameters.THD_I_grid.c = Grid_filter.CIC1_THD_I_grid[2].out;

}
