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

    Timer_PWM.CLA_IERR = TIMESTAMP_PWM;

    ///////////////////////////////////////////////////////////////////

    register float wL = PLL.w_filter * MATH_SQRT2 * Conv.L_conv;
    Conv.U_coupl.a = wL * (Conv.iq_ref.a * PLL.trig_table[0].cosine + Conv.id_ref.a * PLL.trig_table[0].sine * PLL.sign);
    Conv.U_coupl.b = wL * (Conv.iq_ref.b * PLL.trig_table[1].cosine + Conv.id_ref.b * PLL.trig_table[1].sine * PLL.sign);
    Conv.U_coupl.c = wL * (Conv.iq_ref.c * PLL.trig_table[2].cosine + Conv.id_ref.c * PLL.trig_table[2].sine * PLL.sign);

    register float Kp_I = Conv.Kp_I;
    Conv.U_ref.a = Conv.I_err.a * Kp_I + Conv.MR_ref.a + Conv.U_coupl.a;
    Conv.U_ref.b = Conv.I_err.b * Kp_I + Conv.MR_ref.b + Conv.U_coupl.b;
    Conv.U_ref.c = Conv.I_err.c * Kp_I + Conv.MR_ref.c + Conv.U_coupl.c;
    Conv.U_ref.n = -(Conv.U_ref.a + Conv.U_ref.b + Conv.U_ref.c);

    register float sum;
    sum =  fabsf(Conv.Kalman_U_grid_diff.a = Meas_master.U_grid.a - Conv.Kalman_U_grid.a);
    sum += fabsf(Conv.Kalman_U_grid_diff.b = Meas_master.U_grid.b - Conv.Kalman_U_grid.b);
    sum += fabsf(Conv.Kalman_U_grid_diff.c = Meas_master.U_grid.c - Conv.Kalman_U_grid.c);

    static float sag_timer;
    sag_timer += Conv.Ts;
    if (sum >= 20.0f) sag_timer = 0.0f;
    Conv.sag = 0.0f;
    if (sag_timer < 20e-3)
    {
        Conv.sag = 1.0f;
//        Conv.U_ref.a += Conv.Kalman_U_grid_diff.a;
//        Conv.U_ref.b += Conv.Kalman_U_grid_diff.b;
//        Conv.U_ref.c += Conv.Kalman_U_grid_diff.c;
    }

    register float ref_scaling = Conv.cycle_period * 2.0f / fmaxf(Meas_master.U_dc_avg, 1.0f);
    Conv.duty_float[0] = Conv.U_ref.a * ref_scaling;
    Conv.duty_float[1] = Conv.U_ref.b * ref_scaling;
    Conv.duty_float[2] = Conv.U_ref.c * ref_scaling;
    Conv.duty_float[3] = Conv.U_ref.n * ref_scaling;

    static volatile float select_modulation;
    static volatile float switcher;
    static volatile float switcher_val;
    register float correction;
    if(select_modulation)
    {
        register float max = fmaxf(Conv.duty_float[0], fmaxf(Conv.duty_float[1], Conv.duty_float[2]));
        register float min = fminf(Conv.duty_float[0], fminf(Conv.duty_float[1], Conv.duty_float[2]));
        correction = (min + max) * (-0.5f);
    }
    else
    {
        Uint32 index_max = 0;
        if (Conv.duty_float[1] > Conv.duty_float[0]) index_max = 1;
        if (Conv.duty_float[2] > Conv.duty_float[index_max]) index_max = 2;
        Uint32 index_min = 0;
        if (Conv.duty_float[1] < Conv.duty_float[0]) index_min = 1;
        if (Conv.duty_float[2] < Conv.duty_float[index_min]) index_min = 2;

        register float correction_switch;
        correction_switch = Conv.correction_switch;
        if(switcher++ > switcher_val)
        {
            switcher = 0.0f;
            correction_switch = 0.0f;
            if (fabsf(*(&Conv.I_ref.a + index_max)) > fabsf(*(&Conv.I_ref.a + index_min))) correction_switch = 1.0f;
        }

        if (correction_switch) correction = Conv.cycle_period - Conv.duty_float[index_max];
        else correction = -Conv.cycle_period - Conv.duty_float[index_min];

        if (Conv.correction_switch != correction_switch) correction = (Conv.correction + correction) * 0.5f;
        Conv.correction_switch = correction_switch;
    }

    Conv.correction = correction;
    Conv.duty[0] = Conv.duty_float[0] + correction;
    Conv.duty[1] = Conv.duty_float[1] + correction;
    Conv.duty[2] = Conv.duty_float[2] + correction;
    Conv.duty[3] = Conv.duty_float[3] + correction;

    static volatile int32 add_val;
    if((float)Conv.duty[0] == Conv.cycle_period) Conv.duty[0] += add_val;
    if((float)Conv.duty[1] == Conv.cycle_period) Conv.duty[1] += add_val;
    if((float)Conv.duty[2] == Conv.cycle_period) Conv.duty[2] += add_val;
    if((float)Conv.duty[3] == Conv.cycle_period) Conv.duty[3] += add_val;
    if((float)Conv.duty[0] == -Conv.cycle_period) Conv.duty[0] -= add_val;
    if((float)Conv.duty[1] == -Conv.cycle_period) Conv.duty[1] -= add_val;
    if((float)Conv.duty[2] == -Conv.cycle_period) Conv.duty[2] -= add_val;
    if((float)Conv.duty[3] == -Conv.cycle_period) Conv.duty[3] -= add_val;

    Cla1SoftIntRegs.SOFTINTFRC.all =
    Cla1SoftIntRegs.SOFTINTEN.all = 2;

    Timer_PWM.CLA_CONV = TIMESTAMP_PWM;
}