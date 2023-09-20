// Tomasz Święchowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

void Converter_calc_master()
{
    Filter1_calc_CLAasm(&Conv.master.slave[0].I_lim_prefilter, Conv.master.from_slave[0].I_lim);
    Filter1_calc_CLAasm(&Conv.master.slave[1].I_lim_prefilter, Conv.master.from_slave[1].I_lim);
    Filter1_calc_CLAasm(&Conv.master.slave[2].I_lim_prefilter, Conv.master.from_slave[2].I_lim);
    Filter1_calc_CLAasm(&Conv.master.slave[3].I_lim_prefilter, Conv.master.from_slave[3].I_lim);
    Filter1_calc_CLAasm(&Conv.master.slave[4].I_lim_prefilter, Conv.I_lim);

    Conv.master.total.C_conv = Conv.C_conv * Conv.RDY2 + Conv.master.from_slave[0].C_conv + Conv.master.from_slave[1].C_conv + Conv.master.from_slave[2].C_conv + Conv.master.from_slave[3].C_conv;
    Conv.master.total.I_lim = Conv.I_lim + Conv.master.from_slave[0].I_lim + Conv.master.from_slave[1].I_lim + Conv.master.from_slave[2].I_lim + Conv.master.from_slave[3].I_lim;
    Conv.master.total.I_lim_prefilter =
    Conv.master.slave[0].I_lim_prefilter.out + Conv.master.slave[1].I_lim_prefilter.out +
    Conv.master.slave[2].I_lim_prefilter.out + Conv.master.slave[3].I_lim_prefilter.out +
    Conv.master.slave[4].I_lim_prefilter.out;

    register float I_lim_div = Saturation(1.0f / Conv.master.total.I_lim_prefilter, 0.0f, 1.0f);
    Conv.master.slave[0].ratio = Conv.master.slave[0].I_lim_prefilter.out * I_lim_div;
    Conv.master.slave[1].ratio = Conv.master.slave[1].I_lim_prefilter.out * I_lim_div;
    Conv.master.slave[2].ratio = Conv.master.slave[2].I_lim_prefilter.out * I_lim_div;
    Conv.master.slave[3].ratio = Conv.master.slave[3].I_lim_prefilter.out * I_lim_div;
    Conv.master.slave[4].ratio = Conv.master.slave[4].I_lim_prefilter.out * I_lim_div;

    Conv.master.total.P_conv_1h.a = Conv.master.from_slave[0].P_conv_1h.a + Conv.master.from_slave[1].P_conv_1h.a + Conv.master.from_slave[2].P_conv_1h.a + Conv.master.from_slave[3].P_conv_1h.a + Grid.P_conv_1h.a;
    Conv.master.total.P_conv_1h.b = Conv.master.from_slave[0].P_conv_1h.b + Conv.master.from_slave[1].P_conv_1h.b + Conv.master.from_slave[2].P_conv_1h.b + Conv.master.from_slave[3].P_conv_1h.b + Grid.P_conv_1h.b;
    Conv.master.total.P_conv_1h.c = Conv.master.from_slave[0].P_conv_1h.c + Conv.master.from_slave[1].P_conv_1h.c + Conv.master.from_slave[2].P_conv_1h.c + Conv.master.from_slave[3].P_conv_1h.c + Grid.P_conv_1h.c;

    Conv.master.total.Q_conv_1h.a = Conv.master.from_slave[0].Q_conv_1h.a + Conv.master.from_slave[1].Q_conv_1h.a + Conv.master.from_slave[2].Q_conv_1h.a + Conv.master.from_slave[3].Q_conv_1h.a + Grid.Q_conv_1h.a;
    Conv.master.total.Q_conv_1h.b = Conv.master.from_slave[0].Q_conv_1h.b + Conv.master.from_slave[1].Q_conv_1h.b + Conv.master.from_slave[2].Q_conv_1h.b + Conv.master.from_slave[3].Q_conv_1h.b + Grid.Q_conv_1h.b;
    Conv.master.total.Q_conv_1h.c = Conv.master.from_slave[0].Q_conv_1h.c + Conv.master.from_slave[1].Q_conv_1h.c + Conv.master.from_slave[2].Q_conv_1h.c + Conv.master.from_slave[3].Q_conv_1h.c + Grid.Q_conv_1h.c;

    Conv.master.total.P_conv_1h_filter.a = Conv.master.from_slave[0].P_conv_1h_filter.a + Conv.master.from_slave[1].P_conv_1h_filter.a + Conv.master.from_slave[2].P_conv_1h_filter.a + Conv.master.from_slave[3].P_conv_1h_filter.a + Grid_filter.P_conv_1h.a;
    Conv.master.total.P_conv_1h_filter.b = Conv.master.from_slave[0].P_conv_1h_filter.b + Conv.master.from_slave[1].P_conv_1h_filter.b + Conv.master.from_slave[2].P_conv_1h_filter.b + Conv.master.from_slave[3].P_conv_1h_filter.b + Grid_filter.P_conv_1h.b;
    Conv.master.total.P_conv_1h_filter.c = Conv.master.from_slave[0].P_conv_1h_filter.c + Conv.master.from_slave[1].P_conv_1h_filter.c + Conv.master.from_slave[2].P_conv_1h_filter.c + Conv.master.from_slave[3].P_conv_1h_filter.c + Grid_filter.P_conv_1h.c;

    Conv.master.total.Q_conv_1h_filter.a = Conv.master.from_slave[0].Q_conv_1h_filter.a + Conv.master.from_slave[1].Q_conv_1h_filter.a + Conv.master.from_slave[2].Q_conv_1h_filter.a + Conv.master.from_slave[3].Q_conv_1h_filter.a + Grid_filter.Q_conv_1h.a;
    Conv.master.total.Q_conv_1h_filter.b = Conv.master.from_slave[0].Q_conv_1h_filter.b + Conv.master.from_slave[1].Q_conv_1h_filter.b + Conv.master.from_slave[2].Q_conv_1h_filter.b + Conv.master.from_slave[3].Q_conv_1h_filter.b + Grid_filter.Q_conv_1h.b;
    Conv.master.total.Q_conv_1h_filter.c = Conv.master.from_slave[0].Q_conv_1h_filter.c + Conv.master.from_slave[1].Q_conv_1h_filter.c + Conv.master.from_slave[2].Q_conv_1h_filter.c + Conv.master.from_slave[3].Q_conv_1h_filter.c + Grid_filter.Q_conv_1h.c;

    //////////////////////////////////////////////////////////////////////////////////

    struct abc_struct U_grid_1h_div;
    U_grid_1h_div.a = 1.0f / fmaxf(Grid.U_grid_1h.a, 1.0f);
    U_grid_1h_div.b = 1.0f / fmaxf(Grid.U_grid_1h.b, 1.0f);
    U_grid_1h_div.c = 1.0f / fmaxf(Grid.U_grid_1h.c, 1.0f);

    Conv.id_conv.a = Grid.P_conv_1h.a * U_grid_1h_div.a;
    Conv.id_conv.b = Grid.P_conv_1h.b * U_grid_1h_div.b;
    Conv.id_conv.c = Grid.P_conv_1h.c * U_grid_1h_div.c;

    Conv.iq_conv.a = Grid.Q_conv_1h.a * U_grid_1h_div.a;
    Conv.iq_conv.b = Grid.Q_conv_1h.b * U_grid_1h_div.b;
    Conv.iq_conv.c = Grid.Q_conv_1h.c * U_grid_1h_div.c;

    Conv.id_load.a = Grid.P_load_1h.a * U_grid_1h_div.a;
    Conv.id_load.b = Grid.P_load_1h.b * U_grid_1h_div.b;
    Conv.id_load.c = Grid.P_load_1h.c * U_grid_1h_div.c;

    Conv.iq_load.a = Grid.Q_load_1h.a * U_grid_1h_div.a;
    Conv.iq_load.b = Grid.Q_load_1h.b * U_grid_1h_div.b;
    Conv.iq_load.c = Grid.Q_load_1h.c * U_grid_1h_div.c;

    Conv.id_grid.a = Grid.P_grid_1h.a * U_grid_1h_div.a;
    Conv.id_grid.b = Grid.P_grid_1h.b * U_grid_1h_div.b;
    Conv.id_grid.c = Grid.P_grid_1h.c * U_grid_1h_div.c;

    Conv.iq_grid.a = Grid.Q_grid_1h.a * U_grid_1h_div.a;
    Conv.iq_grid.b = Grid.Q_grid_1h.b * U_grid_1h_div.b;
    Conv.iq_grid.c = Grid.Q_grid_1h.c * U_grid_1h_div.c;

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

    register float U_grid_1h_avg_div = MATH_1_3 / (Grid.average.U_grid_1h);
    Conv.id_conv_total = (Grid.P_conv_1h.a + Grid.P_conv_1h.b + Grid.P_conv_1h.c) * U_grid_1h_avg_div;
    Conv.iq_conv_total = (Grid.Q_conv_1h.a + Grid.Q_conv_1h.b + Grid.Q_conv_1h.c) * U_grid_1h_avg_div;
    Conv.id_load_total = Grid.average.P_load_1h * U_grid_1h_avg_div;
    Conv.iq_load_total = Grid.average.Q_load_1h * U_grid_1h_avg_div;
    Conv.id_grid_total = Grid.average.P_grid_1h * U_grid_1h_avg_div;
    Conv.iq_grid_total = Grid.average.Q_grid_1h * U_grid_1h_avg_div;

    if(Conv.no_neutral)
    {
        float iq_tangens;
        if(Conv.tangens_range_local_prefilter[0].a.out > Conv.tangens_range_local_prefilter[1].a.out)
        {
            iq_tangens = fmaxf(Conv.iq_load_total - fabsf(Conv.id_load_total) * Conv.tangens_range_local_prefilter[0].a.out, 0.0f)
                       + fminf(Conv.iq_load_total - fabsf(Conv.id_load_total) * Conv.tangens_range_local_prefilter[1].a.out, 0.0f);
        }
        else
        {
            iq_tangens = fminf(Conv.iq_load_total - fabsf(Conv.id_load_total) * Conv.tangens_range_local_prefilter[0].a.out, 0.0f)
                       + fmaxf(Conv.iq_load_total - fabsf(Conv.id_load_total) * Conv.tangens_range_local_prefilter[1].a.out, 0.0f);
        }

        Conv.iq_load_ref.a = (iq_tangens * (1.0f - Conv.version_Q_comp_local_prefilter.a.out) - Conv.Q_set_local_prefilter.a.out * U_grid_1h_avg_div * 3.0f) * Conv.enable_Q_comp_local_prefilter.a.out;

        float iq_lim_sat;
        register float I_lim_q = Conv.master.total.I_lim_prefilter;
        iq_lim_sat = Saturation(Conv.iq_load_ref.a, -I_lim_q, I_lim_q);

        static float iq_lim;
        register float rate = fmaxf(Conv.master.total.I_lim_prefilter, 24.0f) * 0.0016f;
        iq_lim += Saturation(iq_lim_sat - iq_lim, -rate, rate);

        float not_in_limit = fabsf(iq_lim) - I_lim_q;
        if(not_in_limit == 0.0f && Conv.enable_Q_comp_local.a + Conv.enable_Q_comp_local.b + Conv.enable_Q_comp_local.c >= 1.0f) status_ACDC.in_limit_Q = 1;
        else status_ACDC.in_limit_Q = 0;

        status_ACDC.in_limit_P = 0;

        Conv.master.total.id_lim.a =
        Conv.master.total.id_lim.b =
        Conv.master.total.id_lim.c = 0.0f;
        Conv.master.total.iq_lim.a =
        Conv.master.total.iq_lim.b =
        Conv.master.total.iq_lim.c = iq_lim;
    }
    else
    {
        struct abc_struct iq_tangens;
        if(Conv.tangens_range_local_prefilter[0].a.out > Conv.tangens_range_local_prefilter[1].a.out)
        {
            iq_tangens.a = fmaxf(Conv.iq_load.a - fabsf(Conv.id_load.a) * Conv.tangens_range_local_prefilter[0].a.out, 0.0f)
                         + fminf(Conv.iq_load.a - fabsf(Conv.id_load.a) * Conv.tangens_range_local_prefilter[1].a.out, 0.0f);
        }
        else
        {
            iq_tangens.a = fminf(Conv.iq_load.a - fabsf(Conv.id_load.a) * Conv.tangens_range_local_prefilter[0].a.out, 0.0f)
                         + fmaxf(Conv.iq_load.a - fabsf(Conv.id_load.a) * Conv.tangens_range_local_prefilter[1].a.out, 0.0f);
        }

        if(Conv.tangens_range_local_prefilter[0].b.out > Conv.tangens_range_local_prefilter[1].b.out)
        {
            iq_tangens.b = fmaxf(Conv.iq_load.b - fabsf(Conv.id_load.b) * Conv.tangens_range_local_prefilter[0].b.out, 0.0f)
                         + fminf(Conv.iq_load.b - fabsf(Conv.id_load.b) * Conv.tangens_range_local_prefilter[1].b.out, 0.0f);
        }
        else
        {
            iq_tangens.b = fminf(Conv.iq_load.b - fabsf(Conv.id_load.b) * Conv.tangens_range_local_prefilter[0].b.out, 0.0f)
                         + fmaxf(Conv.iq_load.b - fabsf(Conv.id_load.b) * Conv.tangens_range_local_prefilter[1].b.out, 0.0f);
        }

        if(Conv.tangens_range_local_prefilter[0].c.out > Conv.tangens_range_local_prefilter[1].c.out)
        {
            iq_tangens.c = fmaxf(Conv.iq_load.c - fabsf(Conv.id_load.c) * Conv.tangens_range_local_prefilter[0].c.out, 0.0f)
                         + fminf(Conv.iq_load.c - fabsf(Conv.id_load.c) * Conv.tangens_range_local_prefilter[1].c.out, 0.0f);
        }
        else
        {
            iq_tangens.c = fminf(Conv.iq_load.c - fabsf(Conv.id_load.c) * Conv.tangens_range_local_prefilter[0].c.out, 0.0f)
                         + fmaxf(Conv.iq_load.c - fabsf(Conv.id_load.c) * Conv.tangens_range_local_prefilter[1].c.out, 0.0f);
        }

        Conv.iq_load_ref.a = (iq_tangens.a * (1.0f - Conv.version_Q_comp_local_prefilter.a.out) - Conv.Q_set_local_prefilter.a.out * U_grid_1h_div.a) * Conv.enable_Q_comp_local_prefilter.a.out;
        Conv.iq_load_ref.b = (iq_tangens.b * (1.0f - Conv.version_Q_comp_local_prefilter.b.out) - Conv.Q_set_local_prefilter.b.out * U_grid_1h_div.b) * Conv.enable_Q_comp_local_prefilter.b.out;
        Conv.iq_load_ref.c = (iq_tangens.c * (1.0f - Conv.version_Q_comp_local_prefilter.c.out) - Conv.Q_set_local_prefilter.c.out * U_grid_1h_div.c) * Conv.enable_Q_comp_local_prefilter.c.out;

        struct abc_struct iq_lim_sat;
        register float I_lim_q = Conv.master.total.I_lim_prefilter;
        register float I_lim_q_n = -I_lim_q;
        iq_lim_sat.a = Saturation(Conv.iq_load_ref.a, I_lim_q_n, I_lim_q);
        iq_lim_sat.b = Saturation(Conv.iq_load_ref.b, I_lim_q_n, I_lim_q);
        iq_lim_sat.c = Saturation(Conv.iq_load_ref.c, I_lim_q_n, I_lim_q);

        static struct abcn_struct iq_lim;
        register float rate = fmaxf(Conv.master.total.I_lim_prefilter * 0.0016f, 24.0f * 0.0016f);
        register float rate_n = -rate;
        iq_lim.a += Saturation(iq_lim_sat.a - iq_lim.a, rate_n, rate);
        iq_lim.b += Saturation(iq_lim_sat.b - iq_lim.b, rate_n, rate);
        iq_lim.c += Saturation(iq_lim_sat.c - iq_lim.c, rate_n, rate);

        float not_in_limit = 1.0f;
        not_in_limit *= fabsf(iq_lim.a) - I_lim_q;
        not_in_limit *= fabsf(iq_lim.b) - I_lim_q;
        not_in_limit *= fabsf(iq_lim.c) - I_lim_q;

        register float Iq_x = MATH_SQRT3_2 * (iq_lim.c - iq_lim.b);
        register float Iq_y = 0.5f*(iq_lim.b + iq_lim.c) - iq_lim.a;
        register float sign = Conv.sign;
        Conv.Iq_x = Iq_x * sign;
        Conv.Iq_y = Iq_y * sign;
        iq_lim.n = sqrtf(Iq_x * Iq_x + Iq_y * Iq_y);
        float not_in_limit_n = fminf(I_lim_q, iq_lim.n) - I_lim_q;
        register float ratio_q = Saturation(I_lim_q / fmaxf(I_lim_q, iq_lim.n), 0.0f, 1.0f);
        Conv.master.total.iq_lim.a = iq_lim.a * ratio_q;
        Conv.master.total.iq_lim.b = iq_lim.b * ratio_q;
        Conv.master.total.iq_lim.c = iq_lim.c * ratio_q;
        Conv.master.total.iq_lim.n = iq_lim.n * ratio_q;

        if(not_in_limit * not_in_limit_n == 0.0f && Conv.enable_Q_comp_local.a + Conv.enable_Q_comp_local.b + Conv.enable_Q_comp_local.c >= 1.0f) status_ACDC.in_limit_Q = 1;
        else status_ACDC.in_limit_Q = 0;

        //////////////////////////////////////////////////////////////////////////////////

        static struct abcn_struct id_lim;
        register float P_avg = (Grid.P_load_1h.a + Grid.P_load_1h.b + Grid.P_load_1h.c) * MATH_1_3;
        register float U_grid_1h_avg_div_ratio = (1.0f - Conv.version_P_sym_local_prefilter.out) / fmaxf(Grid.average.U_grid_1h, 1.0f);
        id_lim.a = Conv.id_load.a - P_avg * (U_grid_1h_div.a * Conv.version_P_sym_local_prefilter.out + U_grid_1h_avg_div_ratio);
        id_lim.b = Conv.id_load.b - P_avg * (U_grid_1h_div.b * Conv.version_P_sym_local_prefilter.out + U_grid_1h_avg_div_ratio);
        id_lim.c = Conv.id_load.c - P_avg * (U_grid_1h_div.c * Conv.version_P_sym_local_prefilter.out + U_grid_1h_avg_div_ratio);

        static struct abcn_struct ratio_p;
        ratio_p.a = Saturation(sqrtf(Conv.master.total.I_lim_prefilter * Conv.master.total.I_lim_prefilter - Conv.master.total.iq_lim.a * Conv.master.total.iq_lim.a) / fabsf(id_lim.a), 0.0f, 1.0f);
        ratio_p.b = Saturation(sqrtf(Conv.master.total.I_lim_prefilter * Conv.master.total.I_lim_prefilter - Conv.master.total.iq_lim.b * Conv.master.total.iq_lim.b) / fabsf(id_lim.b), 0.0f, 1.0f);
        ratio_p.c = Saturation(sqrtf(Conv.master.total.I_lim_prefilter * Conv.master.total.I_lim_prefilter - Conv.master.total.iq_lim.c * Conv.master.total.iq_lim.c) / fabsf(id_lim.c), 0.0f, 1.0f);

        register float Id_x = id_lim.a - 0.5f*(id_lim.b + id_lim.c);
        register float Id_y = MATH_SQRT3_2 * (id_lim.c - id_lim.b);
        Conv.Id_x = Id_x;
        Conv.Id_y = Id_y;
        register float Ix = Conv.Iq_x + Id_x;
        register float Iy = Conv.Iq_y + Id_y;
        id_lim.n = sqrtf(Ix * Ix + Iy * Iy);

        if(id_lim.n < Conv.master.total.I_lim_prefilter)
        {
            ratio_p.n = 1.0f;
        }
        else
        {
            float a = Conv.Id_x * Conv.Id_x + Conv.Id_y * Conv.Id_y;
            float b = 2.0f * (Conv.Iq_x * Conv.Id_x + Conv.Iq_y * Conv.Id_y);
            float c = Conv.Iq_x * Conv.Iq_x + Conv.Iq_y * Conv.Iq_y - Conv.master.total.I_lim_prefilter * Conv.master.total.I_lim_prefilter;
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
        Conv.master.total.id_lim.a = id_lim.a * ratio_p_min;
        Conv.master.total.id_lim.b = id_lim.b * ratio_p_min;
        Conv.master.total.id_lim.c = id_lim.c * ratio_p_min;
        Conv.master.total.id_lim.n = id_lim.n * ratio_p_min;

        if(ratio_p_min != 1.0f && Conv.enable_P_sym_local_prefilter.out == 1.0f) status_ACDC.in_limit_P = 1;
        else status_ACDC.in_limit_P = 0;
    }

    Filter1_calc_CLAasm(&Conv.id_ref_override_prefilter.a, Conv.id_ref_override.a);
    Filter1_calc_CLAasm(&Conv.id_ref_override_prefilter.b, Conv.id_ref_override.b);
    Filter1_calc_CLAasm(&Conv.id_ref_override_prefilter.c, Conv.id_ref_override.c);
    Filter1_calc_CLAasm(&Conv.iq_ref_override_prefilter.a, Conv.iq_ref_override.a);
    Filter1_calc_CLAasm(&Conv.iq_ref_override_prefilter.b, Conv.iq_ref_override.b);
    Filter1_calc_CLAasm(&Conv.iq_ref_override_prefilter.c, Conv.iq_ref_override.c);

    Filter1_calc_CLAasm(&Conv.enable_override_prefilter, Conv.enable_override = status_ACDC.control_override);
    Filter1_calc_CLAasm(&Conv.master_slave_prefilter, (float)(status_ACDC.master_slave_selector || !status_ACDC.master_rdy));
    register float ratio_local1 = Conv.master.slave[4].ratio * Conv.master_slave_prefilter.out * Conv.enable_override_prefilter.out;
    register float ratio_local2 = Conv.master.slave[4].ratio * Conv.master_slave_prefilter.out * (1.0f - Conv.enable_override_prefilter.out);
    register float ratio_local3 = Conv.slave.ratio_local * (1.0f - Conv.master_slave_prefilter.out);
    Conv.id_ref.a = Conv.id_ref_override_prefilter.a.out * ratio_local1 + Conv.master.total.id_lim.a * ratio_local2 + Conv.slave.from_master.id_lim.a * ratio_local3;
    Conv.id_ref.b = Conv.id_ref_override_prefilter.b.out * ratio_local1 + Conv.master.total.id_lim.b * ratio_local2 + Conv.slave.from_master.id_lim.b * ratio_local3;
    Conv.id_ref.c = Conv.id_ref_override_prefilter.c.out * ratio_local1 + Conv.master.total.id_lim.c * ratio_local2 + Conv.slave.from_master.id_lim.c * ratio_local3;
    Conv.iq_ref.a = Conv.iq_ref_override_prefilter.a.out * ratio_local1 + Conv.master.total.iq_lim.a * ratio_local2 + Conv.slave.from_master.iq_lim.a * ratio_local3;
    Conv.iq_ref.b = Conv.iq_ref_override_prefilter.b.out * ratio_local1 + Conv.master.total.iq_lim.b * ratio_local2 + Conv.slave.from_master.iq_lim.b * ratio_local3;
    Conv.iq_ref.c = Conv.iq_ref_override_prefilter.c.out * ratio_local1 + Conv.master.total.iq_lim.c * ratio_local2 + Conv.slave.from_master.iq_lim.c * ratio_local3;
}

void Converter_calc_slave()
{
    Conv.U_dc_filter = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Conv.CIC1_U_dc, Meas_ACDC.U_dc);

    ////////////////////////////////////////////////////////////////////////////////////////////

    float U_grid_phph[3];
    U_grid_phph[0] = fabsf(Meas_ACDC.U_grid.a - Meas_ACDC.U_grid.b);
    U_grid_phph[1] = fabsf(Meas_ACDC.U_grid.b - Meas_ACDC.U_grid.c);
    U_grid_phph[2] = fabsf(Meas_ACDC.U_grid.c - Meas_ACDC.U_grid.a);
    float voltage_max_temp = fmaxf(fmaxf(U_grid_phph[0], U_grid_phph[1]), U_grid_phph[2]);

    ////////////////////////////////////////////////////////////////////////////////////////////

    static struct abcn_struct I_conv_running_max;
    I_conv_running_max.a = fmaxf(fabsf(Meas_ACDC.I_conv.a), I_conv_running_max.a);
    I_conv_running_max.b = fmaxf(fabsf(Meas_ACDC.I_conv.b), I_conv_running_max.b);
    I_conv_running_max.c = fmaxf(fabsf(Meas_ACDC.I_conv.c), I_conv_running_max.c);
    I_conv_running_max.n = fmaxf(fabsf(Meas_ACDC.I_conv.n), I_conv_running_max.n);

    static float U_grid_phph_running_max;
    U_grid_phph_running_max = fmaxf(voltage_max_temp, U_grid_phph_running_max);

    static float counter_max;
    counter_max += Conv.Ts;
    if (counter_max > 0.021f)
    {
        counter_max = 0.021f;

        Conv.I_conv_max.a = I_conv_running_max.a;
        Conv.I_conv_max.b = I_conv_running_max.b;
        Conv.I_conv_max.c = I_conv_running_max.c;
        Conv.I_conv_max.n = I_conv_running_max.n;
        I_conv_running_max.a =
        I_conv_running_max.b =
        I_conv_running_max.c =
        I_conv_running_max.n = 0.0f;
    }
    static float counter_max2;
    counter_max2 += Conv.Ts;
    if (counter_max2 > 0.5f)
    {
        counter_max2 = 0.0f;

        Conv.U_grid_phph_max = U_grid_phph_running_max;
        U_grid_phph_running_max = 0.0f;
    }

//    Conv.U_dc_ref += Conv.Ts * (1.0f / 0.1f) *(fminf(Conv.U_grid_phph_max + 85.0f, 650.0f) - Conv.U_dc_ref);

    if (!Conv.enable || (alarm_ACDC.all[0] | alarm_ACDC.all[1] | alarm_ACDC.all[2]))
    {
        GPIO_CLEAR(PWM_EN_CM);
        GPIO_CLEAR(SS_DClink_CM);
        GPIO_CLEAR(GR_RLY_L1_CM);
        GPIO_CLEAR(GR_RLY_L2_CM);
        GPIO_CLEAR(GR_RLY_L3_CM);
        GPIO_CLEAR(GR_RLY_N_CM);
        GPIO_CLEAR(C_SS_RLY_L1_CM);
        GPIO_CLEAR(C_SS_RLY_L2_CM);
        GPIO_CLEAR(C_SS_RLY_L3_CM);
        GPIO_CLEAR(C_SSR_CM);

        Meas_ACDC_alarm_L.U_dc = -5.0f;

        Conv.PI_I_harm_ratio[0].out =
        Conv.PI_I_harm_ratio[1].out =
        Conv.PI_I_harm_ratio[2].out =
        Conv.PI_I_harm_ratio[3].out =
        Conv.I_lim =
        Conv.RDY =
        Conv.RDY2 = 0.0f;
        Conv.state = CONV_softstart;
        Conv.state_last = CONV_active;
    }
    else
    {
        switch (Conv.state)
        {
        case CONV_softstart:
        {
            static float counter_ss;
            static float counter_ss_last;
            static float integrated_current;
            static float integrated_voltage;
            static float current_running_max;
            if (Conv.state_last != Conv.state)
            {
                current_running_max =
                integrated_current =
                integrated_voltage =
                counter_ss =
                counter_ss_last = 0.0f;
//                GPIO_SET(SS_DClink_CM);

                Conv.state_last = Conv.state;
            }

            integrated_current += Conv.Ts * (voltage_max_temp - Meas_ACDC.U_dc - integrated_voltage - 6.0f) / (2.0f * (Conv.L_conv + 100e-6));
            integrated_current = fmaxf(0.0f, integrated_current);
            integrated_voltage += Conv.Ts * integrated_current / Conv.C_dc;

            current_running_max = fmaxf(current_running_max, integrated_current);
            if(integrated_current < 1.0f) integrated_voltage = 0.0f;
            if(Conv.U_dc_filter > 0.0f)
            {
                Conv.state++;
            }
            if (counter_ss - counter_ss_last > 1.0f)
            {
                if ((Meas_ACDC.U_dc > 0.8f*Conv.U_grid_phph_max) && (current_running_max < 20.0f * MATH_SQRT2) && (Grid.average.U_grid_1h > 10.0f)) Conv.state++;
                current_running_max = 0;
                counter_ss_last = counter_ss;
            }

            counter_ss += Conv.Ts;
            if (counter_ss > 60.0f) alarm_ACDC.bit.CONV_SOFTSTART = 1;
            break;
        }
        case CONV_grid_relay:
        {
            static float counter_ss;
            static float counter_ss2;
            static float SSR_A_done;
            static float SSR_B_done;
            static float SSR_C_done;
            static struct abc_struct U_grid_last;
            if (Conv.state_last != Conv.state)
            {
                SSR_A_done =
                SSR_B_done =
                SSR_C_done =
                counter_ss =
                counter_ss2 = 0;

                U_grid_last.a =
                U_grid_last.b =
                U_grid_last.c = 1e6;

                GPIO_CLEAR(SS_DClink_CM);

                Conv.state_last = Conv.state;
            }

            counter_ss += Conv.Ts;
            if (counter_ss > 0.5f)
            {
                if(fabsf(2.0f * Meas_ACDC.U_grid.a - U_grid_last.a) < 1.0f) GPIO_SET(C_SS_RLY_L1_CM), SSR_A_done = 1.0f;
                if(fabsf(2.0f * Meas_ACDC.U_grid.b - U_grid_last.b) < 1.0f) GPIO_SET(C_SS_RLY_L2_CM), SSR_B_done = 1.0f;
                if(fabsf(2.0f * Meas_ACDC.U_grid.c - U_grid_last.c) < 1.0f) GPIO_SET(C_SS_RLY_L3_CM), SSR_C_done = 1.0f;
            }

            U_grid_last.a = Meas_ACDC.U_grid.a;
            U_grid_last.b = Meas_ACDC.U_grid.b;
            U_grid_last.c = Meas_ACDC.U_grid.c;

            if(SSR_A_done * SSR_B_done * SSR_C_done)
            {
                counter_ss2 += Conv.Ts;
//                if (counter_ss2 > 0.5f)
//                {
//                    GPIO_SET(GR_RLY_L1_CM);
//                    GPIO_SET(GR_RLY_L2_CM);
//                    GPIO_SET(GR_RLY_L3_CM);
//                    if(!Conv.no_neutral) GPIO_SET(GR_RLY_N_CM);
//                }

//                if (counter_ss2 > 1.0f)
//                {
//                    Conv.state++;
//                }
            }

            break;
        }
        case CONV_active:
        {
            if (Conv.state_last != Conv.state)
            {
                GPIO_SET(PWM_EN_CM);
                Conv.RDY = 1;
                Conv.state_last = Conv.state;
            }

            //////////////////////////////////////////////////////////////////////////////////

            if(Conv.RDY2)
            {
                Meas_ACDC_alarm_L.U_dc = Conv.U_grid_phph_max + 20.0f;//#TODO do zmianyw w finalnej wersji
//                Meas_alarm_L.U_dc = 670.0f;
                register float Temp_power = 0;
                Temp_power = fmaxf(Meas_ACDC.Temperature1,  fmaxf(Meas_ACDC.Temperature2, Meas_ACDC.Temperature3));
                float duty_power = fmaxf(1.0f - fmaxf((Temp_power - (Meas_ACDC_alarm_H.Temp - 5.0f)) * 0.2, 0.0f), 0.0f);
                Conv.I_lim = fmaxf(Conv.I_lim_nominal * duty_power, 1.0f);
            }
            else if(fabsf(Conv.U_dc_filter - Conv.U_dc_ref) < 1.0f)
                Conv.RDY2 = 1.0f;

            float U_grid_1h_avg_div = 1.0f / (Grid.average.U_grid_1h);
            register float error_U_dc = (Conv.U_dc_ref - Conv.U_dc_kalman) * Conv.U_dc_filter * U_grid_1h_avg_div * MATH_1_3;
            PI_antiwindup_fast_CLAasm(&Conv.PI_U_dc, error_U_dc);

            //////////////////////////////////////////////////////////////////////////////////

            if (Conv.enable_override_prefilter.out == 1.0f)
            {
                Conv.PI_Id[0].out = Conv.id_ref.a - Conv.PI_U_dc.out;
                Conv.PI_Id[1].out = Conv.id_ref.b - Conv.PI_U_dc.out;
                Conv.PI_Id[2].out = Conv.id_ref.c - Conv.PI_U_dc.out;
                Conv.PI_Iq[0].out = Conv.iq_ref.a;
                Conv.PI_Iq[1].out = Conv.iq_ref.b;
                Conv.PI_Iq[2].out = Conv.iq_ref.c;
            }
            else
            {
                if(Conv.no_neutral)
                {
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[0], Conv.iq_ref.a - Conv.iq_conv_total);
                    Conv.PI_Iq[1].out =
                    Conv.PI_Iq[2].out = Conv.PI_Iq[0].out;

                    Conv.PI_Id[0].out =
                    Conv.PI_Id[1].out =
                    Conv.PI_Id[2].out = -Conv.PI_U_dc.out;
                }
                else
                {
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[0], Conv.iq_ref.a - Conv.iq_conv.a);
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[1], Conv.iq_ref.b - Conv.iq_conv.b);
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Iq[2], Conv.iq_ref.c - Conv.iq_conv.c);

                    register float average1 = (Conv.id_ref.a + Conv.id_ref.b + Conv.id_ref.c - Conv.id_conv.a - Conv.id_conv.b - Conv.id_conv.c) * MATH_1_3;
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Id[0], Conv.id_ref.a - Conv.id_conv.a - average1);
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Id[1], Conv.id_ref.b - Conv.id_conv.b - average1);
                    PI_antiwindup_fast_CLAasm(&Conv.PI_Id[2], Conv.id_ref.c - Conv.id_conv.c - average1);
                    register float average2 = (Conv.PI_Id[0].integrator + Conv.PI_Id[1].integrator + Conv.PI_Id[2].integrator) * MATH_1_3;
                    Conv.PI_Id[0].integrator -= average2;
                    Conv.PI_Id[1].integrator -= average2;
                    Conv.PI_Id[2].integrator -= average2;
                    Conv.PI_Id[0].out += -Conv.PI_U_dc.out;
                    Conv.PI_Id[1].out += -Conv.PI_U_dc.out;
                    Conv.PI_Id[2].out += -Conv.PI_U_dc.out;
                }
            }

            //////////////////////////////////////////////////////////////////////////////////

            if (Conv.enable_H_comp_local * Conv.RDY2)
            {
                if(Conv.PI_I_harm_ratio[3].lim_H != 1.0f)
                {
                    Conv.PI_I_harm_ratio[0].integrator =
                    Conv.PI_I_harm_ratio[1].integrator =
                    Conv.PI_I_harm_ratio[2].integrator =
                    Conv.PI_I_harm_ratio[3].integrator = -1.0f;
                    Conv.PI_I_harm_ratio[3].lim_H = 1.0f;
                }
            }
            else
            {
                Conv.PI_I_harm_ratio[3].lim_H = 0.0f;
            }

            register float I_lim = Conv.I_lim;
            register float I_lim_div = Saturation(1.0f / I_lim, 0.0f, 1.0f);
            if(Conv.no_neutral)
            {
                float I_conv_max = fmaxf(fmaxf(Conv.I_conv_max.a, Conv.I_conv_max.b), Conv.I_conv_max.c);
                float I_conv_rms = fmaxf(fmaxf(Grid.I_conv.a, Grid.I_conv.b), Grid.I_conv.c);
                float error = (I_lim - fmaxf(I_conv_max * MATH_1_SQRT2 * 0.9f, I_conv_rms)) * I_lim_div;
                PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[3], error);
                Conv.PI_I_harm_ratio[0].out =
                Conv.PI_I_harm_ratio[1].out =
                Conv.PI_I_harm_ratio[2].out = Conv.PI_I_harm_ratio[3].out * (1.0f - Conv.sag);
            }
            else
            {
                static struct abcn_struct error;
                error.n = (I_lim - fmaxf(Conv.I_conv_max.n * MATH_1_SQRT2 * 0.9f, Grid.I_conv.n)) * I_lim_div;
                PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[3], error.n);
                Conv.PI_I_harm_ratio[0].lim_H =
                Conv.PI_I_harm_ratio[1].lim_H =
                Conv.PI_I_harm_ratio[2].lim_H = Conv.PI_I_harm_ratio[3].out * (1.0f - Conv.sag);
                error.a = (I_lim - fmaxf(Conv.I_conv_max.a * MATH_1_SQRT2 * 0.9f, Grid.I_conv.a)) * I_lim_div;
                error.b = (I_lim - fmaxf(Conv.I_conv_max.b * MATH_1_SQRT2 * 0.9f, Grid.I_conv.b)) * I_lim_div;
                error.c = (I_lim - fmaxf(Conv.I_conv_max.c * MATH_1_SQRT2 * 0.9f, Grid.I_conv.c)) * I_lim_div;
                PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[0], error.a);
                PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[1], error.b);
                PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[2], error.c);
            }

            if (Conv.PI_I_harm_ratio[0].out * Conv.PI_I_harm_ratio[1].out * Conv.PI_I_harm_ratio[2].out != 1.0f && Conv.RDY2 * Conv.enable_H_comp_local)
                status_ACDC.in_limit_H = 1;
            else status_ACDC.in_limit_H = 0;
            break;
        }
        default:
        {
            break;
        }
        }
    }
}
