/*
 * State.cpp
 *
 *  Created on: 2 maj 2020
 *      Author: MrTea
 */

#include <math.h>
#include <string.h>

#include "stdafx.h"

#include "State_background.h"
#include "State_master.h"

#include "Scope.h"
#include "SD_card.h"
#include "Init.h"
#include "FLASH.h"

#include "MosfetCtrlApp.h"

class Machine_slave_class Machine_slave;
void (*Machine_slave_class::state_pointers[Machine_slave_class::state_max])();
struct timer_struct Timer_total;

class FLASH_class error_retry_FLASH =
{
 .address = {(Uint16 *)&Machine_slave.error_retry, 0},
 .sector = SectorK,
 .size16_each = {sizeof(Machine_slave.error_retry), 0},
};

void timer_update(struct timer_struct *Timer, Uint16 enable_counting)
{
    struct timer_struct Timer_temp = *Timer;

    Timer_temp.magic = 3;
    if(enable_counting)
    {
        Uint32 counter_old = Timer_temp.counter;
        Timer_temp.counter = ReadIpcTimer();
        Timer_temp.integrator += (Timer_temp.counter - counter_old);
        if(Timer_temp.integrator >= 200000000) Timer_temp.seconds++, Timer_temp.integrator -= 200000000;
        if(Timer_temp.seconds >= 60) Timer_temp.minutes++, Timer_temp.seconds = 0;
        if(Timer_temp.minutes >= 60) Timer_temp.hours++, Timer_temp.minutes = 0;
        if(Timer_temp.hours >= 24) Timer_temp.days++, Timer_temp.hours = 0;
    }
    else Timer_temp.counter = ReadIpcTimer();

    *Timer = Timer_temp;
}

void Machine_slave_class::Main()
{
    register void (*pointer_temp)() = Machine_slave.state_pointers[Machine_slave.state];

    if(pointer_temp != NULL && Machine_slave.state < sizeof(Machine_slave_class::state_pointers)/sizeof(Machine_slave_class::state_pointers[0]))
        (*pointer_temp)();
    else
        Machine_slave.state = state_idle;

    if(Machine_slave.error_retry)
    {
        if(Timer_total.minutes + Timer_total.hours + Timer_total.days || !ONOFF.ONOFF)
        {
            Machine_slave.error_retry = 0;
            status_ACDC.error_retry = Machine_slave.error_retry;
            error_retry_FLASH.save();
        }
    }
}

void Machine_slave_class::idle()
{
    static Uint64 delay_timer;
    static float delay = 0.0f;
    if(Machine_slave.state_last != Machine_slave.state)
    {
        delay_timer = ReadIpcTimer();
        Machine_slave.state_last = Machine_slave.state;
        Conv.enable = 0;
    }

    if(ONOFF.ONOFF)
    {
        float Temp = fmaxf(Meas_ACDC.Temperature1, fmaxf(Meas_ACDC.Temperature2, Meas_ACDC.Temperature3));
        if(status_ACDC.SD_no_calibration) Machine_slave.state = state_calibrate_offsets;
        else if(status_ACDC.PLL_sync && status_ACDC.Grid_present && Temp < Meas_ACDC_alarm_H.Temp - 15.0f)
        {
            static const float delay_table[] =
            {
                 [0] = 0.0f,
                 [1] = 0.0f,
                 [2] = 10.0f,
                 [3] = 0.0f,
                 [4] = 60.0f,
                 [5] = 0.0f,
                 [6] = 10.0f * 60.0f,
                 [7] = 0.0f,
                 [8] = 1.0f * 60.0f * 60.0f,
                 [9] = 0.0f,
                 [10] = 2.0f * 60.0f * 60.0f,
                 [11] = 0.0f,
                 [12] = 2.0f * 60.0f * 60.0f,
                 [13] = 0.0f,
                 [14] = 24.0f * 60.0f * 60.0f,
            };
            Uint16 index_temp = Machine_slave.error_retry;
            if (index_temp >= sizeof(delay_table)/sizeof(float) - 1) index_temp = sizeof(delay_table)/sizeof(float) - 1;
            delay = fmaxf(delay_table[index_temp], 10.0f * Conv.Ts);

            static volatile union
            {
                Uint32 l[2];
                Uint64 ll;
            }convert;
            convert.ll = ReadIpcTimer() - delay_timer;
            static volatile float delay_timer_real;
            static volatile float U32H = (float)(0x100000000)/200e6;
            static volatile float U32L = (float)(0x1)/200e6;
            delay_timer_real = (float)convert.l[1] * U32H + (float)convert.l[0] * U32L;
            if(delay_timer_real > delay) Machine_slave.state = state_start;
        }
    }
}

