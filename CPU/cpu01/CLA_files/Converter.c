// Tomasz Święchowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

void Converter_calc()
{
    Filter1_calc_CLAasm(&Conv.I_lim_slave_prefilter[0], Conv.I_lim_slave[0]);
    Filter1_calc_CLAasm(&Conv.I_lim_slave_prefilter[1], Conv.I_lim_slave[1]);
    Filter1_calc_CLAasm(&Conv.I_lim_slave_prefilter[2], Conv.I_lim_slave[2]);
    Filter1_calc_CLAasm(&Conv.I_lim_slave_prefilter[3], Conv.I_lim_slave[3]);
    if(!Conv.I_lim_slave[0]) Conv.I_lim_slave_prefilter[0].out = 0;
    if(!Conv.I_lim_slave[1]) Conv.I_lim_slave_prefilter[1].out = 0;
    if(!Conv.I_lim_slave[2]) Conv.I_lim_slave_prefilter[2].out = 0;
    if(!Conv.I_lim_slave[3]) Conv.I_lim_slave_prefilter[3].out = 0;

    Conv.I_lim_avg = Conv.I_lim_slave[0] + Conv.I_lim_slave[1] + Conv.I_lim_slave[2] + Conv.I_lim_slave[3];
    Conv.I_lim_avg_prefilter = Conv.I_lim_slave_prefilter[0].out + Conv.I_lim_slave_prefilter[1].out + Conv.I_lim_slave_prefilter[2].out + Conv.I_lim_slave_prefilter[3].out;

    //////////////////////////////////////////////////////////////////////////////////

    struct abc_struct U_grid_1h_div;
    U_grid_1h_div.a = 1.0f / fmaxf(CLA2toCLA1.Grid.U_grid_1h.a, 1.0f);
    U_grid_1h_div.b = 1.0f / fmaxf(CLA2toCLA1.Grid.U_grid_1h.b, 1.0f);
    U_grid_1h_div.c = 1.0f / fmaxf(CLA2toCLA1.Grid.U_grid_1h.c, 1.0f);

    Conv.id_conv.a = CLA2toCLA1.Grid.P_conv_1h.a * U_grid_1h_div.a;
    Conv.id_conv.b = CLA2toCLA1.Grid.P_conv_1h.b * U_grid_1h_div.b;
    Conv.id_conv.c = CLA2toCLA1.Grid.P_conv_1h.c * U_grid_1h_div.c;

    Conv.iq_conv.a = CLA2toCLA1.Grid.Q_conv_1h.a * U_grid_1h_div.a;
    Conv.iq_conv.b = CLA2toCLA1.Grid.Q_conv_1h.b * U_grid_1h_div.b;
    Conv.iq_conv.c = CLA2toCLA1.Grid.Q_conv_1h.c * U_grid_1h_div.c;

    Conv.id_load.a = CLA2toCLA1.Grid.P_load_1h.a * U_grid_1h_div.a;
    Conv.id_load.b = CLA2toCLA1.Grid.P_load_1h.b * U_grid_1h_div.b;
    Conv.id_load.c = CLA2toCLA1.Grid.P_load_1h.c * U_grid_1h_div.c;

    Conv.iq_load.a = CLA2toCLA1.Grid.Q_load_1h.a * U_grid_1h_div.a;
    Conv.iq_load.b = CLA2toCLA1.Grid.Q_load_1h.b * U_grid_1h_div.b;
    Conv.iq_load.c = CLA2toCLA1.Grid.Q_load_1h.c * U_grid_1h_div.c;

    Filter1_calc_CLAasm(&Conv.version_Q_comp_local_prefilter.a, Conv.version_Q_comp_local.a);
    Filter1_calc_CLAasm(&Conv.version_Q_comp_local_prefilter.b, Conv.version_Q_comp_local.b);
    Filter1_calc_CLAasm(&Conv.version_Q_comp_local_prefilter.c, Conv.version_Q_comp_local.c);

    Filter1_calc_CLAasm(&Conv.enable_Q_comp_local_prefilter.a, Conv.enable_Q_comp_local.a);
    Filter1_calc_CLAasm(&Conv.enable_Q_comp_local_prefilter.b, Conv.enable_Q_comp_local.b);
    Filter1_calc_CLAasm(&Conv.enable_Q_comp_local_prefilter.c, Conv.enable_Q_comp_local.c);

    Filter1_calc_CLAasm(&Conv.Q_set_local_prefilter.a, Conv.Q_set_local.a);
    Filter1_calc_CLAasm(&Conv.Q_set_local_prefilter.b, Conv.Q_set_local.b);
    Filter1_calc_CLAasm(&Conv.Q_set_local_prefilter.c, Conv.Q_set_local.c);

    Filter1_calc_CLAasm(&Conv.enable_P_sym_local_prefilter, Conv.enable_P_sym_local);
    Filter1_calc_CLAasm(&Conv.version_P_sym_local_prefilter, Conv.version_P_sym_local);

    Filter1_calc_CLAasm(&Conv.tangens_range_local_prefilter[0].a, Conv.tangens_range_local[0].a);
    Filter1_calc_CLAasm(&Conv.tangens_range_local_prefilter[0].b, Conv.tangens_range_local[0].b);
    Filter1_calc_CLAasm(&Conv.tangens_range_local_prefilter[0].c, Conv.tangens_range_local[0].c);

    Filter1_calc_CLAasm(&Conv.tangens_range_local_prefilter[1].a, Conv.tangens_range_local[1].a);
    Filter1_calc_CLAasm(&Conv.tangens_range_local_prefilter[1].b, Conv.tangens_range_local[1].b);
    Filter1_calc_CLAasm(&Conv.tangens_range_local_prefilter[1].c, Conv.tangens_range_local[1].c);

    struct abc_struct iq_tangens;
    if(Conv.tangens_range_local_prefilter[0].a.out > Conv.tangens_range_local_prefilter[1].a.out)
    {
        iq_tangens.a = fmaxf(Conv.iq_load.a - fabs(Conv.id_load.a) * Conv.tangens_range_local_prefilter[0].a.out, 0.0f)
                     + fminf(Conv.iq_load.a - fabs(Conv.id_load.a) * Conv.tangens_range_local_prefilter[1].a.out, 0.0f);
    }
    else
    {
        iq_tangens.a = fminf(Conv.iq_load.a - fabs(Conv.id_load.a) * Conv.tangens_range_local_prefilter[0].a.out, 0.0f)
                     + fmaxf(Conv.iq_load.a - fabs(Conv.id_load.a) * Conv.tangens_range_local_prefilter[1].a.out, 0.0f);
    }

    if(Conv.tangens_range_local_prefilter[0].b.out > Conv.tangens_range_local_prefilter[1].b.out)
    {
        iq_tangens.b = fmaxf(Conv.iq_load.b - fabs(Conv.id_load.b) * Conv.tangens_range_local_prefilter[0].b.out, 0.0f)
                     + fminf(Conv.iq_load.b - fabs(Conv.id_load.b) * Conv.tangens_range_local_prefilter[1].b.out, 0.0f);
    }
    else
    {
        iq_tangens.b = fminf(Conv.iq_load.b - fabs(Conv.id_load.b) * Conv.tangens_range_local_prefilter[0].b.out, 0.0f)
                     + fmaxf(Conv.iq_load.b - fabs(Conv.id_load.b) * Conv.tangens_range_local_prefilter[1].b.out, 0.0f);
    }

    if(Conv.tangens_range_local_prefilter[0].c.out > Conv.tangens_range_local_prefilter[1].c.out)
    {
        iq_tangens.c = fmaxf(Conv.iq_load.c - fabs(Conv.id_load.c) * Conv.tangens_range_local_prefilter[0].c.out, 0.0f)
                     + fminf(Conv.iq_load.c - fabs(Conv.id_load.c) * Conv.tangens_range_local_prefilter[1].c.out, 0.0f);
    }
    else
    {
        iq_tangens.c = fminf(Conv.iq_load.c - fabs(Conv.id_load.c) * Conv.tangens_range_local_prefilter[0].c.out, 0.0f)
                     + fmaxf(Conv.iq_load.c - fabs(Conv.id_load.c) * Conv.tangens_range_local_prefilter[1].c.out, 0.0f);
    }

    Conv.iq_load_ref.a = (iq_tangens.a * (1.0f - Conv.version_Q_comp_local_prefilter.a.out) - Conv.Q_set_local_prefilter.a.out * U_grid_1h_div.a) * Conv.enable_Q_comp_local_prefilter.a.out;
    Conv.iq_load_ref.b = (iq_tangens.b * (1.0f - Conv.version_Q_comp_local_prefilter.b.out) - Conv.Q_set_local_prefilter.b.out * U_grid_1h_div.b) * Conv.enable_Q_comp_local_prefilter.b.out;
    Conv.iq_load_ref.c = (iq_tangens.c * (1.0f - Conv.version_Q_comp_local_prefilter.c.out) - Conv.Q_set_local_prefilter.c.out * U_grid_1h_div.c) * Conv.enable_Q_comp_local_prefilter.c.out;

    struct abc_struct iq_lim_sat;
    register float I_lim_q = Conv.I_lim_avg_prefilter;
    register float I_lim_q_n = -I_lim_q;
    iq_lim_sat.a = Saturation(Conv.iq_load_ref.a, I_lim_q_n, I_lim_q);
    iq_lim_sat.b = Saturation(Conv.iq_load_ref.b, I_lim_q_n, I_lim_q);
    iq_lim_sat.c = Saturation(Conv.iq_load_ref.c, I_lim_q_n, I_lim_q);

    static struct abcn_struct iq_lim;
    register float rate = fmaxf(Conv.I_lim_avg_prefilter * 0.0016f, 24.0f * 0.0016f);
    register float rate_n = -rate;
    iq_lim.a += Saturation(iq_lim_sat.a - iq_lim.a, rate_n, rate);
    iq_lim.b += Saturation(iq_lim_sat.b - iq_lim.b, rate_n, rate);
    iq_lim.c += Saturation(iq_lim_sat.c - iq_lim.c, rate_n, rate);

    float not_in_limit = 1.0f;
    not_in_limit *= fabs(iq_lim.a) - I_lim_q;
    not_in_limit *= fabs(iq_lim.b) - I_lim_q;
    not_in_limit *= fabs(iq_lim.c) - I_lim_q;

    register float Iq_x = MATH_SQRT3_2 * (iq_lim.c - iq_lim.b);
    register float Iq_y = 0.5f*(iq_lim.b + iq_lim.c) - iq_lim.a;
    register float sign = PLL.sign;
    Conv.Iq_x = Iq_x * sign;
    Conv.Iq_y = Iq_y * sign;
    iq_lim.n = sqrtf(Iq_x * Iq_x + Iq_y * Iq_y);
    float not_in_limit_n = fminf(I_lim_q, iq_lim.n) - I_lim_q;
    register float ratio_q = Saturation(I_lim_q / fmaxf(I_lim_q, iq_lim.n), 0.0f, 1.0f);
    Conv.iq_lim.a = iq_lim.a * ratio_q;
    Conv.iq_lim.b = iq_lim.b * ratio_q;
    Conv.iq_lim.c = iq_lim.c * ratio_q;
    Conv.iq_lim.n = iq_lim.n * ratio_q;

    if(not_in_limit * not_in_limit_n == 0.0f && Conv.enable_Q_comp_local.a + Conv.enable_Q_comp_local.b + Conv.enable_Q_comp_local.c >= 1.0f) status_master.in_limit_Q = 1;
    else status_master.in_limit_Q = 0;

    //////////////////////////////////////////////////////////////////////////////////

    static struct abcn_struct id_lim;
    register float P_avg = (CLA2toCLA1.Grid.P_load_1h.a + CLA2toCLA1.Grid.P_load_1h.b + CLA2toCLA1.Grid.P_load_1h.c) * MATH_1_3;
    register float U_grid_1h_avg_div_ratio = (1.0f - Conv.version_P_sym_local_prefilter.out) / fmaxf(CLA2toCLA1.Grid.average.U_grid_1h, 1.0f);
    id_lim.a = Conv.id_load.a - P_avg * (U_grid_1h_div.a * Conv.version_P_sym_local_prefilter.out + U_grid_1h_avg_div_ratio);
    id_lim.b = Conv.id_load.b - P_avg * (U_grid_1h_div.b * Conv.version_P_sym_local_prefilter.out + U_grid_1h_avg_div_ratio);
    id_lim.c = Conv.id_load.c - P_avg * (U_grid_1h_div.c * Conv.version_P_sym_local_prefilter.out + U_grid_1h_avg_div_ratio);

    static struct abcn_struct ratio_p;
    ratio_p.a = Saturation(sqrtf(Conv.I_lim_avg_prefilter * Conv.I_lim_avg_prefilter - Conv.iq_lim.a * Conv.iq_lim.a) / fabs(id_lim.a), 0.0f, 1.0f);
    ratio_p.b = Saturation(sqrtf(Conv.I_lim_avg_prefilter * Conv.I_lim_avg_prefilter - Conv.iq_lim.b * Conv.iq_lim.b) / fabs(id_lim.b), 0.0f, 1.0f);
    ratio_p.c = Saturation(sqrtf(Conv.I_lim_avg_prefilter * Conv.I_lim_avg_prefilter - Conv.iq_lim.c * Conv.iq_lim.c) / fabs(id_lim.c), 0.0f, 1.0f);

    register float Id_x = id_lim.a - 0.5f*(id_lim.b + id_lim.c);
    register float Id_y = MATH_SQRT3_2 * (id_lim.c - id_lim.b);
    Conv.Id_x = Id_x;
    Conv.Id_y = Id_y;
    register float Ix = Conv.Iq_x + Id_x;
    register float Iy = Conv.Iq_y + Id_y;
    id_lim.n = sqrtf(Ix * Ix + Iy * Iy);

    if(id_lim.n < Conv.I_lim_avg_prefilter)
    {
        ratio_p.n = 1.0f;
    }
    else
    {
        float a = Conv.Id_x * Conv.Id_x + Conv.Id_y * Conv.Id_y;
        float b = 2.0f * (Conv.Iq_x * Conv.Id_x + Conv.Iq_y * Conv.Id_y);
        float c = Conv.Iq_x * Conv.Iq_x + Conv.Iq_y * Conv.Iq_y - Conv.I_lim_avg_prefilter * Conv.I_lim_avg_prefilter;
        float delta = b * b - 4.0f * a * c;
        if(delta >= 0.0f)
        {
            float sqrt_delta = sqrtf(delta);
            float x1 = (-b+sqrt_delta)/(2.0f * a);
            float x2 = (-b-sqrt_delta)/(2.0f * a);
            if(x1 > 0.0f && x1 < 1.0f)
            {
                ratio_p.n = x1;
                if(x2 > 0.0f && x2 < 1.0f)
                {
                    ratio_p.n = fmaxf(x1, x2);
                }
            }
            else if(x2 > 0.0f && x2 < 1.0f)
            {
                ratio_p.n = x2;
            }
            else ratio_p.n = 0.0f;
        }
        else ratio_p.n = 0.0f;
    }

    register float ratio_p_min = fminf(fminf(fminf(ratio_p.a, ratio_p.b), ratio_p.c), ratio_p.n) * Conv.enable_P_sym_local_prefilter.out;
    Conv.id_lim.a = id_lim.a * ratio_p_min;
    Conv.id_lim.b = id_lim.b * ratio_p_min;
    Conv.id_lim.c = id_lim.c * ratio_p_min;
    Conv.id_lim.n = id_lim.n * ratio_p_min;

    if(ratio_p_min != 1.0f && Conv.enable_P_sym_local_prefilter.out == 1.0f) status_master.in_limit_P = 1;
    else status_master.in_limit_P = 0;

    //////////////////////////////////////////////////////////////////////////////////

    register float I_lim_div = Saturation(1.0f / Conv.I_lim_avg_prefilter, 0.0f, 1.0f);
    Conv.ratio[0] = Conv.I_lim_slave_prefilter[0].out * I_lim_div;
    Conv.ratio[1] = Conv.I_lim_slave_prefilter[1].out * I_lim_div;
    Conv.ratio[2] = Conv.I_lim_slave_prefilter[2].out * I_lim_div;
    Conv.ratio[3] = Conv.I_lim_slave_prefilter[3].out * I_lim_div;

    //problem z kierunkiem pradow w slave - zalezny od znaku wzmocnienia
    static float set_current_gain;
    register float set_current_gain_temp = set_current_gain;
    Conv.id_lim.a += set_current_gain_temp * 0.0f;
    Conv.id_lim.b += -set_current_gain_temp * MATH_SQRT3_2;
    Conv.id_lim.c += set_current_gain_temp * MATH_SQRT3_2;
    Conv.iq_lim.a += set_current_gain_temp;
    Conv.iq_lim.b += set_current_gain_temp * 0.5f;
    Conv.iq_lim.c += set_current_gain_temp * 0.5f;

    Conv.PI_Id[0].lim_H =
    Conv.PI_Id[1].lim_H =
    Conv.PI_Id[2].lim_H =
    Conv.PI_Iq[0].lim_H =
    Conv.PI_Iq[1].lim_H =
    Conv.PI_Iq[2].lim_H = Conv.I_lim_avg_prefilter;
    Conv.PI_Id[0].lim_L =
    Conv.PI_Id[1].lim_L =
    Conv.PI_Id[2].lim_L =
    Conv.PI_Iq[0].lim_L =
    Conv.PI_Iq[1].lim_L =
    Conv.PI_Iq[2].lim_L = -Conv.I_lim_avg_prefilter;

    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[0], Conv.iq_lim.a - Conv.iq_conv.a);
    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[1], Conv.iq_lim.b - Conv.iq_conv.b);
    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[2], Conv.iq_lim.c - Conv.iq_conv.c);

    register float average1 = (Conv.id_lim.a + Conv.id_lim.b + Conv.id_lim.c - Conv.id_conv.a - Conv.id_conv.b - Conv.id_conv.c) * MATH_1_3;
    PI_antiwindup_fast_CLAasm(&Conv.PI_Id[0], Conv.id_lim.a - Conv.id_conv.a - average1);
    PI_antiwindup_fast_CLAasm(&Conv.PI_Id[1], Conv.id_lim.b - Conv.id_conv.b - average1);
    PI_antiwindup_fast_CLAasm(&Conv.PI_Id[2], Conv.id_lim.c - Conv.id_conv.c - average1);
    register float average2 = (Conv.PI_Id[0].integrator + Conv.PI_Id[1].integrator + Conv.PI_Id[2].integrator) * MATH_1_3;
    Conv.PI_Id[0].integrator -= average2;
    Conv.PI_Id[1].integrator -= average2;
    Conv.PI_Id[2].integrator -= average2;
}
