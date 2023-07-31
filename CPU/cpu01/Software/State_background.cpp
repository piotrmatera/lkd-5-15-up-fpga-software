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
#include "State_slave.h"

#include "Scope.h"
#include "SD_card.h"
#include "Init.h"
#include "FLASH.h"
#include "Blink.h"
#include "Rtc.h"
#include "diskio.h"
#include "Modbus_Converter_memory.h"
#include "Modbus_devices.h"

#include "Fiber_comm_master.h"
#include "Fiber_comm_slave.h"

#include "MosfetCtrlApp.h"

FATFS fs;           /* Filesystem object */
MosfetCtrlApp mosfet_ctrl_app;

Rtc rtc;
struct time_BCD_struct RTC_current_time;
struct time_BCD_struct RTC_new_time;

class Background_class Background;

struct CT_calc_struct CT_char_vars;
struct ONOFF_struct ONOFF;

class FLASH_class switch_FLASH =
{
 .address = {(Uint16 *)&ONOFF.ONOFF_FLASH, 0},
 .sector = SectorM,
 .size16_each = {sizeof(ONOFF.ONOFF_FLASH), 0},
};

void CT_char_calc()
{
    if(CT_char_vars.CT_char.available && CT_char_vars.calibration.available)
    {
        if(CT_char_vars.CT_char.number_of_elements == 1)
        {
            Meas_ACDC_gain.I_grid.a = CT_char_vars.calibration.Meas_ACDC_gain.I_grid.a * fabsf(CT_char_vars.CT_char.CT_ratio_a[0]);
            Meas_ACDC_gain.I_grid.b = CT_char_vars.calibration.Meas_ACDC_gain.I_grid.b * fabsf(CT_char_vars.CT_char.CT_ratio_b[0]);
            Meas_ACDC_gain.I_grid.c = CT_char_vars.calibration.Meas_ACDC_gain.I_grid.c * fabsf(CT_char_vars.CT_char.CT_ratio_c[0]);
            register float degrees_kTs = 0.02f / (360.0f * Conv.Ts);
            CT_char_vars.CT_phase[0] = CT_char_vars.CT_char.phase_a[0] * degrees_kTs;
            CT_char_vars.CT_phase[1] = CT_char_vars.CT_char.phase_b[0] * degrees_kTs;
            CT_char_vars.CT_phase[2] = CT_char_vars.CT_char.phase_c[0] * degrees_kTs;
        }
        else
        {
            struct abc_struct X_point[2];
            X_point[0].a = CT_char_vars.CT_char.set_current[CT_char_vars.CT_char_index[0]];
            X_point[1].a = CT_char_vars.CT_char.set_current[CT_char_vars.CT_char_index[0]+1];
            X_point[0].b = CT_char_vars.CT_char.set_current[CT_char_vars.CT_char_index[1]];
            X_point[1].b = CT_char_vars.CT_char.set_current[CT_char_vars.CT_char_index[1]+1];
            X_point[0].c = CT_char_vars.CT_char.set_current[CT_char_vars.CT_char_index[2]];
            X_point[1].c = CT_char_vars.CT_char.set_current[CT_char_vars.CT_char_index[2]+1];

            if(X_point[0].a == X_point[1].a || X_point[0].b == X_point[1].b || X_point[0].c == X_point[1].c)
                alarm_ACDC.bit.CT_char_error = 1;

            struct abc_struct X_ratio;
            X_ratio.a = (Grid.I_grid.a - X_point[0].a) / (X_point[1].a - X_point[0].a);
            X_ratio.b = (Grid.I_grid.b - X_point[0].b) / (X_point[1].b - X_point[0].b);
            X_ratio.c = (Grid.I_grid.c - X_point[0].c) / (X_point[1].c - X_point[0].c);

            X_ratio.a = Saturation(X_ratio.a, 0.0f, 1.0f);
            X_ratio.b = Saturation(X_ratio.b, 0.0f, 1.0f);
            X_ratio.c = Saturation(X_ratio.c, 0.0f, 1.0f);

            struct abc_struct Y_point[2];
            struct abc_struct Y_interp;
            Y_point[0].a = CT_char_vars.CT_char.CT_ratio_a[CT_char_vars.CT_char_index[0]];
            Y_point[1].a = CT_char_vars.CT_char.CT_ratio_a[CT_char_vars.CT_char_index[0]+1];
            Y_point[0].b = CT_char_vars.CT_char.CT_ratio_b[CT_char_vars.CT_char_index[1]];
            Y_point[1].b = CT_char_vars.CT_char.CT_ratio_b[CT_char_vars.CT_char_index[1]+1];
            Y_point[0].c = CT_char_vars.CT_char.CT_ratio_c[CT_char_vars.CT_char_index[2]];
            Y_point[1].c = CT_char_vars.CT_char.CT_ratio_c[CT_char_vars.CT_char_index[2]+1];
            Y_interp.a = Y_point[0].a + X_ratio.a * (Y_point[1].a - Y_point[0].a);
            Y_interp.b = Y_point[0].b + X_ratio.b * (Y_point[1].b - Y_point[0].b);
            Y_interp.c = Y_point[0].c + X_ratio.c * (Y_point[1].c - Y_point[0].c);
            Meas_ACDC_gain.I_grid.a = CT_char_vars.calibration.Meas_ACDC_gain.I_grid.a * fabsf(Y_interp.a);
            Meas_ACDC_gain.I_grid.b = CT_char_vars.calibration.Meas_ACDC_gain.I_grid.b * fabsf(Y_interp.b);
            Meas_ACDC_gain.I_grid.c = CT_char_vars.calibration.Meas_ACDC_gain.I_grid.c * fabsf(Y_interp.c);

            Y_point[0].a = CT_char_vars.CT_char.phase_a[CT_char_vars.CT_char_index[0]];
            Y_point[1].a = CT_char_vars.CT_char.phase_a[CT_char_vars.CT_char_index[0]+1];
            Y_point[0].b = CT_char_vars.CT_char.phase_b[CT_char_vars.CT_char_index[1]];
            Y_point[1].b = CT_char_vars.CT_char.phase_b[CT_char_vars.CT_char_index[1]+1];
            Y_point[0].c = CT_char_vars.CT_char.phase_c[CT_char_vars.CT_char_index[2]];
            Y_point[1].c = CT_char_vars.CT_char.phase_c[CT_char_vars.CT_char_index[2]+1];
            Y_interp.a = Y_point[0].a + X_ratio.a * (Y_point[1].a - Y_point[0].a);
            Y_interp.b = Y_point[0].b + X_ratio.b * (Y_point[1].b - Y_point[0].b);
            Y_interp.c = Y_point[0].c + X_ratio.c * (Y_point[1].c - Y_point[0].c);
            register float degrees_kTs = 0.02f / (360.0f * Conv.Ts);
            CT_char_vars.CT_phase[0] =  - Y_interp.a * degrees_kTs;
            CT_char_vars.CT_phase[1] =  - Y_interp.b * degrees_kTs;
            CT_char_vars.CT_phase[2] =  - Y_interp.c * degrees_kTs;

            if(X_ratio.a == 1.0f)
                if(++CT_char_vars.CT_char_index[0] > CT_char_vars.CT_char.number_of_elements-2) CT_char_vars.CT_char_index[0]--;
            if(X_ratio.a == 0.0f)
                if(--CT_char_vars.CT_char_index[0] == 0xFFFF) CT_char_vars.CT_char_index[0]++;
            if(X_ratio.b == 1.0f)
                if(++CT_char_vars.CT_char_index[1] > CT_char_vars.CT_char.number_of_elements-2) CT_char_vars.CT_char_index[1]--;
            if(X_ratio.b == 0.0f)
                if(--CT_char_vars.CT_char_index[1] == 0xFFFF) CT_char_vars.CT_char_index[1]++;
            if(X_ratio.c == 1.0f)
                if(++CT_char_vars.CT_char_index[2] > CT_char_vars.CT_char.number_of_elements-2) CT_char_vars.CT_char_index[2]--;
            if(X_ratio.c == 0.0f)
                if(--CT_char_vars.CT_char_index[2] == 0xFFFF) CT_char_vars.CT_char_index[2]++;
        }
    }
    else
    {
        CT_char_vars.CT_phase[0] =
        CT_char_vars.CT_phase[1] =
        CT_char_vars.CT_phase[2] = 0.0f;
    }

    float rotation;
    float wTs = Conv.w_filter * Conv.Ts;
    rotation = CT_char_vars.CT_phase[0] * wTs;
    Conv.I_grid_rot[0].sine = sinf(rotation);
    Conv.I_grid_rot[0].cosine = cosf(rotation);
    rotation = CT_char_vars.CT_phase[1] * wTs;
    Conv.I_grid_rot[1].sine = sinf(rotation);
    Conv.I_grid_rot[1].cosine = cosf(rotation);
    rotation = CT_char_vars.CT_phase[2] * wTs;
    Conv.I_grid_rot[2].sine = sinf(rotation);
    Conv.I_grid_rot[2].cosine = cosf(rotation);

    CPU1toCPU2.Meas_ACDC_gain = Meas_ACDC_gain;
    CPU1toCPU2.Meas_ACDC_offset = Meas_ACDC_offset;
    IpcRegs.IPCSET.bit.IPC4 = 1;
}