void Machine_slave_class::calibrate_offsets()
{
    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;
    }

    if(ONOFF.ONOFF_last != ONOFF.ONOFF)
    {
        GPIO_SET(LED2_CM);
        memcpy(&SD_card.calibration.Meas_ACDC_gain, &Meas_ACDC_gain, sizeof(Meas_ACDC_gain));

        CIC2_calibration_input.ptr = &Meas_ACDC.I_grid.a;
        DELAY_US(100000);
        Meas_ACDC_offset.I_grid.a += CIC2_calibration.out / Meas_ACDC_gain.I_grid.a;

        CIC2_calibration_input.ptr = &Meas_ACDC.I_grid.b;
        DELAY_US(100000);
        Meas_ACDC_offset.I_grid.b += CIC2_calibration.out / Meas_ACDC_gain.I_grid.b;

        CIC2_calibration_input.ptr = &Meas_ACDC.I_grid.c;
        DELAY_US(100000);
        Meas_ACDC_offset.I_grid.c += CIC2_calibration.out / Meas_ACDC_gain.I_grid.c;


        CIC2_calibration_input.ptr = &Meas_ACDC.U_grid.a;
        DELAY_US(100000);
        Meas_ACDC_offset.U_grid.a += CIC2_calibration.out / Meas_ACDC_gain.U_grid.a;

        CIC2_calibration_input.ptr = &Meas_ACDC.U_grid.b;
        DELAY_US(100000);
        Meas_ACDC_offset.U_grid.b += CIC2_calibration.out / Meas_ACDC_gain.U_grid.b;

        CIC2_calibration_input.ptr = &Meas_ACDC.U_grid.c;
        DELAY_US(100000);
        Meas_ACDC_offset.U_grid.c += CIC2_calibration.out / Meas_ACDC_gain.U_grid.c;

        CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.a;
        DELAY_US(100000);
        Meas_ACDC_offset.I_conv.a += CIC2_calibration.out / Meas_ACDC_gain.I_conv.a;

        CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.b;
        DELAY_US(100000);
        Meas_ACDC_offset.I_conv.b += CIC2_calibration.out / Meas_ACDC_gain.I_conv.b;

        CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.c;
        DELAY_US(100000);
        Meas_ACDC_offset.I_conv.c += CIC2_calibration.out / Meas_ACDC_gain.I_conv.c;

        CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.n;
        DELAY_US(100000);
        Meas_ACDC_offset.I_conv.n += CIC2_calibration.out / Meas_ACDC_gain.I_conv.n;

        CIC2_calibration_input.ptr = &Meas_ACDC.U_dc;
        DELAY_US(100000);
        Meas_ACDC_offset.U_dc += CIC2_calibration.out / Meas_ACDC_gain.U_dc;

        CIC2_calibration_input.ptr = &Meas_ACDC.U_dc_n;
        DELAY_US(100000);
        Meas_ACDC_offset.U_dc_n += CIC2_calibration.out / Meas_ACDC_gain.U_dc_n;

        Meas_ACDC_offset_error.I_conv.a = fabsf(Meas_ACDC_offset.I_conv.a * Meas_ACDC_gain.I_conv.a);
        Meas_ACDC_offset_error.I_conv.b = fabsf(Meas_ACDC_offset.I_conv.b * Meas_ACDC_gain.I_conv.b);
        Meas_ACDC_offset_error.I_conv.c = fabsf(Meas_ACDC_offset.I_conv.c * Meas_ACDC_gain.I_conv.c);
        Meas_ACDC_offset_error.I_conv.n = fabsf(Meas_ACDC_offset.I_conv.n * Meas_ACDC_gain.I_conv.n);
        Meas_ACDC_offset_error.U_dc_n = fabsf(Meas_ACDC_offset.U_dc_n * Meas_ACDC_gain.U_dc_n);
        Meas_ACDC_offset_error.U_dc = fabsf(Meas_ACDC_offset.U_dc * Meas_ACDC_gain.U_dc);


        Meas_ACDC_offset_error.I_grid.a = fabsf(Meas_ACDC_offset.I_grid.a * Meas_ACDC_gain.I_grid.a);
        Meas_ACDC_offset_error.I_grid.b = fabsf(Meas_ACDC_offset.I_grid.b * Meas_ACDC_gain.I_grid.b);
        Meas_ACDC_offset_error.I_grid.c = fabsf(Meas_ACDC_offset.I_grid.c * Meas_ACDC_gain.I_grid.c);
        Meas_ACDC_offset_error.U_grid.a = fabsf(Meas_ACDC_offset.U_grid.a * Meas_ACDC_gain.U_grid.a);
        Meas_ACDC_offset_error.U_grid.b = fabsf(Meas_ACDC_offset.U_grid.b * Meas_ACDC_gain.U_grid.b);
        Meas_ACDC_offset_error.U_grid.c = fabsf(Meas_ACDC_offset.U_grid.c * Meas_ACDC_gain.U_grid.c);

        if (Meas_ACDC_offset_error.I_grid.a > 0.05f ||
            Meas_ACDC_offset_error.I_grid.b > 0.05f ||
            Meas_ACDC_offset_error.I_grid.c > 0.05f ||
            Meas_ACDC_offset_error.U_grid.a > 2.0f ||
            Meas_ACDC_offset_error.U_grid.b > 2.0f ||
            Meas_ACDC_offset_error.U_grid.c > 2.0f ||
            Meas_ACDC_offset_error.I_conv.a > 0.1f ||
            Meas_ACDC_offset_error.I_conv.b > 0.1f ||
            Meas_ACDC_offset_error.I_conv.c > 0.1f ||
            Meas_ACDC_offset_error.I_conv.n > 0.1f ||
            Meas_ACDC_offset_error.U_dc_n > 1.0f ||
            Meas_ACDC_offset_error.U_dc > 1.0f)
        {
            status_ACDC.calibration_procedure_error = 1;
        }
        else
        {
            status_ACDC.calibration_procedure_error = 0;
            Machine_slave.state = state_calibrate_curent_gain;
        }
    }
}

