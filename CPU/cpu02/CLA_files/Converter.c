// Tomasz Święchowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

void Converter_calc()
{
    Conv.I_ref.a = (Conv.id_ref.a * PLL.trig_table[0].cosine + Conv.iq_ref.a * PLL.trig_table[0].sine * PLL.sign) * MATH_SQRT2;
    Conv.I_ref.b = (Conv.id_ref.b * PLL.trig_table[1].cosine + Conv.iq_ref.b * PLL.trig_table[1].sine * PLL.sign) * MATH_SQRT2;
    Conv.I_ref.c = (Conv.id_ref.c * PLL.trig_table[2].cosine + Conv.iq_ref.c * PLL.trig_table[2].sine * PLL.sign) * MATH_SQRT2;

    Conv.I_err.a = Conv.I_ref.a - Meas_master.I_conv.a;
    Conv.I_err.b = Conv.I_ref.b - Meas_master.I_conv.b;
    Conv.I_err.c = Conv.I_ref.c - Meas_master.I_conv.c;

    Cla1SoftIntRegs.SOFTINTFRC.all =
    Cla1SoftIntRegs.SOFTINTEN.all = 1;

    ///////////////////////////////////////////////////////////////////

    register float wL = PLL.w_filter * MATH_SQRT2;
    Conv.U_coupl.a = wL * (Conv.iq_ref.a * PLL.trig_table[0].cosine + Conv.id_ref.a * PLL.trig_table[0].sine * PLL.sign);
    Conv.U_coupl.b = wL * (Conv.iq_ref.b * PLL.trig_table[1].cosine + Conv.id_ref.b * PLL.trig_table[1].sine * PLL.sign);
    Conv.U_coupl.c = wL * (Conv.iq_ref.c * PLL.trig_table[2].cosine + Conv.id_ref.c * PLL.trig_table[2].sine * PLL.sign);

    Conv.div_U_dc = 1.0f / fmaxf(Meas_master.U_dc_avg, 1.0f);
    register float Kp_I = Conv.Kp_I;
    Conv.U_ref.a = Conv.I_err.a * Kp_I + Conv.MR_ref.a + Conv.U_coupl.a;
    Conv.U_ref.b = Conv.I_err.b * Kp_I + Conv.MR_ref.b + Conv.U_coupl.b;
    Conv.U_ref.c = Conv.I_err.c * Kp_I + Conv.MR_ref.c + Conv.U_coupl.c;
    Conv.U_ref.n = -(Conv.U_ref.a + Conv.U_ref.b + Conv.U_ref.c);

//    Conv.U_ref.a += Meas_master.U_grid.a;
//    Conv.U_ref.b += Meas_master.U_grid.b;
//    Conv.U_ref.c += Meas_master.U_grid.c;

//    register float sum;
//    sum = fabsf(Conv.Kalman_U_grid_diff.a = Meas_master.U_grid.a - Conv.Kalman_U_grid.a);
//    sum += fabsf(Conv.Kalman_U_grid_diff.b = Meas_master.U_grid.b - Conv.Kalman_U_grid.b);
//    sum += fabsf(Conv.Kalman_U_grid_diff.c = Meas_master.U_grid.c - Conv.Kalman_U_grid.c);
//
//    static float sag_timer;
//    sag_timer += Conv.Ts;
//    if (sum >= 20.0f) sag_timer = 0.0f;
//    Conv.sag = 0.0f;
//    if (sag_timer < 20e-3)
//    {
//        Conv.sag = 1.0f;
//        Conv.U_ref.a += Conv.Kalman_U_grid_diff.a;
//        Conv.U_ref.b += Conv.Kalman_U_grid_diff.b;
//        Conv.U_ref.c += Conv.Kalman_U_grid_diff.c;
//    }

    register float max = fmaxf(Conv.U_ref.a, fmaxf(Conv.U_ref.b, Conv.U_ref.c));
    register float min = fminf(Conv.U_ref.a, fminf(Conv.U_ref.b, Conv.U_ref.c));
    register float correction = (min + max) * (-0.5f);

    register float ref_scaling = Conv.div_U_dc * Conv.cycle_period * 2.0f;
    Conv.duty[0] = ref_scaling * (Conv.U_ref.a + correction);
    Conv.duty[1] = ref_scaling * (Conv.U_ref.b + correction);
    Conv.duty[2] = ref_scaling * (Conv.U_ref.c + correction);
    Conv.duty[3] = ref_scaling * (Conv.U_ref.n + correction);

    Cla1SoftIntRegs.SOFTINTFRC.all =
    Cla1SoftIntRegs.SOFTINTEN.all = 2;
}