void convert_harmonics_to_bits()
{
    {
        register Uint32 temp_harmonic_a = 0;
        register Uint32 temp_harmonic_b = 0;
        register Uint32 temp_harmonic_c = 0;
        register float *source_harmonic_a = on_off_odd_a;
        register float *source_harmonic_b = on_off_odd_b;
        register float *source_harmonic_c = on_off_odd_c;

        Uint16 i;
        for(i = 0; i<sizeof(on_off_odd_a)/sizeof(on_off_odd_a[0]); i++)
        {
            temp_harmonic_a |= (Uint32)((Uint16)*source_harmonic_a++ & 0x01)<<i;
            temp_harmonic_b |= (Uint32)((Uint16)*source_harmonic_b++ & 0x01)<<i;
            temp_harmonic_c |= (Uint32)((Uint16)*source_harmonic_c++ & 0x01)<<i;
        }
        *(Uint32 *)&control_ACDC.H_odd_a = temp_harmonic_a;
        *(Uint32 *)&control_ACDC.H_odd_b = temp_harmonic_b;
        *(Uint32 *)&control_ACDC.H_odd_c = temp_harmonic_c;
    }

    {
        register Uint32 temp_harmonic_a = 0;
        register Uint32 temp_harmonic_b = 0;
        register Uint32 temp_harmonic_c = 0;
        register float *source_harmonic_a = on_off_even_a;
        register float *source_harmonic_b = on_off_even_b;
        register float *source_harmonic_c = on_off_even_c;

        Uint16 i;
        for(i = 0; i<sizeof(on_off_even_a)/sizeof(on_off_even_a[0]); i++)
        {
            temp_harmonic_a |= (Uint32)((Uint16)*source_harmonic_a++ & 0x01)<<i;
            temp_harmonic_b |= (Uint32)((Uint16)*source_harmonic_b++ & 0x01)<<i;
            temp_harmonic_c |= (Uint32)((Uint16)*source_harmonic_c++ & 0x01)<<i;
        }
        *(Uint32 *)&control_ACDC.H_even_a = temp_harmonic_a;
        *(Uint32 *)&control_ACDC.H_even_b = temp_harmonic_b;
        *(Uint32 *)&control_ACDC.H_even_c = temp_harmonic_c;
    }
}

void convert_harmonics_to_floats()
{
    {
        Uint16 i;
        register Uint32 temp_harmonic_a = *(Uint32 *)&control_ACDC.H_odd_a;
        register Uint32 temp_harmonic_b = *(Uint32 *)&control_ACDC.H_odd_b;
        register Uint32 temp_harmonic_c = *(Uint32 *)&control_ACDC.H_odd_c;
        register float *dest_harmonic_a = on_off_odd_a;
        register float *dest_harmonic_b = on_off_odd_b;
        register float *dest_harmonic_c = on_off_odd_c;
        for(i = 1; i<sizeof(on_off_odd_a)/sizeof(on_off_odd_a[0]); i++)
        {
            *dest_harmonic_a++ = (temp_harmonic_a >> i) & 0x01;
            *dest_harmonic_b++ = (temp_harmonic_b >> i) & 0x01;
            *dest_harmonic_c++ = (temp_harmonic_c >> i) & 0x01;
        }
    }

    {
        Uint16 i;
        register Uint32 temp_harmonic_a = *(Uint32 *)&control_ACDC.H_even_a;
        register Uint32 temp_harmonic_b = *(Uint32 *)&control_ACDC.H_even_b;
        register Uint32 temp_harmonic_c = *(Uint32 *)&control_ACDC.H_even_c;
        register float *dest_harmonic_a = on_off_even_a;
        register float *dest_harmonic_b = on_off_even_b;
        register float *dest_harmonic_c = on_off_even_c;
        for(i = 0; i<sizeof(on_off_even_a)/sizeof(on_off_even_a[0]); i++)
        {
            *dest_harmonic_a++ = (temp_harmonic_a >> i) & 0x01;
            *dest_harmonic_b++ = (temp_harmonic_b >> i) & 0x01;
            *dest_harmonic_c++ = (temp_harmonic_c >> i) & 0x01;
        }
    }
}