void Machine_slave_class::calibrate_curent_gain()
{
    static Uint16 calib_rdy;
    static Uint16 calib_rdy_last;
    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;
        calib_rdy = 0;
        calib_rdy_last = 0;
    }

    if(ONOFF.ONOFF_last != ONOFF.ONOFF)
    {
        GPIO_SET(LED2_CM);

        float I_cal = 2.0f;
        if(fabsf(Meas_ACDC.I_grid.a) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_grid.a;
            DELAY_US(100000);
            Meas_ACDC_gain.I_grid.a = fabsf(Meas_ACDC_gain.I_grid.a * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<0;
        }
        if(fabsf(Meas_ACDC.I_grid.b) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_grid.b;
            DELAY_US(100000);
            Meas_ACDC_gain.I_grid.b = fabsf(Meas_ACDC_gain.I_grid.b * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<1;
        }
        if(fabsf(Meas_ACDC.I_grid.c) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_grid.c;
            DELAY_US(100000);
            Meas_ACDC_gain.I_grid.c = fabsf(Meas_ACDC_gain.I_grid.c * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<2;
        }
        if(fabsf(Meas_ACDC.I_conv.a) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.a;
            DELAY_US(100000);
            Meas_ACDC_gain.I_conv.a = fabsf(Meas_ACDC_gain.I_conv.a * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<3;
        }
        if(fabsf(Meas_ACDC.I_conv.b) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.b;
            DELAY_US(100000);
            Meas_ACDC_gain.I_conv.b = fabsf(Meas_ACDC_gain.I_conv.b * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<4;
        }
        if(fabsf(Meas_ACDC.I_conv.c) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.c;
            DELAY_US(100000);
            Meas_ACDC_gain.I_conv.c = fabsf(Meas_ACDC_gain.I_conv.c * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<5;
        }
        if(fabsf(Meas_ACDC.I_conv.n) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.n;
            DELAY_US(100000);
            Meas_ACDC_gain.I_conv.n = fabsf(Meas_ACDC_gain.I_conv.n * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<6;
        }

        if(calib_rdy == calib_rdy_last)
        {
            status_ACDC.calibration_procedure_error = 1;
        }
        calib_rdy_last = calib_rdy;

        if(calib_rdy == 0x3F)
        {
            Meas_ACDC_gain_error.I_grid.a = fabsf((Meas_ACDC_gain.I_grid.a/SD_card.calibration.Meas_ACDC_gain.I_grid.a) - 1.0f);
            Meas_ACDC_gain_error.I_grid.b = fabsf((Meas_ACDC_gain.I_grid.b/SD_card.calibration.Meas_ACDC_gain.I_grid.b) - 1.0f);
            Meas_ACDC_gain_error.I_grid.c = fabsf((Meas_ACDC_gain.I_grid.c/SD_card.calibration.Meas_ACDC_gain.I_grid.c) - 1.0f);

            float mean_gain_meas = (Meas_ACDC_gain.I_conv.a + Meas_ACDC_gain.I_conv.b + Meas_ACDC_gain.I_conv.c) / 3.0f;
            Meas_ACDC_gain_error.I_conv.a = fabsf((Meas_ACDC_gain.I_conv.a/mean_gain_meas) - 1.0f);
            Meas_ACDC_gain_error.I_conv.b = fabsf((Meas_ACDC_gain.I_conv.b/mean_gain_meas) - 1.0f);
            Meas_ACDC_gain_error.I_conv.c = fabsf((Meas_ACDC_gain.I_conv.c/mean_gain_meas) - 1.0f);
            Meas_ACDC_gain_error.I_conv.n = fabsf((Meas_ACDC_gain.I_conv.n/mean_gain_meas) - 1.0f);

            if (Meas_ACDC_gain_error.I_grid.a > 0.03f ||
                Meas_ACDC_gain_error.I_grid.b > 0.03f ||
                Meas_ACDC_gain_error.I_grid.c > 0.03f ||
                Meas_ACDC_gain_error.I_conv.a > 0.03f ||
                Meas_ACDC_gain_error.I_conv.b > 0.03f ||
                Meas_ACDC_gain_error.I_conv.c > 0.03f ||
                Meas_ACDC_gain_error.I_conv.n > 0.03f)
            {
                status_ACDC.calibration_procedure_error = 1;
            }
            else
            {
                status_ACDC.calibration_procedure_error = 0;
                Machine_slave.state = state_calibrate_AC_voltage_gain;
            }
        }
    }
}

void Machine_slave_class::calibrate_AC_voltage_gain()
{
    static Uint16 calib_rdy;
    static Uint16 calib_rdy_last;
    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;
        calib_rdy = 0;
        calib_rdy_last = 0;
    }

    if(ONOFF.ONOFF_last != ONOFF.ONOFF)
    {
        GPIO_SET(LED2_CM);

        float U_cal = 28.0f;
        if(fabsf(Meas_ACDC.U_grid.a) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.U_grid.a;
            DELAY_US(100000);
            Meas_ACDC_gain.U_grid.a = fabsf(Meas_ACDC_gain.U_grid.a * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<0;
        }
        if(fabsf(Meas_ACDC.U_grid.b) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.U_grid.b;
            DELAY_US(100000);
            Meas_ACDC_gain.U_grid.b = fabsf(Meas_ACDC_gain.U_grid.b * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<1;
        }
        if(fabsf(Meas_ACDC.U_grid.c) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.U_grid.c;
            DELAY_US(100000);
            Meas_ACDC_gain.U_grid.c = fabsf(Meas_ACDC_gain.U_grid.c * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<2;
        }

        if(calib_rdy == calib_rdy_last)
        {
            status_ACDC.calibration_procedure_error = 1;
        }
        calib_rdy_last = calib_rdy;

        if(calib_rdy == 7)
        {
            Meas_ACDC_gain_error.U_grid.a = fabsf((Meas_ACDC_gain.U_grid.a/SD_card.calibration.Meas_ACDC_gain.U_grid.a) - 1.0f);
            Meas_ACDC_gain_error.U_grid.b = fabsf((Meas_ACDC_gain.U_grid.b/SD_card.calibration.Meas_ACDC_gain.U_grid.b) - 1.0f);
            Meas_ACDC_gain_error.U_grid.c = fabsf((Meas_ACDC_gain.U_grid.c/SD_card.calibration.Meas_ACDC_gain.U_grid.c) - 1.0f);

            if (Meas_ACDC_gain_error.U_grid.a > 0.03f ||
                Meas_ACDC_gain_error.U_grid.b > 0.03f ||
                Meas_ACDC_gain_error.U_grid.c > 0.03f)
            {
                status_ACDC.calibration_procedure_error = 1;
            }
            else
            {
                status_ACDC.calibration_procedure_error = 0;
                Machine_slave.state = state_calibrate_DC_voltage_gain;
            }
        }
    }
}

void Machine_slave_class::calibrate_DC_voltage_gain()
{
    static Uint16 calib_rdy;
    static Uint16 calib_rdy_last;
    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;
        calib_rdy = 0;
        calib_rdy_last = 0;
    }

    if(ONOFF.ONOFF_last != ONOFF.ONOFF)
    {
        GPIO_SET(LED2_CM);
        float U_cal = 28.0f;

        if(fabsf(Meas_ACDC.U_dc) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.U_dc;
            DELAY_US(100000);
            Meas_ACDC_gain.U_dc = fabsf(Meas_ACDC_gain.U_dc * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<0;
        }

        if(fabsf(Meas_ACDC.U_dc_n) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_ACDC.U_dc_n;
            DELAY_US(100000);
            Meas_ACDC_gain.U_dc_n = fabsf(Meas_ACDC_gain.U_dc_n * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<1;
        }

        if(calib_rdy == calib_rdy_last)
        {
            status_ACDC.calibration_procedure_error = 1;
        }
        calib_rdy_last = calib_rdy;

        if(calib_rdy == 3)
        {
            Meas_ACDC_gain_error.U_dc = fabsf((Meas_ACDC_gain.U_dc/SD_card.calibration.Meas_ACDC_gain.U_dc) - 1.0f);
            Meas_ACDC_gain_error.U_dc_n = fabsf((Meas_ACDC_gain.U_dc_n/SD_card.calibration.Meas_ACDC_gain.U_dc_n) - 1.0f);

            if (Meas_ACDC_gain_error.U_dc_n > 0.03f ||
                Meas_ACDC_gain_error.U_dc > 0.03f)
            {
                status_ACDC.calibration_procedure_error = 1;
            }
            else
            {
                status_ACDC.calibration_procedure_error = 0;
                memcpy(&SD_card.calibration.Meas_ACDC_gain, &Meas_ACDC_gain, sizeof(Meas_ACDC_gain));
                memcpy(&SD_card.calibration.Meas_ACDC_offset, &Meas_ACDC_offset, sizeof(Meas_ACDC_offset));
                SD_card.save_calibration_data();
                Machine_slave.state = state_idle;
                control_ACDC.triggers.bit.CPU_reset = 1;
            }
        }
        else
        {
            status_ACDC.calibration_procedure_error = 1;
        }
    }

}

void Machine_slave_class::start()
{
    static Uint16 state;
    static Uint32 delay_timer;

    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;

        if(Machine_slave.error_retry) Machine_slave.recent_error = 1;
        if(++Machine_slave.error_retry >= 16) Machine_slave.error_retry = 15;
        status_ACDC.error_retry = Machine_slave.error_retry;
        error_retry_FLASH.save();

        memset(&Timer_total, 0, sizeof(Timer_total));
        timer_update(&Timer_total, 0);

        Conv.PI_U_dc.integrator =
        Conv.PI_Id[0].integrator =
        Conv.PI_Id[1].integrator =
        Conv.PI_Id[2].integrator =
        Conv.PI_Iq[0].integrator =
        Conv.PI_Iq[1].integrator =
        Conv.PI_Iq[2].integrator = 0.0f;

        Init.clear_alarms();

        DELAY_US(100);


        EMIF_mem.write.Scope_acquire_before_trigger = (EMIF_mem.read.Scope_depth / EMIF_mem.read.Scope_width_mult)>>1;
        EMIF_mem.write.Scope_trigger = 0;
        Scope_start();

        if( mosfet_ctrl_app.getState() != MosfetCtrlApp::state_idle ){
            uint16_t mosfet_errors = mosfet_ctrl_app.getErrorsGetStatus();
            mosfet_errors |= mosfet_ctrl_app.getErrorsConfig();
            mosfet_errors |= mosfet_ctrl_app.getErrorsAtPoint(MosfetCtrlApp::logpoint_softreset);

            if( (mosfet_errors & 0xFF) == 0 )
                mosfet_errors = 0xFF;

            alarm_ACDC.bit.Driver_soft_error = 1;
        }

        CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.a;

        state = 0;
        delay_timer = IpcRegs.IPCCOUNTERL;
    }

    float compare_delay = 0.05f;

    switch(state)
    {
        case 0:
        {
            if((float)(IpcRegs.IPCCOUNTERL - delay_timer) * (1.0f/200000000.0f) > compare_delay)
            {
                Meas_ACDC_offset.I_conv.a += CIC2_calibration.out / Meas_ACDC_gain.I_conv.a;
                CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.b;
                delay_timer = IpcRegs.IPCCOUNTERL;
                state++;
            }
            break;
        }
        case 1:
        {
            if((float)(IpcRegs.IPCCOUNTERL - delay_timer) * (1.0f/200000000.0f) > compare_delay)
            {
                Meas_ACDC_offset.I_conv.b += CIC2_calibration.out / Meas_ACDC_gain.I_conv.b;
                CIC2_calibration_input.ptr = &Meas_ACDC.I_conv.c;
                delay_timer = IpcRegs.IPCCOUNTERL;
                state++;
            }
            break;
        }
        case 2:
        {
            if((float)(IpcRegs.IPCCOUNTERL - delay_timer) * (1.0f/200000000.0f) > compare_delay)
            {
                Meas_ACDC_offset.I_conv.c += CIC2_calibration.out / Meas_ACDC_gain.I_conv.c;
                delay_timer = IpcRegs.IPCCOUNTERL;
                state++;
            }
            break;
        }
        case 3:
        {
            Conv.enable = 1;
            break;
        }
    }

    if(Conv.RDY2) Machine_slave.state = state_operational;

    if(!ONOFF.ONOFF) Machine_slave.state = state_idle;
    if(alarm_ACDC.all[0] | alarm_ACDC.all[1] | alarm_ACDC.all[2]) Machine_slave.state = state_cleanup;
}

void Machine_slave_class::operational()
{
    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;
    }

    timer_update(&Timer_total, 1);

    if(!ONOFF.ONOFF) Machine_slave.state = state_idle;
    if(alarm_ACDC.all[0] | alarm_ACDC.all[1] | alarm_ACDC.all[2]) Machine_slave.state = state_cleanup;
}

void Machine_slave_class::cleanup()
{
    if(Machine_slave.state_last != Machine_slave.state)
    {
        Machine_slave.state_last = Machine_slave.state;
        Conv.enable = 0;
        Scope_trigger_unc();
        EMIF_mem.write.Scope_trigger = 1;
        Machine_slave.recent_error = 1;
        if(mosfet_ctrl_app.getState() == MosfetCtrlApp::state_error){
            //gdy wystapil jakis blad to proba zrestartowania maszyn stanowych
            mosfet_ctrl_app.process_event(MosfetCtrlApp::event_restart, NULL);
        }
        mosfet_ctrl_app.process_event( MosfetCtrlApp::event_get_status );
    }

    if(Scope.finished_sorting && !status_ACDC.Scope_snapshot_pending && !control_ACDC.triggers.bit.Scope_snapshot
            && mosfet_ctrl_app.finished() && EMIF_mem.read.Scope_rdy)
    {
        SD_card.save_state();
        EMIF_mem.write.Scope_trigger = 0;
        if(Machine_slave.error_retry == 4) control_ACDC.triggers.bit.CPU_reset = 1;
        Machine_slave.state = state_idle;
    }
}
