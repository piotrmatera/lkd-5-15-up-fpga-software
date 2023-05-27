// Tomasz Święchowicz swiechowicz.tomasz@gmail.com

#include "stdafx.h"

void Converter_calc()
{
    register float sum;
    sum = fabsf(Conv.Kalman_U_grid_diff.a = Meas_master.U_grid.a - Conv.Kalman_U_grid.a);
    sum += fabsf(Conv.Kalman_U_grid_diff.b = Meas_master.U_grid.b - Conv.Kalman_U_grid.b);
    sum += fabsf(Conv.Kalman_U_grid_diff.c = Meas_master.U_grid.c - Conv.Kalman_U_grid.c);

    static float sag_timer;
    sag_timer += Conv.Ts;
    if (sum >= 20.0f) sag_timer = 0.0f;
    Conv.sag = 0.0f;
    if (sag_timer < 20e-3)
    {
        Conv.sag = 1.0f;
    }

    Conv.U_dc_filter = CIC1_adaptive_filter_CLAasm(&CIC1_adaptive_global__50Hz, &Conv.CIC1_U_dc, Meas_master.U_dc);
    //Conv.U_dc_kalman = CLA2toCLA1.U_dc_kalman;

    ////////////////////////////////////////////////////////////////////////////////////////////

    float U_grid_phph[3];
    U_grid_phph[0] = fabs(Meas_master.U_grid.a - Meas_master.U_grid.b);
    U_grid_phph[1] = fabs(Meas_master.U_grid.b - Meas_master.U_grid.c);
    U_grid_phph[2] = fabs(Meas_master.U_grid.c - Meas_master.U_grid.a);
    float voltage_max_temp = fmaxf(fmaxf(U_grid_phph[0], U_grid_phph[1]), U_grid_phph[2]);

    ////////////////////////////////////////////////////////////////////////////////////////////

    static struct abcn_struct I_conv_running_max;
    I_conv_running_max.a = fmaxf(fabs(Meas_master.I_conv.a), I_conv_running_max.a);
    I_conv_running_max.b = fmaxf(fabs(Meas_master.I_conv.b), I_conv_running_max.b);
    I_conv_running_max.c = fmaxf(fabs(Meas_master.I_conv.c), I_conv_running_max.c);
    I_conv_running_max.n = fmaxf(fabs(Meas_master.I_conv.n), I_conv_running_max.n);

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

    if (!Conv.enable || (alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]))
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
        GPIO_CLEAR(C_SS_RLY_N_CM);

        Meas_alarm_L.U_dc = -5.0f;

        Conv.I_ref.a =
        Conv.I_ref.b =
        Conv.I_ref.c =
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
                GPIO_SET(SS_DClink_CM);

                Conv.state_last = Conv.state;
            }

            integrated_current += Conv.Ts * (voltage_max_temp - Meas_master.U_dc - integrated_voltage - 6.0f) / (2.0f * (Conv.L_conv + 100e-6));
            integrated_current = fmaxf(0.0f, integrated_current);
            integrated_voltage += Conv.Ts * integrated_current / Conv.C_dc;

            current_running_max = fmaxf(current_running_max, integrated_current);
            if(integrated_current < 1.0f) integrated_voltage = 0.0f;
            if(Conv.U_dc_filter > 620.0f)
            {
                Conv.state++;
            }
            if (counter_ss - counter_ss_last > 1.0f)
            {
                if ((Meas_master.U_dc > 0.8f*Conv.U_grid_phph_max) && (current_running_max < 20.0f * MATH_SQRT2) && (Grid.parameters.average.U_grid_1h > 10.0f)) Conv.state++;
                current_running_max = 0;
                counter_ss_last = counter_ss;
            }

            counter_ss += Conv.Ts;
            if (counter_ss > 60.0f) alarm_master.bit.CONV_SOFTSTART = 1;
            break;
        }
        case CONV_grid_relay:
        {
            static float counter_ss;
            if (Conv.state_last != Conv.state)
            {
                counter_ss = 0;
                GPIO_CLEAR(SS_DClink_CM);

                Conv.state_last = Conv.state;
            }

            counter_ss += Conv.Ts;
            if (counter_ss > 0.5f)
            {
                GPIO_SET(C_SS_RLY_L1_CM);
                GPIO_SET(C_SS_RLY_L2_CM);
                GPIO_SET(C_SS_RLY_L3_CM);
                GPIO_SET(C_SS_RLY_N_CM);
            }

            if (counter_ss > 1.0f)
            {
                GPIO_SET(GR_RLY_L1_CM);
                GPIO_SET(GR_RLY_L2_CM);
                GPIO_SET(GR_RLY_L3_CM);
                GPIO_SET(GR_RLY_N_CM);
            }

            if (counter_ss > 1.5f)
            {
                GPIO_CLEAR(C_SS_RLY_L1_CM);
                GPIO_CLEAR(C_SS_RLY_L2_CM);
                GPIO_CLEAR(C_SS_RLY_L3_CM);
                GPIO_CLEAR(C_SS_RLY_N_CM);
            }

            if (counter_ss > 2.0f || (Conv.U_dc_filter > 620.0f && counter_ss > 1.75f))
            {
                Conv.state++;
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
                Meas_alarm_L.U_dc = Conv.U_grid_phph_max + 20.0f;//#TODO do zmianyw w finalnej wersji
//                Meas_alarm_L.U_dc = 670.0f;
                register float Temp_power = 0;
                Temp_power = fmaxf(Meas_master.Temperature1,  fmaxf(Meas_master.Temperature2, Meas_master.Temperature3));
                float duty_power = fmaxf(1.0f - fmaxf((Temp_power - (Meas_alarm_H.Temp - 5.0f)) * 0.2, 0.0f), 0.0f);
                Conv.I_lim = fmaxf(Conv.I_lim_nominal * duty_power, 1.0f);
            }
            else if(fabs(Conv.U_dc_filter - Conv.U_dc_ref) < 1.0f)
                Conv.RDY2 = 1.0f;

            float U_grid_1h_avg_div = 1.0f / (Grid.parameters.average.U_grid_1h);
            register float error_U_dc = (Conv.U_dc_ref - Conv.U_dc_kalman) * Conv.U_dc_filter * U_grid_1h_avg_div * MATH_1_3;
            PI_antiwindup_fast_CLAasm(&Conv.PI_U_dc, error_U_dc);

            //////////////////////////////////////////////////////////////////////////////////

            if (Conv.enable_H_comp_local)
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
            static struct abcn_struct error;
            error.n = (I_lim - fmaxf(Conv.I_conv_max.n * MATH_1_SQRT2 * 0.9f, Grid.parameters.I_conv.n)) * I_lim_div;
            PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[3], error.n);
            Conv.PI_I_harm_ratio[0].lim_H =
            Conv.PI_I_harm_ratio[1].lim_H =
            Conv.PI_I_harm_ratio[2].lim_H = Conv.PI_I_harm_ratio[3].out * (1.0f - Conv.sag);
            error.a = (I_lim - fmaxf(Conv.I_conv_max.a * MATH_1_SQRT2 * 0.9f, Grid.parameters.I_conv.a)) * I_lim_div;
            error.b = (I_lim - fmaxf(Conv.I_conv_max.b * MATH_1_SQRT2 * 0.9f, Grid.parameters.I_conv.b)) * I_lim_div;
            error.c = (I_lim - fmaxf(Conv.I_conv_max.c * MATH_1_SQRT2 * 0.9f, Grid.parameters.I_conv.c)) * I_lim_div;
            PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[0], error.a);
            PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[1], error.b);
            PI_antiwindup_CLAasm(&Conv.PI_I_harm_ratio[2], error.c);

            if (Conv.PI_I_harm_ratio[0].out * Conv.PI_I_harm_ratio[1].out * Conv.PI_I_harm_ratio[2].out != 1.0f && Conv.enable_H_comp_local)
                status_master.in_limit_H = 1;
            else status_master.in_limit_H = 0;
            break;
        }
        default:
        {
            break;
        }
        }
    }
}