void update_harmonics()
{
    on_off_odd_a[0] =
    on_off_odd_b[0] =
    on_off_odd_c[0] = 0.0f;
    *(Uint32 *)&control_ACDC.H_odd_a &= 0xFFFFFFFE;
    *(Uint32 *)&control_ACDC.H_odd_b &= 0xFFFFFFFE;
    *(Uint32 *)&control_ACDC.H_odd_c &= 0xFFFFFFFE;

    if(Background.harmonics_odd_last != Background.harmonics_odd)
    {
        Background.harmonics_odd_last = Background.harmonics_odd;
        if(Background.harmonics_odd > 24) Background.harmonics_odd = 24;
        Uint16 count = 1;
        while(count < Background.harmonics_odd + 1)
        {
            on_off_odd_a[count] =
            on_off_odd_b[count] =
            on_off_odd_c[count] = 1.0f;
            count++;
        }

        while(count < 25)
        {
            on_off_odd_a[count] =
            on_off_odd_b[count] =
            on_off_odd_c[count] = 0.0f;
            count++;
        }
        convert_harmonics_to_bits();
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    if(Background.harmonics_even_last != Background.harmonics_even)
    {
        Background.harmonics_even_last = Background.harmonics_even;
        if(Background.harmonics_even > 25) Background.harmonics_even = 25;
        Uint16 count = 0;
        while(count < Background.harmonics_even)
        {
            on_off_even_a[count] =
            on_off_even_b[count] =
            on_off_even_c[count] = 1.0f;
            count++;
        }

        while(count < 25)
        {
            on_off_even_a[count] =
            on_off_even_b[count] =
            on_off_even_c[count] = 0.0f;
            count++;
        }
        convert_harmonics_to_bits();
    }
}


void Blink()
{
    static class Blink_class Blink_LED1(false);
    static class Blink_class Blink_LED2(false);
    static class Blink_class Blink_LED3(false);
    static class Blink_class Blink_LED4(false);
    static class Blink_class Blink_LED5(false);

    switch(Machine_slave.state)
    {
        case Machine_slave_class::state_calibrate_offsets:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -1.1f, -1.1f};
            static const float period = 1.1f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_ACDC.calibration_procedure_error);
            break;
        }

        case Machine_slave_class::state_calibrate_curent_gain:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -0.2f, 0.3f, -1.3f, -1.3f};
            static const float period = 1.3f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_ACDC.calibration_procedure_error);
            break;
        }

        case Machine_slave_class::state_calibrate_AC_voltage_gain:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -0.2f, 0.3f, -0.4f, 0.5f, -1.5f, -1.5f};
            static const float period = 1.5f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_ACDC.calibration_procedure_error);
            break;
        }

        case Machine_slave_class::state_calibrate_DC_voltage_gain:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -0.2f, 0.3f, -0.4f, 0.5f, -0.6f, 0.7f, -1.7f, -1.7f};
            static const float period = 1.5f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_ACDC.calibration_procedure_error);
            break;
        }
        default:
        {
            if(ONOFF.ONOFF)
            {
                if(Machine_slave.state == Machine_slave_class::state_operational)
                {
                    if(status_ACDC.master_slave_selector)
                    {
                        if(status_ACDC.no_CT_connected_a || status_ACDC.no_CT_connected_b || status_ACDC.no_CT_connected_c) Blink_LED1.update_pattern(0.2f, 0.5f);
                        else Blink_LED1.update_pattern(true);
                    }
                    else
                    {
                        if(status_ACDC_master.no_CT_connected_a || status_ACDC_master.no_CT_connected_b || status_ACDC_master.no_CT_connected_c) Blink_LED1.update_pattern(0.2f, 0.5f);
                        else Blink_LED1.update_pattern(true);
                    }
                }
                else Blink_LED1.update_pattern(2.0f, 0.5f);
            }
            else Blink_LED1.update_pattern(false);

            if(alarm_ACDC.all[0] | alarm_ACDC.all[1] | alarm_ACDC.all[2])
            {
                Blink_LED2.update_pattern(false);
                Blink_LED3.update_pattern(0.67f, 0.5f);
            }
            else
            {
                if(Blink_LED2.zero_crossing)
                {
                    if(status_ACDC.master_slave_selector)
                    {
                        if(status_ACDC.in_limit_Q)
                            Blink_LED2.update_pattern(true);
                        else if(status_ACDC.in_limit_P)
                            Blink_LED2.update_pattern(2.0f, 0.67f);
                        else if(status_ACDC.in_limit_H)
                            Blink_LED2.update_pattern(2.0f, 0.33f);
                        else
                            Blink_LED2.update_pattern(false);
                    }
                    else
                    {
                        if(status_ACDC_master.in_limit_Q)
                            Blink_LED2.update_pattern(true);
                        else if(status_ACDC_master.in_limit_P)
                            Blink_LED2.update_pattern(2.0f, 0.67f);
                        else if(status_ACDC_master.in_limit_H)
                            Blink_LED2.update_pattern(2.0f, 0.33f);
                        else
                            Blink_LED2.update_pattern(false);
                    }
                }
                if(!status_ACDC.master_rdy && !status_ACDC.master_slave_selector) Blink_LED3.update_pattern(true);
                else Blink_LED3.update_pattern(false);
            }
            break;
        }
    }

    if(Machine_slave.recent_error && Conv.enable) Blink_LED4.update_pattern(true);
    else Blink_LED4.update_pattern(false);

    if(status_ACDC.wifi_on) Blink_LED5.update_pattern(true);
    else Blink_LED5.update_pattern(false);

    GPIO_WRITE(LED1_CM, Blink_LED1.task());
    GPIO_WRITE(LED2_CM, Blink_LED2.task());
    GPIO_WRITE(LED3_CM, Blink_LED3.task());
    GPIO_WRITE(LED4_CM, Blink_LED4.task());
    GPIO_WRITE(LED5_CM, Blink_LED5.task());
}

#pragma CODE_SECTION(".TI.ramfunc");
void ONOFF_switch_interrupt()
{
    float switch_timer_temp = ONOFF.switch_timer;
    if(GPIO_READ(ON_OFF_CM)) ONOFF.switch_timer = 0.0f;
    else ONOFF.switch_timer += Conv.Ts;

    ONOFF.ONOFF_switch_last = ONOFF.ONOFF_switch;
    if(ONOFF.switch_timer > 0.05f) ONOFF.ONOFF_switch = 1;
    else ONOFF.ONOFF_switch = 0;

    if( ONOFF.switch_timer >= 10.0f && switch_timer_temp < 10.0f ){
        status_ACDC.wifi_on ^= 1;
        control_ACDC.triggers.bit.SD_save_settings = 1;
    }

    if(ONOFF.ONOFF == ONOFF.ONOFF_temp)
    {
        if(switch_timer_temp > 30.0f && switch_timer_temp < 1e6)
        {
            control_ACDC.triggers.bit.CPU_reset = 1;
        }
        else if(switch_timer_temp > 2.0f)
        {
            Machine_slave.recent_error = 0;
        }
        else if(ONOFF.ONOFF_switch_last && !ONOFF.ONOFF_switch)
        {
            ONOFF.ONOFF_temp = ONOFF.ONOFF ^ 1;
            if(ONOFF.ONOFF_temp) Machine_slave.recent_error = 0;
        }
    }
}

void ONOFF_switch_func()
{
    ONOFF.ONOFF_last = ONOFF.ONOFF;
    ONOFF.ONOFF = ONOFF.ONOFF_temp;
    status_ACDC.ONOFF = ONOFF.ONOFF;

    if (ONOFF.ONOFF_FLASH != ONOFF.ONOFF)
    {
        ONOFF.ONOFF_FLASH = ONOFF.ONOFF;
        switch_FLASH.save();
    }
}

void Background_class::init()
{
    rtc.init();
    rtc.process_event(Rtc::event_init);
    RTC_new_time.second = 0;
    RTC_new_time.second10 = 0;
    RTC_new_time.minute = 9;
    RTC_new_time.minute10 = 4;
    RTC_new_time.hour = 1;
    RTC_new_time.hour10 = 1;
    RTC_new_time.day = 8;
    RTC_new_time.day10 = 0;
    RTC_new_time.month = 3;
    RTC_new_time.month10 = 0;
    RTC_new_time.year = 1;
    RTC_new_time.year10 = 2;

    FatFS_time.second_2 = 5;
    FatFS_time.minute = 10;
    FatFS_time.hour = 10;
    FatFS_time.day = 10;
    FatFS_time.month = 10;
    FatFS_time.year = 10 + 20;

    f_mount(&fs, "", 1);

    mosfet_ctrl_app.init();

    if(EMIF_mem.read.cycle_period != CYCLE_PERIOD) alarm_ACDC.bit.FPGA_parameters = 1;
    if(EMIF_mem.read.control_rate != CONTROL_RATE) alarm_ACDC.bit.FPGA_parameters = 1;

    memset(&Modbus_Converter.coils, 0, sizeof(Modbus_Converter.coils));
    memset(&Modbus_Converter.discrete_inputs, 0, sizeof(Modbus_Converter.discrete_inputs));
    memset(&Modbus_Converter.holding_registers, 0, sizeof(Modbus_Converter.holding_registers));
    memset(&Modbus_Converter.input_registers, 0, sizeof(Modbus_Converter.input_registers));
    memset(&Machine_master, 0, sizeof(Machine_master));
    memset(&Machine_slave, 0, sizeof(Machine_slave));
    memset(&Conv, 0, sizeof(Conv));
    memset(&Meas_ACDC, 0, sizeof(Meas_ACDC));
    memset(&Meas_ACDC_offset, 0, sizeof(Meas_ACDC_offset));
    memset(&Meas_ACDC_gain, 0, sizeof(Meas_ACDC_gain));
    memset(&Meas_ACDC_alarm_H, 0, sizeof(Meas_ACDC_alarm_H));
    memset(&Meas_ACDC_alarm_L, 0, sizeof(Meas_ACDC_alarm_L));
    memset(&CPU1toCPU2, 0, sizeof(CPU1toCPU2));
    memset(&CT_char_vars, 0, sizeof(CT_char_vars));
    memset(&Energy_meter, 0, sizeof(Energy_meter));
    memset(&Grid, 0, sizeof(Grid));
    memset(&Grid_filter, 0, sizeof(Grid_filter));
    memset(&Grid_params, 0, sizeof(Grid_params));
    memset(&Grid_filter_params, 0, sizeof(Grid_filter_params));

    memset(&control_ACDC, 0, sizeof(control_ACDC));
    memset(&control_ext_modbus, 0, sizeof(control_ext_modbus));
    memset(&status_ACDC, 0, sizeof(status_ACDC));
    memset(&alarm_ACDC, 0, sizeof(alarm_ACDC));
    memset(&alarm_ACDC_snapshot, 0, sizeof(alarm_ACDC_snapshot));

    Fiber_comm_master[0].node_number = 0;
    Fiber_comm_master[1].node_number = 1;
    Fiber_comm_master[2].node_number = 2;
    Fiber_comm_master[3].node_number = 3;

    if(L_grid_FLASH.retrieve()) L_grid_meas.L_grid_previous[0] = 100e-6;
    error_retry_FLASH.retrieve();
    status_ACDC.error_retry = Machine_slave.error_retry;

    SD_card.read_settings();

    SD_card.read_CT_characteristic();
    SD_card.read_H_settings();
    SD_card.read_calibration_data();
    SD_card.read_meter_data();
    if(SD_card.meter.available) memcpy(&Energy_meter.upper, &SD_card.meter.Energy_meter, sizeof(Energy_meter.upper));
    else status_ACDC.SD_no_meter = 1;

    if(!SD_card.harmonics.available || !SD_card.settings.available || !SD_card.calibration.available || !SD_card.CT_char.available)
        status_ACDC.SD_card_not_enough_data = 1;
    else
        status_ACDC.SD_card_not_enough_data = 0;


    if(!SD_card.settings.available)
    {
        Modbus_slave_EXT_translated.slave_address = 240;
        Modbus_slave_EXT.slave_address = MODBUS_EXT_ADDRESS;
        status_ACDC.wifi_on = 1;
    }
    else
    {
        Modbus_slave_EXT_translated.slave_address = SD_card.settings.modbus_ext_server_id;
        Modbus_slave_EXT.slave_address = MODBUS_EXT_ADDRESS;
        status_ACDC.wifi_on = SD_card.settings.wifi_on;
    }

    if(!SD_card.CT_char.available) status_ACDC.SD_no_CT_characteristic = 1;
    if(!SD_card.calibration.available) status_ACDC.SD_no_calibration = 1;
    if(!SD_card.settings.available) status_ACDC.SD_no_settings = 1;
    if(!SD_card.harmonics.available) status_ACDC.SD_no_harmonic_settings = 1;

    if(status_ACDC.SD_card_not_enough_data)
    {
        alarm_ACDC.bit.Not_enough_data_master = 1;

        Meas_ACDC_gain.def_osr = EMIF_mem.read.def_osr;
        Meas_ACDC_gain.sd_shift = EMIF_mem.read.sd_shift;

        Meas_ACDC_gain.U_grid.a =
        Meas_ACDC_gain.U_grid.b =
        Meas_ACDC_gain.U_grid.c = 0.064/(Meas_ACDC_gain.def_osr*Meas_ACDC_gain.def_osr)*Meas_ACDC_gain.sd_shift*(680.0*3.0 + 0.27)/(0.27)*(1.0 + (0.54/4.9));

        Meas_ACDC_gain.I_grid.a =
        Meas_ACDC_gain.I_grid.b =
        Meas_ACDC_gain.I_grid.c = 0.064/(Meas_ACDC_gain.def_osr*Meas_ACDC_gain.def_osr)*Meas_ACDC_gain.sd_shift/0.005;

        Meas_ACDC_gain.U_dc = 0.064/(Meas_ACDC_gain.def_osr*Meas_ACDC_gain.def_osr)*Meas_ACDC_gain.sd_shift*(680.0*6.0 + 0.24)/(0.24)*(1.0 + (0.48/4.9));

        Meas_ACDC_gain.U_dc_n = 0.064/(Meas_ACDC_gain.def_osr*Meas_ACDC_gain.def_osr)*Meas_ACDC_gain.sd_shift*(680.0*3.0 + 0.24)/(0.24)*(1.0 + (0.48/4.9));

        Meas_ACDC_gain.I_conv.a =
        Meas_ACDC_gain.I_conv.b =
        Meas_ACDC_gain.I_conv.c =
        Meas_ACDC_gain.I_conv.n = 0.064/(Meas_ACDC_gain.def_osr*Meas_ACDC_gain.def_osr)*Meas_ACDC_gain.sd_shift/0.001;

        control_ext_modbus.fields.baudrate = 1152;
        control_ext_modbus.fields.ext_server_id = 1;

        SD_card.settings.C_dc = 990e-6;
        SD_card.settings.L_conv = 265e-6;
        SD_card.settings.C_conv = 12.5e-6 + 4.4e-6;
        SD_card.settings.I_lim =
        Conv.I_lim_nominal = 24.0f;
    }
    else
    {
        control_ACDC.Q_set = SD_card.settings.control.Q_set;
        control_ACDC.flags.bit.enable_Q_comp_a = SD_card.settings.control.flags.bit.enable_Q_comp_a;
        control_ACDC.flags.bit.enable_Q_comp_b = SD_card.settings.control.flags.bit.enable_Q_comp_b;
        control_ACDC.flags.bit.enable_Q_comp_c = SD_card.settings.control.flags.bit.enable_Q_comp_c;
        control_ACDC.flags.bit.enable_P_sym =  SD_card.settings.control.flags.bit.enable_P_sym;
        control_ACDC.flags.bit.enable_H_comp = SD_card.settings.control.flags.bit.enable_H_comp;
        control_ACDC.flags.bit.version_P_sym = SD_card.settings.control.flags.bit.version_P_sym;
        control_ACDC.flags.bit.version_Q_comp_a = SD_card.settings.control.flags.bit.version_Q_comp_a;
        control_ACDC.flags.bit.version_Q_comp_b = SD_card.settings.control.flags.bit.version_Q_comp_b;
        control_ACDC.flags.bit.version_Q_comp_c = SD_card.settings.control.flags.bit.version_Q_comp_c;
        control_ACDC.tangens_range[0] = SD_card.settings.control.tangens_range[0];
        control_ACDC.tangens_range[1] = SD_card.settings.control.tangens_range[1];
        control_ext_modbus.fields.baudrate = SD_card.settings.Baudrate/100;
        control_ext_modbus.fields.ext_server_id = SD_card.settings.modbus_ext_server_id;
        Conv.C_dc = SD_card.settings.C_dc;
        Conv.L_conv = SD_card.settings.L_conv;
        Conv.C_conv = SD_card.settings.C_conv;
        Conv.I_lim_nominal = SD_card.settings.I_lim;
        Conv.no_neutral = SD_card.settings.no_neutral;
        status_ACDC.expected_number_of_slaves = SD_card.settings.number_of_slaves;

        memcpy(on_off_odd_a, SD_card.harmonics.on_off_odd_a, sizeof(on_off_odd_a));
        memcpy(on_off_odd_b, SD_card.harmonics.on_off_odd_b, sizeof(on_off_odd_b));
        memcpy(on_off_odd_c, SD_card.harmonics.on_off_odd_c, sizeof(on_off_odd_c));
        memcpy(on_off_even_a, SD_card.harmonics.on_off_even_a, sizeof(on_off_even_a));
        memcpy(on_off_even_b, SD_card.harmonics.on_off_even_b, sizeof(on_off_even_b));
        memcpy(on_off_even_c, SD_card.harmonics.on_off_even_c, sizeof(on_off_even_c));
        memcpy(&CT_char_vars.CT_char, &SD_card.CT_char, sizeof(CT_char_vars.CT_char));
        memcpy(&CT_char_vars.calibration, &SD_card.calibration, sizeof(CT_char_vars.calibration));

        Meas_ACDC_gain = SD_card.calibration.Meas_ACDC_gain;
        Meas_ACDC_offset = SD_card.calibration.Meas_ACDC_offset;
        register float ratio_SD = (SD_card.calibration.Meas_ACDC_gain.def_osr * SD_card.calibration.Meas_ACDC_gain.def_osr) / ((float)EMIF_mem.read.def_osr * (float)EMIF_mem.read.def_osr);
        ratio_SD *= (float)EMIF_mem.read.sd_shift / SD_card.calibration.Meas_ACDC_gain.sd_shift;
        Meas_ACDC_gain.U_grid.a *= ratio_SD;
        Meas_ACDC_gain.U_grid.b *= ratio_SD;
        Meas_ACDC_gain.U_grid.c *= ratio_SD;
        Meas_ACDC_gain.U_dc     *= ratio_SD;
        Meas_ACDC_gain.U_dc_n   *= ratio_SD;
        Meas_ACDC_gain.I_conv.a *= ratio_SD;
        Meas_ACDC_gain.I_conv.b *= ratio_SD;
        Meas_ACDC_gain.I_conv.c *= ratio_SD;
        Meas_ACDC_gain.I_conv.n *= ratio_SD;
        Meas_ACDC_gain.I_grid.a *= ratio_SD;
        Meas_ACDC_gain.I_grid.b *= ratio_SD;
        Meas_ACDC_gain.I_grid.c *= ratio_SD;

        Meas_ACDC_offset.U_grid.a *= ratio_SD;
        Meas_ACDC_offset.U_grid.b *= ratio_SD;
        Meas_ACDC_offset.U_grid.c *= ratio_SD;
        Meas_ACDC_offset.U_dc     *= ratio_SD;
        Meas_ACDC_offset.U_dc_n   *= ratio_SD;
        Meas_ACDC_offset.I_conv.a *= ratio_SD;
        Meas_ACDC_offset.I_conv.b *= ratio_SD;
        Meas_ACDC_offset.I_conv.c *= ratio_SD;
        Meas_ACDC_offset.I_conv.n *= ratio_SD;
        Meas_ACDC_offset.I_grid.a *= ratio_SD;
        Meas_ACDC_offset.I_grid.b *= ratio_SD;
        Meas_ACDC_offset.I_grid.c *= ratio_SD;
    }

    CPU1toCPU2.Meas_ACDC_gain = Meas_ACDC_gain;
    CPU1toCPU2.Meas_ACDC_offset = Meas_ACDC_offset;
    IpcRegs.IPCSET.bit.IPC4 = 1;

    Background.harmonics_odd_last =
    Background.harmonics_even_last =
    Background.harmonics_odd =
    Background.harmonics_even = 0;

    update_harmonics();
    convert_harmonics_to_bits();

    if(!switch_FLASH.retrieve())
    {
        ONOFF.ONOFF = ONOFF.ONOFF_FLASH;
        ONOFF.ONOFF_last = !ONOFF.ONOFF_FLASH;
    }
    else
    {
        ONOFF.ONOFF =
        ONOFF.ONOFF_last = 0;
    }

    ONOFF.ONOFF_switch = !GPIO_READ(ON_OFF_CM);
    ONOFF.ONOFF_switch_last = ONOFF.ONOFF_switch;
    ONOFF.switch_timer = 1e6;
    ONOFF.ONOFF_temp = ONOFF.ONOFF;

    Init.Variables();

    union
    {
        Uint32 u32;
        int16 i16[2];
    }Meas_alarm_int;

    Meas_alarm_int.i16[0] = Meas_ACDC_alarm_H.I_conv / Meas_ACDC_gain.I_conv.a + Meas_ACDC_offset.I_conv.a;
    Meas_alarm_int.i16[1] = Meas_ACDC_alarm_L.I_conv / Meas_ACDC_gain.I_conv.a + Meas_ACDC_offset.I_conv.a;
    EMIF_mem.write.I_conv_a_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = Meas_ACDC_alarm_H.I_conv / Meas_ACDC_gain.I_conv.b + Meas_ACDC_offset.I_conv.b;
    Meas_alarm_int.i16[1] = Meas_ACDC_alarm_L.I_conv / Meas_ACDC_gain.I_conv.b + Meas_ACDC_offset.I_conv.b;
    EMIF_mem.write.I_conv_b_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = Meas_ACDC_alarm_H.I_conv / Meas_ACDC_gain.I_conv.c + Meas_ACDC_offset.I_conv.c;
    Meas_alarm_int.i16[1] = Meas_ACDC_alarm_L.I_conv / Meas_ACDC_gain.I_conv.c + Meas_ACDC_offset.I_conv.c;
    EMIF_mem.write.I_conv_c_lim = Meas_alarm_int.u32;
    Meas_ACDC_gain.I_conv.n = (Meas_ACDC_gain.I_conv.a + Meas_ACDC_gain.I_conv.b + Meas_ACDC_gain.I_conv.c) * MATH_1_3;
    Meas_ACDC_offset.I_conv.n = Meas_ACDC_offset.I_conv.a + Meas_ACDC_offset.I_conv.b + Meas_ACDC_offset.I_conv.c;
    Meas_alarm_int.i16[0] = (Meas_ACDC_alarm_H.I_conv / Meas_ACDC_gain.I_conv.n + Meas_ACDC_offset.I_conv.n) * 0.25f;
    Meas_alarm_int.i16[1] = (Meas_ACDC_alarm_L.I_conv / Meas_ACDC_gain.I_conv.n + Meas_ACDC_offset.I_conv.n) * 0.25f;
    EMIF_mem.write.I_conv_n_lim = Meas_alarm_int.u32;

    Meas_alarm_int.i16[0] = +Meas_ACDC_alarm_H.U_grid_abs / Meas_ACDC_gain.U_grid.a + Meas_ACDC_offset.U_grid.a;
    Meas_alarm_int.i16[1] = -Meas_ACDC_alarm_H.U_grid_abs / Meas_ACDC_gain.U_grid.a + Meas_ACDC_offset.U_grid.a;
    EMIF_mem.write.U_grid_a_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = +Meas_ACDC_alarm_H.U_grid_abs / Meas_ACDC_gain.U_grid.b + Meas_ACDC_offset.U_grid.b;
    Meas_alarm_int.i16[1] = -Meas_ACDC_alarm_H.U_grid_abs / Meas_ACDC_gain.U_grid.b + Meas_ACDC_offset.U_grid.b;
    EMIF_mem.write.U_grid_b_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = +Meas_ACDC_alarm_H.U_grid_abs / Meas_ACDC_gain.U_grid.c + Meas_ACDC_offset.U_grid.c;
    Meas_alarm_int.i16[1] = -Meas_ACDC_alarm_H.U_grid_abs / Meas_ACDC_gain.U_grid.c + Meas_ACDC_offset.U_grid.c;
    EMIF_mem.write.U_grid_c_lim = Meas_alarm_int.u32;

    Modbus_RTU_class::Modbus_RTU_parameters_struct RTU_LCD_parameters;
    RTU_LCD_parameters.is_fiber = 0;
    RTU_LCD_parameters.use_DERE = 1;
    RTU_LCD_parameters.DERE_pin = EN_Mod_1_CM;
    RTU_LCD_parameters.RX_pin = RX_Mod_1_CM;
    RTU_LCD_parameters.TX_pin = TX_Mod_1_CM;
    RTU_LCD_parameters.SciRegs = &SciaRegs;
    RTU_LCD_parameters.ECapRegs = &ECap1Regs;
    RTU_LCD_parameters.baudrate = 115200;
    Modbus_slave_LCD.RTU->init(&RTU_LCD_parameters);

    Modbus_RTU_class::Modbus_RTU_parameters_struct RTU_EXT_parameters;
    RTU_EXT_parameters.is_fiber = 0;
    RTU_EXT_parameters.use_DERE = 1;
    RTU_EXT_parameters.DERE_pin = EN_Mod_2_CM;
    RTU_EXT_parameters.RX_pin = RX_Mod_2_CM;
    RTU_EXT_parameters.TX_pin = TX_Mod_2_CM;
    RTU_EXT_parameters.SciRegs = &ScidRegs;
    RTU_EXT_parameters.ECapRegs = &ECap2Regs;
    if(SD_card.settings.Baudrate >= 9600) RTU_EXT_parameters.baudrate = SD_card.settings.Baudrate;
    else RTU_EXT_parameters.baudrate = 9600;
    Modbus_slave_EXT.RTU->init(&RTU_EXT_parameters);

    Modbus_RTU_class::Modbus_RTU_parameters_struct RTU_FIBER_parameters;
    RTU_FIBER_parameters.is_fiber = 1;
    Modbus_slave_FIBER.RTU->init(&RTU_FIBER_parameters);

    Uint32 delay_timer = IpcRegs.IPCCOUNTERL;
    while(!RTC_current_time.year10 && !RTC_current_time.year)
    {
        rtc.process();
        rtc.process_event(Rtc::event_read_time);

        Rtc::datetime_s now = rtc.get_last_time();
        FatFS_time.second_2 = now.sec>>1;
        FatFS_time.minute = now.min;
        FatFS_time.hour = now.hour;
        FatFS_time.day = now.day;
        FatFS_time.month = now.month;
        FatFS_time.year = now.year + 20;
        Rtc::datetime_bcd_s now_bcd = rtc.get_last_time_bcd();
        RTC_current_time.second = (now_bcd.sec >> 0 ) & 0xF;
        RTC_current_time.second10 = (now_bcd.sec >> 4 ) & 0xF;
        RTC_current_time.minute = (now_bcd.min >> 0 ) & 0xF;
        RTC_current_time.minute10 = (now_bcd.min >> 4 ) & 0xF;
        RTC_current_time.hour = (now_bcd.hour >> 0 ) & 0xF;
        RTC_current_time.hour10 = (now_bcd.hour >> 4 ) & 0xF;
        RTC_current_time.day = (now_bcd.day >> 0 ) & 0xF;
        RTC_current_time.day10 = (now_bcd.day >> 4 ) & 0xF;
        RTC_current_time.month = (now_bcd.month >> 0 ) & 0xF;
        RTC_current_time.month10 = (now_bcd.month >> 4 ) & 0xF;
        RTC_current_time.year = (now_bcd.year >> 0 ) & 0xF;
        RTC_current_time.year10 = (now_bcd.year >> 4 ) & 0xF;

        if(IpcRegs.IPCCOUNTERL - delay_timer > 200000000) break;
    }


    GPIO_SET(RST_CM);

    EMIF_mem.write.PWM_control = 0xFF00;

    mosfet_ctrl_app.process_event( MosfetCtrlApp::event_configure );
    MosfetCtrlApp::state_t mosfet_state;

    do{
        mosfet_ctrl_app.process();
        mosfet_state = mosfet_ctrl_app.getState();

    }while( mosfet_state != MosfetCtrlApp::state_idle
            && mosfet_state != MosfetCtrlApp::state_error );

    EMIF_mem.write.PWM_control = 0x0000;

    DELAY_US(1000);

    GPIO_CLEAR(RST_CM);

    EALLOW;
    Cla1Regs.MIER.bit.INT2 =
    Cla1Regs.MIER.bit.INT1 =
    Cla1Regs.MICLR.bit.INT2 =
    Cla1Regs.MICLR.bit.INT1 = 1;
    EDIS;

    ECap1Regs.ECCLR.all = 0xFFFF;
    EMIF_mem.write.rx_ack.all = 0xFFFFFFFF;
    PieCtrlRegs.PIEIFR1.all =
    PieCtrlRegs.PIEIFR2.all =
    PieCtrlRegs.PIEIFR3.all =
    PieCtrlRegs.PIEIFR4.all =
    PieCtrlRegs.PIEIFR5.all =
    PieCtrlRegs.PIEIFR6.all =
    PieCtrlRegs.PIEIFR7.all =
    PieCtrlRegs.PIEIFR8.all =
    PieCtrlRegs.PIEIFR9.all =
    PieCtrlRegs.PIEIFR10.all =
    PieCtrlRegs.PIEIFR11.all =
    PieCtrlRegs.PIEIFR12.all = 0;
    PieCtrlRegs.PIEACK.all = 0xFFFF;
    IFR &= 0;
    __asm(" NOP");
    __asm(" NOP");
    __asm(" NOP");
    __asm(" NOP");
    __asm(" NOP");
    __asm(" NOP");
    __asm(" NOP");
    __asm(" NOP");

    EINT;

    Init.clear_alarms();

    status_ACDC.Init_done = 1;
    Machine_slave.recent_error = 1;
    Machine_slave.state = Machine_slave_class::state_idle;
}

void Background_class::Main()
{
        Uint16 process_next_ADU = 0;

    //przetwarzanie urzadzenia ADU na 1. RTU
    switch( Modbus_slave_LCD.task() ){
    case mdb_no_request:
        process_next_ADU = 1;
        break;

    case mdb_request_error:
    case mdb_request_valid:
        break;

    case mdb_request_wrong_address:
        Modbus_slave_LCD.RTU->signal_data_processed();//skoro wykrylo zly adres to musialy byc dane
        Fiber_comm_master[0].input_flags.send_modbus = 1;
        Fiber_comm_master[1].input_flags.send_modbus = 1;
        Fiber_comm_master[2].input_flags.send_modbus = 1;
        Fiber_comm_master[3].input_flags.send_modbus = 1;
        break;
    }

    //przetwarzanie urzadzen na 2. RTU
    //optym. obciazenia petli glownej i wysylania znakow, gdy obslugiwano urzadzenie z 1. RTU,
    //to z drugiego RTU zrobic w kolejnym cyklu petli glownej
    if( process_next_ADU ){

        switch( Modbus_slave_EXT.task() ){

        case mdb_request_wrong_address: //wykryty zly adres, sprawdzic dla drugiego urzadzenia na tym RTU
            if( Modbus_slave_EXT.task() == mdb_request_wrong_address)
               Modbus_slave_EXT.RTU->signal_data_processed();//skoro wykrylo zly adres to musialy byc dane
            //inne kody nie wymagaja sygnalizacji, bo albo juz zostaly zasygnalizowane albo nie bylo danych
            break;

        //nie ma sensu sprawdzac drugiego urzadzenia ADU na tym samym RTU, dla ponizszego:
        case mdb_no_request:      // brak danych na tym RTU
        case mdb_request_error:   // wykryty blad (juz zasygnalizowane)
        case mdb_request_valid:   // obsluzono (juz zasygnalizowano)
            break;
        }
    }


    static Uint32 master_slave_selector = 0;
    if(status_ACDC.master_slave_selector || master_slave_selector)
    {
//        master_slave_selector = 1;
        Fiber_comm_master[0].Main();//musza byc wszystkie
        Fiber_comm_master[1].Main();
        Fiber_comm_master[2].Main();
        Fiber_comm_master[3].Main();
    }
    Fiber_comm_slave.Main();


    SD_card.Scope_snapshot_task();

    static class Blink_class Blink_logs(10.0f);
    static Uint16 blink_multiple = 0;
    if(Blink_logs.task_simple())
    {
        SD_card.log_data();
        if(++blink_multiple > 60)
        {
            SD_card.save_meter_data();
            blink_multiple = 0;
        }
    }

    static class Blink_class Blink_CT_char(0.1f);
    if(Blink_CT_char.task_simple())
        CT_char_calc();

    Blink();

    Init.IPCBootCPU2_flash();

    ONOFF_switch_func();

    rtc.process(); // wywolywanie cykliczne konieczne do dzialania Rtc

    mosfet_ctrl_app.process(); //jw

    ////////////////////////////////////////////////////////////////////////////////////////

    static class Blink_class Blink_idle(1.0f);
    if(Blink_idle.task_simple())
    {
        if(control_ACDC.triggers.bit.save_to_RTC && !control_ACDC.triggers.bit.CPU_reset)
        {
            control_ACDC.triggers.bit.save_to_RTC = 0;

            Rtc::datetime_bcd_s new_time;
            new_time.sec = RTC_new_time.second10<<4 | RTC_new_time.second;
            new_time.min = RTC_new_time.minute10<<4 | RTC_new_time.minute;
            new_time.hour = RTC_new_time.hour10<<4 | RTC_new_time.hour;
            new_time.day = RTC_new_time.day10<<4 | RTC_new_time.day;
            new_time.dayofweek = 1;
            new_time.month = RTC_new_time.month10<<4 | RTC_new_time.month;
            new_time.year = RTC_new_time.year10<<4 | RTC_new_time.year;
            rtc.process_event(Rtc::event_setup_time_bcd, &new_time);
            RTC_current_time = RTC_new_time;
        }
        else
        {
            Rtc::datetime_s now = rtc.get_last_time();
            FatFS_time.second_2 = now.sec>>1;
            FatFS_time.minute = now.min;
            FatFS_time.hour = now.hour;
            FatFS_time.day = now.day;
            FatFS_time.month = now.month;
            FatFS_time.year = now.year + 20;
            Rtc::datetime_bcd_s now_bcd = rtc.get_last_time_bcd();
            RTC_current_time.second = (now_bcd.sec >> 0 ) & 0xF;
            RTC_current_time.second10 = (now_bcd.sec >> 4 ) & 0xF;
            RTC_current_time.minute = (now_bcd.min >> 0 ) & 0xF;
            RTC_current_time.minute10 = (now_bcd.min >> 4 ) & 0xF;
            RTC_current_time.hour = (now_bcd.hour >> 0 ) & 0xF;
            RTC_current_time.hour10 = (now_bcd.hour >> 4 ) & 0xF;
            RTC_current_time.day = (now_bcd.day >> 0 ) & 0xF;
            RTC_current_time.day10 = (now_bcd.day >> 4 ) & 0xF;
            RTC_current_time.month = (now_bcd.month >> 0 ) & 0xF;
            RTC_current_time.month10 = (now_bcd.month >> 4 ) & 0xF;
            RTC_current_time.year = (now_bcd.year >> 0 ) & 0xF;
            RTC_current_time.year10 = (now_bcd.year >> 4 ) & 0xF;
            rtc.process_event(Rtc::event_read_time);
        }

        ////////////////////////////////////////////////////////////////////////////////////////

        Init.Fan_speed();

        ////////////////////////////////////////////////////////////////////////////////////////

        update_harmonics();

        ////////////////////////////////////////////////////////////////////////////////////////

        convert_harmonics_to_floats();

        ////////////////////////////////////////////////////////////////////////////////////////

        SINCOS_calc_CPUasm(sincos_table, Conv.w_filter * Conv.Ts / Conv.Ts_rate);
        SINCOS_calc_CPUasm(sincos_table_comp, Conv.w_filter * Conv.Ts / Conv.Ts_rate * Conv.compensation);
        SINCOS_calc_CPUasm(sincos_table_comp2, Conv.w_filter * Conv.Ts / Conv.Ts_rate * Conv.compensation2);
        SINCOS_calc_CPUasm(sincos_table_Kalman, Conv.w_filter * Conv.Ts);

        for(Uint16 i = 0; i < FPGA_RESONANT_STATES; i++)
        {
            register float modifier = Conv.range_modifier_Resonant_coefficients;
            EMIF_mem.write.Resonant[0].states[i].CA =
            EMIF_mem.write.Resonant[1].states[i].CA =
            EMIF_mem.write.Resonant[2].states[i].CA = modifier * sincos_table[2 * i].cosine;
            EMIF_mem.write.Resonant[0].states[i].SA =
            EMIF_mem.write.Resonant[1].states[i].SA =
            EMIF_mem.write.Resonant[2].states[i].SA = modifier * sincos_table[2 * i].sine;

            EMIF_mem.write.Resonant[0].states[i].GCB =
            EMIF_mem.write.Resonant[1].states[i].GCB =
            EMIF_mem.write.Resonant[2].states[i].GCB = modifier * (sincos_table[2 * i].cosine - 1.0f);
            EMIF_mem.write.Resonant[0].states[i].GSB =
            EMIF_mem.write.Resonant[1].states[i].GSB =
            EMIF_mem.write.Resonant[2].states[i].GSB = modifier * sincos_table[2 * i].sine;
            EMIF_mem.write.Resonant[0].states[i].GCC =
            EMIF_mem.write.Resonant[1].states[i].GCC =
            EMIF_mem.write.Resonant[2].states[i].GCC = modifier * sincos_table_comp2[2 * i].cosine;
            EMIF_mem.write.Resonant[0].states[i].GSC =
            EMIF_mem.write.Resonant[1].states[i].GSC =
            EMIF_mem.write.Resonant[2].states[i].GSC = modifier * sincos_table_comp2[2 * i].sine;

            EMIF_mem.write.Resonant[0].states[i].CCB =
            EMIF_mem.write.Resonant[1].states[i].CCB =
            EMIF_mem.write.Resonant[2].states[i].CCB = modifier * (sincos_table[2 * i].cosine - 1.0f) / (float)(2 * i + 1) * Conv.Kr_I;
            EMIF_mem.write.Resonant[0].states[i].CSB =
            EMIF_mem.write.Resonant[1].states[i].CSB =
            EMIF_mem.write.Resonant[2].states[i].CSB = modifier * sincos_table[2 * i].sine / (float)(2 * i + 1) * Conv.Kr_I;
            EMIF_mem.write.Resonant[0].states[i].CCC =
            EMIF_mem.write.Resonant[1].states[i].CCC =
            EMIF_mem.write.Resonant[2].states[i].CCC = modifier * sincos_table_comp[2 * i].cosine;
            EMIF_mem.write.Resonant[0].states[i].CSC =
            EMIF_mem.write.Resonant[1].states[i].CSC =
            EMIF_mem.write.Resonant[2].states[i].CSC = modifier * sincos_table_comp[2 * i].sine;
        }
        for(Uint16 i = 0; i < FPGA_RESONANT_GRID_STATES; i++)
        {
            register float modifier = Conv.range_modifier_Resonant_coefficients;
            if(Conv.resonant_even_number == -1.0f) modifier = 0.0f;
            EMIF_mem.write.Resonant[3].states[i].CA =
            EMIF_mem.write.Resonant[4].states[i].CA =
            EMIF_mem.write.Resonant[5].states[i].CA = modifier * sincos_table[2 * i + 1].cosine;
            EMIF_mem.write.Resonant[3].states[i].SA =
            EMIF_mem.write.Resonant[4].states[i].SA =
            EMIF_mem.write.Resonant[5].states[i].SA = modifier * sincos_table[2 * i + 1].sine;

            EMIF_mem.write.Resonant[3].states[i].GCB =
            EMIF_mem.write.Resonant[4].states[i].GCB =
            EMIF_mem.write.Resonant[5].states[i].GCB = modifier * (sincos_table[2 * i + 1].cosine - 1.0f);
            EMIF_mem.write.Resonant[3].states[i].GSB =
            EMIF_mem.write.Resonant[4].states[i].GSB =
            EMIF_mem.write.Resonant[5].states[i].GSB = modifier * sincos_table[2 * i + 1].sine;
            EMIF_mem.write.Resonant[3].states[i].GCC =
            EMIF_mem.write.Resonant[4].states[i].GCC =
            EMIF_mem.write.Resonant[5].states[i].GCC = modifier * sincos_table_comp2[2 * i + 1].cosine;
            EMIF_mem.write.Resonant[3].states[i].GSC =
            EMIF_mem.write.Resonant[4].states[i].GSC =
            EMIF_mem.write.Resonant[5].states[i].GSC = modifier * sincos_table_comp2[2 * i + 1].sine;

            EMIF_mem.write.Resonant[3].states[i].CCB =
            EMIF_mem.write.Resonant[4].states[i].CCB =
            EMIF_mem.write.Resonant[5].states[i].CCB = modifier * (sincos_table[2 * i + 1].cosine - 1.0f) / (float)(2 * i + 2) * Conv.Kr_I;
            EMIF_mem.write.Resonant[3].states[i].CSB =
            EMIF_mem.write.Resonant[4].states[i].CSB =
            EMIF_mem.write.Resonant[5].states[i].CSB = modifier * sincos_table[2 * i + 1].sine / (float)(2 * i + 2) * Conv.Kr_I;
            EMIF_mem.write.Resonant[3].states[i].CCC =
            EMIF_mem.write.Resonant[4].states[i].CCC =
            EMIF_mem.write.Resonant[5].states[i].CCC = modifier * sincos_table_comp[2 * i + 1].cosine;
            EMIF_mem.write.Resonant[3].states[i].CSC =
            EMIF_mem.write.Resonant[4].states[i].CSC =
            EMIF_mem.write.Resonant[5].states[i].CSC = modifier * sincos_table_comp[2 * i + 1].sine;
        }

        for(Uint16 i = 1; i < FPGA_KALMAN_STATES; i++)
        {
            register float modifier = Conv.range_modifier_Kalman_coefficients;
            EMIF_mem.write.Kalman.states[i].cos_K = sincos_table_Kalman[2 * (i - 1)].cosine * modifier;
            EMIF_mem.write.Kalman.states[i].sin_K = sincos_table_Kalman[2 * (i - 1)].sine * modifier;
        }

        for(Uint16 i = 1; i < FPGA_KALMAN_DC_STATES; i++)
        {
            register float modifier = Conv.range_modifier_Kalman_coefficients;
            EMIF_mem.write.Kalman_DC.states[i].cos_K = sincos_table_Kalman[i].cosine * modifier;
            EMIF_mem.write.Kalman_DC.states[i].sin_K = sincos_table_Kalman[i].sine * modifier;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    if(control_ACDC.triggers.bit.ONOFF_set)
    {
        control_ACDC.triggers.bit.ONOFF_set = 0;
        ONOFF.ONOFF_temp = 1;
    }

    if(control_ACDC.triggers.bit.ONOFF_reset)
    {
        control_ACDC.triggers.bit.ONOFF_reset = 0;
        ONOFF.ONOFF_temp = 0;
    }

    if(control_ACDC.triggers.bit.SD_save_H_settings)
    {
        control_ACDC.triggers.bit.SD_save_H_settings = 0;

        memcpy(SD_card.harmonics.on_off_odd_a, on_off_odd_a, sizeof(SD_card.harmonics.on_off_odd_a));
        memcpy(SD_card.harmonics.on_off_odd_b, on_off_odd_b, sizeof(SD_card.harmonics.on_off_odd_b));
        memcpy(SD_card.harmonics.on_off_odd_c, on_off_odd_c, sizeof(SD_card.harmonics.on_off_odd_c));
        memcpy(SD_card.harmonics.on_off_even_a, on_off_even_a, sizeof(SD_card.harmonics.on_off_even_a));
        memcpy(SD_card.harmonics.on_off_even_b, on_off_even_b, sizeof(SD_card.harmonics.on_off_even_b));
        memcpy(SD_card.harmonics.on_off_even_c, on_off_even_c, sizeof(SD_card.harmonics.on_off_even_c));
        SD_card.harmonics.available = 1;

        if(!SD_card.save_H_settings()) status_ACDC.SD_no_harmonic_settings = 0;
        else status_ACDC.SD_no_harmonic_settings = 1;
    }

    if(control_ACDC.triggers.bit.SD_save_settings)
    {
        control_ACDC.triggers.bit.SD_save_settings = 0;

        SD_card.settings.control = control_ACDC;
        SD_card.settings.available = 1;
        SD_card.settings.Baudrate = 100.0*control_ext_modbus.fields.baudrate;
        SD_card.settings.modbus_ext_server_id = control_ext_modbus.fields.ext_server_id;
        SD_card.settings.wifi_on = status_ACDC.wifi_on;

        if(!SD_card.save_settings()) status_ACDC.SD_no_settings = 0;
        else status_ACDC.SD_no_settings = 1;
    }

    if(control_ACDC.triggers.bit.SD_reset_energy_meter)
    {
        control_ACDC.triggers.bit.SD_reset_energy_meter = 0;

        memset(&SD_card.meter.Energy_meter, 0, sizeof(SD_card.meter.Energy_meter));

        DINT_copy_CPUasm((Uint16 *)&Energy_meter.lower,  (Uint16 *)&SD_card.meter.Energy_meter,  sizeof(Energy_meter.lower));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.P_p,  (Uint16 *)&SD_card.meter.Energy_meter.P_p,  sizeof(Energy_meter.upper.P_p));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.P_n,  (Uint16 *)&SD_card.meter.Energy_meter.P_n,  sizeof(Energy_meter.upper.P_n));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.QI,   (Uint16 *)&SD_card.meter.Energy_meter.QI,   sizeof(Energy_meter.upper.QI));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.QII,  (Uint16 *)&SD_card.meter.Energy_meter.QII,  sizeof(Energy_meter.upper.QII));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.QIII, (Uint16 *)&SD_card.meter.Energy_meter.QIII, sizeof(Energy_meter.upper.QIII));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.QIV,  (Uint16 *)&SD_card.meter.Energy_meter.QIV,  sizeof(Energy_meter.upper.QIV));
        DINT_copy_CPUasm((Uint16 *)&Energy_meter.upper.sum,  (Uint16 *)&SD_card.meter.Energy_meter.sum,  sizeof(Energy_meter.upper.sum));

        SD_card.save_meter_data();

        SD_card.meter.available = 1;
    }

    if(control_ACDC.triggers.bit.CPU_reset
            && Modbus_slave_LCD.RTU->state == Modbus_RTU_class::Modbus_RTU_idle
            && Modbus_slave_EXT.RTU->state == Modbus_RTU_class::Modbus_RTU_idle
            && Modbus_slave_FIBER.RTU->state == Modbus_RTU_class::Modbus_RTU_idle)
    {
        control_ACDC.triggers.bit.CPU_reset = 0;

        EALLOW;
        NmiIntruptRegs.NMICFG.bit.NMIE = 1;
        NmiIntruptRegs.NMIWDPRD = 0;
        NmiIntruptRegs.NMIFLGFRC.bit.OVF = 1;
        EDIS;
    }

}
