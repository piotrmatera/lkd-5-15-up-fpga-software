/*
 * State.cpp
 *
 *  Created on: 2 maj 2020
 *      Author: MrTea
 */
#include "stdafx.h"

#include <math.h>
#include <stdlib.h>     /* qsort */
#include <string.h>

#include "State.h"
#include "Scope.h"
#include "SD_card.h"
#include "Init.h"
#include "FLASH.h"
#include "Blink.h"
#include "Modbus_RTU.h"
#include "Rtc.h"
#include "diskio.h"
#include "Modbus_Converter_memory.h"
#include "Modbus_devices.h"

#include "Fiber_comm_master.h"

#include "Software/driver_mosfet/MosfetDriver.h"
#include "MosfetCtrlApp.h"

#include "device_check.h"

extern Rtc rtc;

struct time_BCD_struct RTC_current_time;
struct time_BCD_struct RTC_new_time;

class Machine_class Machine;
void (*Machine_class::state_pointers[Machine_class::state_max])();
struct CT_calc_struct CT_char_vars;
struct L_grid_meas_struct L_grid_meas;
struct timer_struct Timer_total;

const class FLASH_class switch_FLASH =
{
 .address = {(Uint16 *)&Machine.ONOFF_FLASH, 0},
 .sector = SectorM,
 .size16_each = {sizeof(Machine.ONOFF_FLASH), 0},
};

const class FLASH_class L_grid_FLASH =
{
 .address = {(Uint16 *)&L_grid_meas.L_grid_previous, 0},
 .sector = SectorL,
 .size16_each = {sizeof(L_grid_meas.L_grid_previous), 0},
};

const class FLASH_class error_retry_FLASH =
{
 .address = {(Uint16 *)&Machine.error_retry, 0},
 .sector = SectorK,
 .size16_each = {sizeof(Machine.error_retry), 0},
};

static int compare_float (const void * a, const void * b)
{
    if ( *(float*)a <  *(float*)b ) return -1;
    else if ( *(float*)a >  *(float*)b ) return 1;
    else return 0;
}

void CT_char_calc()
{
    if(CT_char_vars.CT_char.available && CT_char_vars.calibration.available)
    {
        if(CT_char_vars.CT_char.number_of_elements == 1)
        {
            Meas_master_gain.I_grid.a = CT_char_vars.calibration.Meas_master_gain.I_grid.a * fabsf(CT_char_vars.CT_char.CT_ratio_a[0]);
            Meas_master_gain.I_grid.b = CT_char_vars.calibration.Meas_master_gain.I_grid.b * fabsf(CT_char_vars.CT_char.CT_ratio_b[0]);
            Meas_master_gain.I_grid.c = CT_char_vars.calibration.Meas_master_gain.I_grid.c * fabsf(CT_char_vars.CT_char.CT_ratio_c[0]);
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
                alarm_master.bit.CT_char_error = 1;

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
            Meas_master_gain.I_grid.a = CT_char_vars.calibration.Meas_master_gain.I_grid.a * fabsf(Y_interp.a);
            Meas_master_gain.I_grid.b = CT_char_vars.calibration.Meas_master_gain.I_grid.b * fabsf(Y_interp.b);
            Meas_master_gain.I_grid.c = CT_char_vars.calibration.Meas_master_gain.I_grid.c * fabsf(Y_interp.c);

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

    CPU1toCPU2.Meas_master_gain = Meas_master_gain;
    CPU1toCPU2.Meas_master_offset = Meas_master_offset;
    IpcRegs.IPCSET.bit.IPC4 = 1;
}

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

void Blink()
{
    static class Blink_class Blink_LED1(false);
    static class Blink_class Blink_LED2(false);
    static class Blink_class Blink_LED3(false);
    static class Blink_class Blink_LED4(false);
    static class Blink_class Blink_LED5(false);

    switch(Machine.state)
    {
        case Machine_class::state_calibrate_offsets:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -1.1f, -1.1f};
            static const float period = 1.1f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_master.calibration_procedure_error);
            break;
        }

        case Machine_class::state_calibrate_curent_gain:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -0.2f, 0.3f, -1.3f, -1.3f};
            static const float period = 1.3f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_master.calibration_procedure_error);
            break;
        }

        case Machine_class::state_calibrate_AC_voltage_gain:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -0.2f, 0.3f, -0.4f, 0.5f, -1.5f, -1.5f};
            static const float period = 1.5f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_master.calibration_procedure_error);
            break;
        }

        case Machine_class::state_calibrate_DC_voltage_gain:
        {
            Blink_LED1.update_pattern(true);

            static const float pattern[] = {0.1f, -0.2f, 0.3f, -0.4f, 0.5f, -0.6f, 0.7f, -1.7f, -1.7f};
            static const float period = 1.5f;
            Blink_LED2.update_pattern(period, (float *)pattern);

            Blink_LED3.update_pattern((bool)status_master.calibration_procedure_error);
            break;
        }
        default:
        {
            if(Machine.ONOFF)
            {
                //zielony nie miga gdy alarm
                if(Machine.state == Machine_class::state_operational)
                {
                    if(status_master.no_CT_connected_a || status_master.no_CT_connected_b || status_master.no_CT_connected_c) Blink_LED1.update_pattern(0.2f, 0.5f);
                    else Blink_LED1.update_pattern(true);
                }
                else Blink_LED1.update_pattern(2.0f, 0.5f);
            }
            else Blink_LED1.update_pattern(false);

            if(alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2])
            {
                Blink_LED2.update_pattern(false);
                Blink_LED3.update_pattern(0.67f, 0.5f);
            }
            else
            {
                if(Conv.enable)
                {
                    if(Blink_LED2.zero_crossing)
                    {
                        if(status_master.in_limit_Q)
                            Blink_LED2.update_pattern(true);
                        else if(status_master.in_limit_P)
                            Blink_LED2.update_pattern(2.0f, 0.67f);
                        else if(status_master.in_limit_H)
                            Blink_LED2.update_pattern(2.0f, 0.33f);
                        else
                            Blink_LED2.update_pattern(false);
                    }
                }
                else
                {
                    Blink_LED2.update_pattern(false);
                }

                Blink_LED3.update_pattern(false);
            }
            break;
        }
    }

    if(Machine.recent_error && Conv.enable) Blink_LED4.update_pattern(true);
    else Blink_LED4.update_pattern(false);

#if !DBG_AT_GPIO
    GPIO_WRITE(LED1_CM, Blink_LED1.task());
    GPIO_WRITE(LED2_CM, Blink_LED2.task());
    GPIO_WRITE(LED3_CM, Blink_LED3.task());
    GPIO_WRITE(LED4_CM, Blink_LED4.task());
#endif
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
        *(Uint32 *)&control_master.H_odd_a = temp_harmonic_a;
        *(Uint32 *)&control_master.H_odd_b = temp_harmonic_b;
        *(Uint32 *)&control_master.H_odd_c = temp_harmonic_c;
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
        *(Uint32 *)&control_master.H_even_a = temp_harmonic_a;
        *(Uint32 *)&control_master.H_even_b = temp_harmonic_b;
        *(Uint32 *)&control_master.H_even_c = temp_harmonic_c;
    }
}

void convert_harmonics_to_floats()
{
    {
        Uint16 i;
        register Uint32 temp_harmonic_a = *(Uint32 *)&control_master.H_odd_a;
        register Uint32 temp_harmonic_b = *(Uint32 *)&control_master.H_odd_b;
        register Uint32 temp_harmonic_c = *(Uint32 *)&control_master.H_odd_c;
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
        register Uint32 temp_harmonic_a = *(Uint32 *)&control_master.H_even_a;
        register Uint32 temp_harmonic_b = *(Uint32 *)&control_master.H_even_b;
        register Uint32 temp_harmonic_c = *(Uint32 *)&control_master.H_even_c;
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
    *(Uint32 *)&control_master.H_odd_a &= 0xFFFFFFFE;
    *(Uint32 *)&control_master.H_odd_b &= 0xFFFFFFFE;
    *(Uint32 *)&control_master.H_odd_c &= 0xFFFFFFFE;

    if(Machine.harmonics_odd_last != Machine.harmonics_odd)
    {
        Machine.harmonics_odd_last = Machine.harmonics_odd;
        if(Machine.harmonics_odd > 24) Machine.harmonics_odd = 24;
        Uint16 count = 1;
        while(count < Machine.harmonics_odd + 1)
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

    if(Machine.harmonics_even_last != Machine.harmonics_even)
    {
        Machine.harmonics_even_last = Machine.harmonics_even;
        if(Machine.harmonics_even > 25) Machine.harmonics_even = 25;
        Uint16 count = 0;
        while(count < Machine.harmonics_even)
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



void ONOFF_switch_func()
{
    Machine.ONOFF_last = Machine.ONOFF;
    Machine.ONOFF = Machine.ONOFF_temp;
    status_master.ONOFF = Machine.ONOFF;

    if (Machine.ONOFF_FLASH != Machine.ONOFF)
    {
        Machine.ONOFF_FLASH = Machine.ONOFF;
        switch_FLASH.save();
    }
}

void Machine_class::Background()
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
        Fiber_comm[0].input_flags.send_modbus = 1;
//        Fiber_comm[1].input_flags.send_modbus = 1;
//        Fiber_comm[2].input_flags.send_modbus = 1;
//        Fiber_comm[3].input_flags.send_modbus = 1;
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

    Fiber_comm[0].Main();//musza byc wszystkie
    Fiber_comm[1].Main();
    Fiber_comm[2].Main();
    Fiber_comm[3].Main();

    SD_card.Scope_snapshot_task();

    if(Machine.error_retry)
    {
        if(Timer_total.minutes + Timer_total.hours + Timer_total.days || !Machine.ONOFF)
        {
            Machine.error_retry = 0;
            status_master.error_retry = Machine.error_retry;
            error_retry_FLASH.save();
        }
    }

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
        if(Machine.save_to_RTC && !control_master.triggers.bit.CPU_reset)
        {
            Machine.save_to_RTC = 0;

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

    if(control_master.triggers.bit.ONOFF_set)
    {
        control_master.triggers.bit.ONOFF_set = 0;
        Machine.ONOFF_temp = 1;
    }

    if(control_master.triggers.bit.ONOFF_reset)
    {
        control_master.triggers.bit.ONOFF_reset = 0;
        Machine.ONOFF_temp = 0;
    }

    if(control_master.triggers.bit.SD_save_H_settings)
    {
        control_master.triggers.bit.SD_save_H_settings = 0;

        memcpy(SD_card.harmonics.on_off_odd_a, on_off_odd_a, sizeof(SD_card.harmonics.on_off_odd_a));
        memcpy(SD_card.harmonics.on_off_odd_b, on_off_odd_b, sizeof(SD_card.harmonics.on_off_odd_b));
        memcpy(SD_card.harmonics.on_off_odd_c, on_off_odd_c, sizeof(SD_card.harmonics.on_off_odd_c));
        memcpy(SD_card.harmonics.on_off_even_a, on_off_even_a, sizeof(SD_card.harmonics.on_off_even_a));
        memcpy(SD_card.harmonics.on_off_even_b, on_off_even_b, sizeof(SD_card.harmonics.on_off_even_b));
        memcpy(SD_card.harmonics.on_off_even_c, on_off_even_c, sizeof(SD_card.harmonics.on_off_even_c));
        SD_card.harmonics.available = 1;

        if(!SD_card.save_H_settings()) status_master.SD_no_harmonic_settings = 0;
        else status_master.SD_no_harmonic_settings = 1;
    }

    if(control_master.triggers.bit.SD_save_settings)
    {
        control_master.triggers.bit.SD_save_settings = 0;

        SD_card.settings.control = control_master;
        SD_card.settings.available = 1;
        SD_card.settings.Baudrate = 100.0*control_ext_modbus.fields.baudrate;
        SD_card.settings.modbus_ext_server_id = control_ext_modbus.fields.ext_server_id;
        SD_card.settings.wifi_on = status_master.wifi_on;

        if(!SD_card.save_settings()) status_master.SD_no_settings = 0;
        else status_master.SD_no_settings = 1;
    }

    if(control_master.triggers.bit.SD_reset_energy_meter)
    {
        control_master.triggers.bit.SD_reset_energy_meter = 0;

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

    if(control_master.triggers.bit.CPU_reset
            && Modbus_slave_LCD.RTU->state == Modbus_RTU_class::Modbus_RTU_idle
            && Modbus_slave_EXT.RTU->state == Modbus_RTU_class::Modbus_RTU_idle)
    {
        control_master.triggers.bit.CPU_reset = 0;

        EALLOW;
        NmiIntruptRegs.NMICFG.bit.NMIE = 1;
        NmiIntruptRegs.NMIWDPRD = 0;
        NmiIntruptRegs.NMIFLGFRC.bit.OVF = 1;
        EDIS;
    }

}

void Machine_class::Main()
{
    if( !is_correct() )
        GPIO_WRITE( LED3_CM, 1 );
    else
        GPIO_WRITE( LED3_CM, 0 );

    register void (*pointer_temp)() = Machine.state_pointers[Machine.state];

    if(pointer_temp != NULL && Machine.state < sizeof(Machine_class::state_pointers)/sizeof(Machine_class::state_pointers[0]))
        (*pointer_temp)();
    else
        Machine.state = state_init;
}

void Machine_class::init()
{
    memset(&Modbus_Converter.coils, 0, sizeof(Modbus_Converter.coils));
    memset(&Modbus_Converter.discrete_inputs, 0, sizeof(Modbus_Converter.discrete_inputs));
    memset(&Modbus_Converter.holding_registers, 0, sizeof(Modbus_Converter.holding_registers));
    memset(&Modbus_Converter.input_registers, 0, sizeof(Modbus_Converter.input_registers));
    memset(&Machine, 0, sizeof(Machine));
    memset(&Conv, 0, sizeof(Conv));
    memset(&Meas_master, 0, sizeof(Meas_master));
    memset(&Meas_master_offset, 0, sizeof(Meas_master_offset));
    memset(&Meas_master_gain, 0, sizeof(Meas_master_gain));
    memset(&Meas_alarm_H, 0, sizeof(Meas_alarm_H));
    memset(&Meas_alarm_L, 0, sizeof(Meas_alarm_L));
    memset(&CPU1toCPU2, 0, sizeof(CPU1toCPU2));
    memset(&CT_char_vars, 0, sizeof(CT_char_vars));
    memset(&Energy_meter, 0, sizeof(Energy_meter));
    memset(&Grid, 0, sizeof(Grid));
    memset(&Grid_filter, 0, sizeof(Grid_filter));
    memset(&Grid_params, 0, sizeof(Grid_params));
    memset(&Grid_filter_params, 0, sizeof(Grid_filter_params));

    memset(&control_master, 0, sizeof(control_master));
    memset(&control_ext_modbus, 0, sizeof(control_ext_modbus));
    memset(&status_master, 0, sizeof(status_master));
    memset(&alarm_master, 0, sizeof(alarm_master));
    memset(&alarm_master_snapshot, 0, sizeof(alarm_master_snapshot));

    Fiber_comm[0].node_number = 0;
    Fiber_comm[1].node_number = 1;
    Fiber_comm[2].node_number = 2;
    Fiber_comm[3].node_number = 3;

    if(L_grid_FLASH.retrieve()) L_grid_meas.L_grid_previous[0] = 100e-6;
    error_retry_FLASH.retrieve();
    status_master.error_retry = Machine.error_retry;

    SD_card.read_settings();
    Modbus_slave_EXT_translated.slave_address = SD_card.settings.modbus_ext_server_id;
    Modbus_slave_EXT.slave_address = MODBUS_EXT_ADDRESS;
    status_master.wifi_on = SD_card.settings.wifi_on;
    GPIO_WRITE( LED5_CM, status_master.wifi_on );

    SD_card.read_CT_characteristic();
    SD_card.read_H_settings();
    SD_card.read_calibration_data();
    SD_card.read_meter_data();
    if(SD_card.meter.available) memcpy(&Energy_meter.upper, &SD_card.meter.Energy_meter, sizeof(Energy_meter.upper));
    else status_master.SD_no_meter = 1;

    if(!SD_card.harmonics.available || !SD_card.settings.available || !SD_card.calibration.available || !SD_card.CT_char.available)
        status_master.SD_card_not_enough_data = 1;
    else
        status_master.SD_card_not_enough_data = 0;

    if(!SD_card.CT_char.available) status_master.SD_no_CT_characteristic = 1;
    if(!SD_card.calibration.available) status_master.SD_no_calibration = 1;
    if(!SD_card.settings.available) status_master.SD_no_settings = 1;
    if(!SD_card.harmonics.available) status_master.SD_no_harmonic_settings = 1;

    if(status_master.SD_card_not_enough_data)
    {
        alarm_master.bit.Not_enough_data_master = 1;

        Meas_master_gain.def_osr = EMIF_mem.read.def_osr;
        Meas_master_gain.sd_shift = EMIF_mem.read.sd_shift;

        Meas_master_gain.U_grid.a =
        Meas_master_gain.U_grid.b =
        Meas_master_gain.U_grid.c = 0.064/(Meas_master_gain.def_osr*Meas_master_gain.def_osr)*Meas_master_gain.sd_shift*(680.0*3.0 + 0.27)/(0.27)*(1.0 + (0.54/4.9));

        Meas_master_gain.I_grid.a =
        Meas_master_gain.I_grid.b =
        Meas_master_gain.I_grid.c = 0.064/(Meas_master_gain.def_osr*Meas_master_gain.def_osr)*Meas_master_gain.sd_shift/0.005;

        Meas_master_gain.U_dc = 0.064/(Meas_master_gain.def_osr*Meas_master_gain.def_osr)*Meas_master_gain.sd_shift*(680.0*6.0 + 0.24)/(0.24)*(1.0 + (0.48/4.9));

        Meas_master_gain.U_dc_n = 0.064/(Meas_master_gain.def_osr*Meas_master_gain.def_osr)*Meas_master_gain.sd_shift*(680.0*3.0 + 0.24)/(0.24)*(1.0 + (0.48/4.9));

        Meas_master_gain.I_conv.a =
        Meas_master_gain.I_conv.b =
        Meas_master_gain.I_conv.c =
        Meas_master_gain.I_conv.n = 0.064/(Meas_master_gain.def_osr*Meas_master_gain.def_osr)*Meas_master_gain.sd_shift/0.001;

        control_ext_modbus.fields.baudrate = 1152;
        control_ext_modbus.fields.ext_server_id = 1;

        SD_card.settings.C_dc = 990e-6;
        SD_card.settings.L_conv = 265e-6;
        SD_card.settings.I_lim =
        Conv.I_lim_nominal = 24.0f;
    }
    else
    {
        control_master.Q_set = SD_card.settings.control.Q_set;
        control_master.flags.bit.enable_Q_comp_a = SD_card.settings.control.flags.bit.enable_Q_comp_a;
        control_master.flags.bit.enable_Q_comp_b = SD_card.settings.control.flags.bit.enable_Q_comp_b;
        control_master.flags.bit.enable_Q_comp_c = SD_card.settings.control.flags.bit.enable_Q_comp_c;
        control_master.flags.bit.enable_P_sym =  SD_card.settings.control.flags.bit.enable_P_sym;
        control_master.flags.bit.enable_H_comp = SD_card.settings.control.flags.bit.enable_H_comp;
        control_master.flags.bit.version_P_sym = SD_card.settings.control.flags.bit.version_P_sym;
        control_master.flags.bit.version_Q_comp_a = SD_card.settings.control.flags.bit.version_Q_comp_a;
        control_master.flags.bit.version_Q_comp_b = SD_card.settings.control.flags.bit.version_Q_comp_b;
        control_master.flags.bit.version_Q_comp_c = SD_card.settings.control.flags.bit.version_Q_comp_c;
        control_master.tangens_range[0] = SD_card.settings.control.tangens_range[0];
        control_master.tangens_range[1] = SD_card.settings.control.tangens_range[1];
        control_ext_modbus.fields.baudrate = SD_card.settings.Baudrate/100;
        control_ext_modbus.fields.ext_server_id = SD_card.settings.modbus_ext_server_id;
        Conv.C_dc = SD_card.settings.C_dc;
        Conv.L_conv = SD_card.settings.L_conv;
        Conv.I_lim_nominal = SD_card.settings.I_lim;

        memcpy(on_off_odd_a, SD_card.harmonics.on_off_odd_a, sizeof(on_off_odd_a));
        memcpy(on_off_odd_b, SD_card.harmonics.on_off_odd_b, sizeof(on_off_odd_b));
        memcpy(on_off_odd_c, SD_card.harmonics.on_off_odd_c, sizeof(on_off_odd_c));
        memcpy(on_off_even_a, SD_card.harmonics.on_off_even_a, sizeof(on_off_even_a));
        memcpy(on_off_even_b, SD_card.harmonics.on_off_even_b, sizeof(on_off_even_b));
        memcpy(on_off_even_c, SD_card.harmonics.on_off_even_c, sizeof(on_off_even_c));
        memcpy(&CT_char_vars.CT_char, &SD_card.CT_char, sizeof(CT_char_vars.CT_char));
        memcpy(&CT_char_vars.calibration, &SD_card.calibration, sizeof(CT_char_vars.calibration));

        Meas_master_gain = SD_card.calibration.Meas_master_gain;
        Meas_master_offset = SD_card.calibration.Meas_master_offset;
        register float ratio_SD = (SD_card.calibration.Meas_master_gain.def_osr * SD_card.calibration.Meas_master_gain.def_osr) / ((float)EMIF_mem.read.def_osr * (float)EMIF_mem.read.def_osr);
        ratio_SD *= (float)EMIF_mem.read.sd_shift / SD_card.calibration.Meas_master_gain.sd_shift;
        Meas_master_gain.U_grid.a *= ratio_SD;
        Meas_master_gain.U_grid.b *= ratio_SD;
        Meas_master_gain.U_grid.c *= ratio_SD;
        Meas_master_gain.U_dc     *= ratio_SD;
        Meas_master_gain.U_dc_n   *= ratio_SD;
        Meas_master_gain.I_conv.a *= ratio_SD;
        Meas_master_gain.I_conv.b *= ratio_SD;
        Meas_master_gain.I_conv.c *= ratio_SD;
        Meas_master_gain.I_conv.n *= ratio_SD;
        Meas_master_gain.I_grid.a *= ratio_SD;
        Meas_master_gain.I_grid.b *= ratio_SD;
        Meas_master_gain.I_grid.c *= ratio_SD;

        Meas_master_offset.U_grid.a *= ratio_SD;
        Meas_master_offset.U_grid.b *= ratio_SD;
        Meas_master_offset.U_grid.c *= ratio_SD;
        Meas_master_offset.U_dc     *= ratio_SD;
        Meas_master_offset.U_dc_n   *= ratio_SD;
        Meas_master_offset.I_conv.a *= ratio_SD;
        Meas_master_offset.I_conv.b *= ratio_SD;
        Meas_master_offset.I_conv.c *= ratio_SD;
        Meas_master_offset.I_conv.n *= ratio_SD;
        Meas_master_offset.I_grid.a *= ratio_SD;
        Meas_master_offset.I_grid.b *= ratio_SD;
        Meas_master_offset.I_grid.c *= ratio_SD;
    }

    CPU1toCPU2.Meas_master_gain = Meas_master_gain;
    CPU1toCPU2.Meas_master_offset = Meas_master_offset;
    IpcRegs.IPCSET.bit.IPC4 = 1;

    Machine.harmonics_odd_last =
    Machine.harmonics_even_last =
    Machine.harmonics_odd =
    Machine.harmonics_even = 0;

    update_harmonics();
    convert_harmonics_to_bits();

    if(!switch_FLASH.retrieve())
    {
        Machine.ONOFF = Machine.ONOFF_FLASH;
        Machine.ONOFF_last = !Machine.ONOFF_FLASH;
    }
    else
    {
        Machine.ONOFF =
        Machine.ONOFF_last = 0;
    }

    Machine.ONOFF_switch = !GPIO_READ(ON_OFF_CM);
    Machine.ONOFF_switch_last = Machine.ONOFF_switch;
    Machine.switch_timer = 1e6;
    Machine.ONOFF_temp = Machine.ONOFF;

    Machine.node_number = 0x0F;

    Init.Variables();

    union
    {
        Uint32 u32;
        int16 i16[2];
    }Meas_alarm_int;

    Meas_alarm_int.i16[0] = Meas_alarm_H.I_conv / Meas_master_gain.I_conv.a + Meas_master_offset.I_conv.a;
    Meas_alarm_int.i16[1] = Meas_alarm_L.I_conv / Meas_master_gain.I_conv.a + Meas_master_offset.I_conv.a;
    EMIF_mem.write.I_conv_a_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = Meas_alarm_H.I_conv / Meas_master_gain.I_conv.b + Meas_master_offset.I_conv.b;
    Meas_alarm_int.i16[1] = Meas_alarm_L.I_conv / Meas_master_gain.I_conv.b + Meas_master_offset.I_conv.b;
    EMIF_mem.write.I_conv_b_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = Meas_alarm_H.I_conv / Meas_master_gain.I_conv.c + Meas_master_offset.I_conv.c;
    Meas_alarm_int.i16[1] = Meas_alarm_L.I_conv / Meas_master_gain.I_conv.c + Meas_master_offset.I_conv.c;
    EMIF_mem.write.I_conv_c_lim = Meas_alarm_int.u32;
    Meas_master_gain.I_conv.n = (Meas_master_gain.I_conv.a + Meas_master_gain.I_conv.b + Meas_master_gain.I_conv.c) * MATH_1_3;
    Meas_master_offset.I_conv.n = Meas_master_offset.I_conv.a + Meas_master_offset.I_conv.b + Meas_master_offset.I_conv.c;
    Meas_alarm_int.i16[0] = (Meas_alarm_H.I_conv / Meas_master_gain.I_conv.n + Meas_master_offset.I_conv.n) * 0.25f;
    Meas_alarm_int.i16[1] = (Meas_alarm_L.I_conv / Meas_master_gain.I_conv.n + Meas_master_offset.I_conv.n) * 0.25f;
    EMIF_mem.write.I_conv_n_lim = Meas_alarm_int.u32;

    Meas_alarm_int.i16[0] = +Meas_alarm_H.U_grid_abs / Meas_master_gain.U_grid.a + Meas_master_offset.U_grid.a;
    Meas_alarm_int.i16[1] = -Meas_alarm_H.U_grid_abs / Meas_master_gain.U_grid.a + Meas_master_offset.U_grid.a;
    EMIF_mem.write.U_grid_a_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = +Meas_alarm_H.U_grid_abs / Meas_master_gain.U_grid.b + Meas_master_offset.U_grid.b;
    Meas_alarm_int.i16[1] = -Meas_alarm_H.U_grid_abs / Meas_master_gain.U_grid.b + Meas_master_offset.U_grid.b;
    EMIF_mem.write.U_grid_b_lim = Meas_alarm_int.u32;
    Meas_alarm_int.i16[0] = +Meas_alarm_H.U_grid_abs / Meas_master_gain.U_grid.c + Meas_master_offset.U_grid.c;
    Meas_alarm_int.i16[1] = -Meas_alarm_H.U_grid_abs / Meas_master_gain.U_grid.c + Meas_master_offset.U_grid.c;
    EMIF_mem.write.U_grid_c_lim = Meas_alarm_int.u32;

    Modbus_RTU_class::Modbus_RTU_parameters_struct RTU_LCD_parameters;
    RTU_LCD_parameters.use_DERE = 1;
    RTU_LCD_parameters.DERE_pin = EN_Mod_1_CM;
    RTU_LCD_parameters.RX_pin = RX_Mod_1_CM;
    RTU_LCD_parameters.TX_pin = TX_Mod_1_CM;
    RTU_LCD_parameters.SciRegs = &SciaRegs;
    RTU_LCD_parameters.ECapRegs = &ECap1Regs;
    RTU_LCD_parameters.baudrate = 115200;
    Modbus_slave_LCD.RTU->init(&RTU_LCD_parameters);

    Modbus_RTU_class::Modbus_RTU_parameters_struct RTU_EXT_parameters;
    RTU_EXT_parameters.use_DERE = 1;
    RTU_EXT_parameters.DERE_pin = EN_Mod_2_CM;
    RTU_EXT_parameters.RX_pin = RX_Mod_2_CM;
    RTU_EXT_parameters.TX_pin = TX_Mod_2_CM;
    RTU_EXT_parameters.SciRegs = &ScidRegs;
    RTU_EXT_parameters.ECapRegs = &ECap2Regs;
    if(SD_card.settings.Baudrate >= 9600) RTU_EXT_parameters.baudrate = SD_card.settings.Baudrate;
    else RTU_EXT_parameters.baudrate = 9600;
    Modbus_slave_EXT.RTU->init(&RTU_EXT_parameters);

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

    status_master.Init_done = 1;
    Machine.recent_error = 1;
    Machine.state = state_idle;
}

void Machine_class::idle()
{
    static Uint64 delay_timer;
    static float delay = 0.0f;
    if(Machine.state_last != Machine.state)
    {
        delay_timer = ReadIpcTimer();
        Machine.state_last = Machine.state;
        Conv.enable = 0;
    }

//    if(ReadIpcTimer() < 2000000000ULL)//10s
//    {
//        if(alarm_master.bit.FPGA_errors.bit.rx1_port_nrdy ||
//            alarm_master.bit.FPGA_errors.bit.rx1_overrun_error ||
//            alarm_master.bit.FPGA_errors.bit.rx1_frame_error ||
//            alarm_master.bit.FPGA_errors.bit.rx1_crc_error)
//            Init.clear_alarms();
//    }

    if(Machine.ONOFF)
    {
        float Temp = fmaxf(Meas_master.Temperature1, fmaxf(Meas_master.Temperature2, Meas_master.Temperature3));
        if(status_master.SD_no_calibration) Machine.state = state_calibrate_offsets;
        else if(status_master.PLL_sync && status_master.Grid_present && Temp < Meas_alarm_H.Temp - 15.0f)
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
            Uint16 index_temp = Machine.error_retry;
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
            if(delay_timer_real > delay) Machine.state = state_start;
        }
    }
}

void Machine_class::calibrate_offsets()
{
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;
    }

    if(Machine.ONOFF_last != Machine.ONOFF)
    {
        GPIO_SET(LED2_CM);
        memcpy(&SD_card.calibration.Meas_master_gain, &Meas_master_gain, sizeof(Meas_master_gain));

        CIC2_calibration_input.ptr = &Meas_master.I_grid.a;
        DELAY_US(100000);
        Meas_master_offset.I_grid.a += CIC2_calibration.out / Meas_master_gain.I_grid.a;

        CIC2_calibration_input.ptr = &Meas_master.I_grid.b;
        DELAY_US(100000);
        Meas_master_offset.I_grid.b += CIC2_calibration.out / Meas_master_gain.I_grid.b;

        CIC2_calibration_input.ptr = &Meas_master.I_grid.c;
        DELAY_US(100000);
        Meas_master_offset.I_grid.c += CIC2_calibration.out / Meas_master_gain.I_grid.c;


        CIC2_calibration_input.ptr = &Meas_master.U_grid.a;
        DELAY_US(100000);
        Meas_master_offset.U_grid.a += CIC2_calibration.out / Meas_master_gain.U_grid.a;

        CIC2_calibration_input.ptr = &Meas_master.U_grid.b;
        DELAY_US(100000);
        Meas_master_offset.U_grid.b += CIC2_calibration.out / Meas_master_gain.U_grid.b;

        CIC2_calibration_input.ptr = &Meas_master.U_grid.c;
        DELAY_US(100000);
        Meas_master_offset.U_grid.c += CIC2_calibration.out / Meas_master_gain.U_grid.c;

        CIC2_calibration_input.ptr = &Meas_master.I_conv.a;
        DELAY_US(100000);
        Meas_master_offset.I_conv.a += CIC2_calibration.out / Meas_master_gain.I_conv.a;

        CIC2_calibration_input.ptr = &Meas_master.I_conv.b;
        DELAY_US(100000);
        Meas_master_offset.I_conv.b += CIC2_calibration.out / Meas_master_gain.I_conv.b;

        CIC2_calibration_input.ptr = &Meas_master.I_conv.c;
        DELAY_US(100000);
        Meas_master_offset.I_conv.c += CIC2_calibration.out / Meas_master_gain.I_conv.c;

        CIC2_calibration_input.ptr = &Meas_master.I_conv.n;
        DELAY_US(100000);
        Meas_master_offset.I_conv.n += CIC2_calibration.out / Meas_master_gain.I_conv.n;

        CIC2_calibration_input.ptr = &Meas_master.U_dc;
        DELAY_US(100000);
        Meas_master_offset.U_dc += CIC2_calibration.out / Meas_master_gain.U_dc;

        CIC2_calibration_input.ptr = &Meas_master.U_dc_n;
        DELAY_US(100000);
        Meas_master_offset.U_dc_n += CIC2_calibration.out / Meas_master_gain.U_dc_n;

        Meas_master_offset_error.I_conv.a = fabsf(Meas_master_offset.I_conv.a * Meas_master_gain.I_conv.a);
        Meas_master_offset_error.I_conv.b = fabsf(Meas_master_offset.I_conv.b * Meas_master_gain.I_conv.b);
        Meas_master_offset_error.I_conv.c = fabsf(Meas_master_offset.I_conv.c * Meas_master_gain.I_conv.c);
        Meas_master_offset_error.I_conv.n = fabsf(Meas_master_offset.I_conv.n * Meas_master_gain.I_conv.n);
        Meas_master_offset_error.U_dc_n = fabsf(Meas_master_offset.U_dc_n * Meas_master_gain.U_dc_n);
        Meas_master_offset_error.U_dc = fabsf(Meas_master_offset.U_dc * Meas_master_gain.U_dc);


        Meas_master_offset_error.I_grid.a = fabsf(Meas_master_offset.I_grid.a * Meas_master_gain.I_grid.a);
        Meas_master_offset_error.I_grid.b = fabsf(Meas_master_offset.I_grid.b * Meas_master_gain.I_grid.b);
        Meas_master_offset_error.I_grid.c = fabsf(Meas_master_offset.I_grid.c * Meas_master_gain.I_grid.c);
        Meas_master_offset_error.U_grid.a = fabsf(Meas_master_offset.U_grid.a * Meas_master_gain.U_grid.a);
        Meas_master_offset_error.U_grid.b = fabsf(Meas_master_offset.U_grid.b * Meas_master_gain.U_grid.b);
        Meas_master_offset_error.U_grid.c = fabsf(Meas_master_offset.U_grid.c * Meas_master_gain.U_grid.c);

        if (Meas_master_offset_error.I_grid.a > 0.05f ||
            Meas_master_offset_error.I_grid.b > 0.05f ||
            Meas_master_offset_error.I_grid.c > 0.05f ||
            Meas_master_offset_error.U_grid.a > 2.0f ||
            Meas_master_offset_error.U_grid.b > 2.0f ||
            Meas_master_offset_error.U_grid.c > 2.0f ||
            Meas_master_offset_error.I_conv.a > 0.1f ||
            Meas_master_offset_error.I_conv.b > 0.1f ||
            Meas_master_offset_error.I_conv.c > 0.1f ||
            Meas_master_offset_error.I_conv.n > 0.1f ||
            Meas_master_offset_error.U_dc_n > 1.0f ||
            Meas_master_offset_error.U_dc > 1.0f)
        {
            status_master.calibration_procedure_error = 1;
        }
        else
        {
            status_master.calibration_procedure_error = 0;
            Machine.state = state_calibrate_curent_gain;
        }
    }
}

void Machine_class::calibrate_curent_gain()
{
    static Uint16 calib_rdy;
    static Uint16 calib_rdy_last;
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;
        calib_rdy = 0;
        calib_rdy_last = 0;
    }

    if(Machine.ONOFF_last != Machine.ONOFF)
    {
        GPIO_SET(LED2_CM);

        float I_cal = 2.0f;
        if(fabsf(Meas_master.I_grid.a) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.I_grid.a;
            DELAY_US(100000);
            Meas_master_gain.I_grid.a = fabsf(Meas_master_gain.I_grid.a * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<0;
        }
        if(fabsf(Meas_master.I_grid.b) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.I_grid.b;
            DELAY_US(100000);
            Meas_master_gain.I_grid.b = fabsf(Meas_master_gain.I_grid.b * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<1;
        }
        if(fabsf(Meas_master.I_grid.c) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.I_grid.c;
            DELAY_US(100000);
            Meas_master_gain.I_grid.c = fabsf(Meas_master_gain.I_grid.c * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<2;
        }
        if(fabsf(Meas_master.I_conv.a) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.I_conv.a;
            DELAY_US(100000);
            Meas_master_gain.I_conv.a = fabsf(Meas_master_gain.I_conv.a * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<3;
        }
        if(fabsf(Meas_master.I_conv.b) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.I_conv.b;
            DELAY_US(100000);
            Meas_master_gain.I_conv.b = fabsf(Meas_master_gain.I_conv.b * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<4;
        }
        if(fabsf(Meas_master.I_conv.c) > I_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.I_conv.c;
            DELAY_US(100000);
            Meas_master_gain.I_conv.c = fabsf(Meas_master_gain.I_conv.c * 5.0f / CIC2_calibration.out);
            calib_rdy |= 1<<5;
        }
//        if(fabsf(Meas_master.I_conv.n) > I_cal)
//        {
//            CIC2_calibration_input.ptr = &Meas_master.I_conv.n;
//            DELAY_US(100000);
//            Meas_master_gain.I_conv.n = -fabsf(Meas_master_gain.I_conv.n * 5.0f / CIC2_calibration.out);
//            calib_rdy |= 1<<6;
//        }

        if(calib_rdy == calib_rdy_last)
        {
            status_master.calibration_procedure_error = 1;
        }
        calib_rdy_last = calib_rdy;

        if(calib_rdy == 0x3F)
        {
            Meas_master_gain_error.I_grid.a = fabsf((Meas_master_gain.I_grid.a/SD_card.calibration.Meas_master_gain.I_grid.a) - 1.0f);
            Meas_master_gain_error.I_grid.b = fabsf((Meas_master_gain.I_grid.b/SD_card.calibration.Meas_master_gain.I_grid.b) - 1.0f);
            Meas_master_gain_error.I_grid.c = fabsf((Meas_master_gain.I_grid.c/SD_card.calibration.Meas_master_gain.I_grid.c) - 1.0f);

            float mean_gain_meas = (Meas_master_gain.I_conv.a + Meas_master_gain.I_conv.b + Meas_master_gain.I_conv.c) / 3.0f;
            Meas_master_gain_error.I_conv.a = fabsf((Meas_master_gain.I_conv.a/mean_gain_meas) - 1.0f);
            Meas_master_gain_error.I_conv.b = fabsf((Meas_master_gain.I_conv.b/mean_gain_meas) - 1.0f);
            Meas_master_gain_error.I_conv.c = fabsf((Meas_master_gain.I_conv.c/mean_gain_meas) - 1.0f);
//            Meas_master_gain_error.I_conv.n = fabsf((Meas_master_gain.I_conv.n/mean_gain_meas) - 1.0f);

            if (Meas_master_gain_error.I_grid.a > 0.03f ||
                Meas_master_gain_error.I_grid.b > 0.03f ||
                Meas_master_gain_error.I_grid.c > 0.03f ||
                Meas_master_gain_error.I_conv.a > 0.03f ||
                Meas_master_gain_error.I_conv.b > 0.03f ||
                Meas_master_gain_error.I_conv.c > 0.03f ||
                Meas_master_gain_error.I_conv.n > 0.03f)
            {
                status_master.calibration_procedure_error = 1;
            }
            else
            {
                status_master.calibration_procedure_error = 0;
                Machine.state = state_calibrate_AC_voltage_gain;
            }
        }
    }
}

void Machine_class::calibrate_AC_voltage_gain()
{
    static Uint16 calib_rdy;
    static Uint16 calib_rdy_last;
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;
        calib_rdy = 0;
        calib_rdy_last = 0;
    }

    if(Machine.ONOFF_last != Machine.ONOFF)
    {
        GPIO_SET(LED2_CM);

        float U_cal = 28.0f;
        if(fabsf(Meas_master.U_grid.a) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.U_grid.a;
            DELAY_US(100000);
            Meas_master_gain.U_grid.a = fabsf(Meas_master_gain.U_grid.a * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<0;
        }
        if(fabsf(Meas_master.U_grid.b) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.U_grid.b;
            DELAY_US(100000);
            Meas_master_gain.U_grid.b = fabsf(Meas_master_gain.U_grid.b * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<1;
        }
        if(fabsf(Meas_master.U_grid.c) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.U_grid.c;
            DELAY_US(100000);
            Meas_master_gain.U_grid.c = fabsf(Meas_master_gain.U_grid.c * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<2;
        }

        if(calib_rdy == calib_rdy_last)
        {
            status_master.calibration_procedure_error = 1;
        }
        calib_rdy_last = calib_rdy;

        if(calib_rdy == 7)
        {
            Meas_master_gain_error.U_grid.a = fabsf((Meas_master_gain.U_grid.a/SD_card.calibration.Meas_master_gain.U_grid.a) - 1.0f);
            Meas_master_gain_error.U_grid.b = fabsf((Meas_master_gain.U_grid.b/SD_card.calibration.Meas_master_gain.U_grid.b) - 1.0f);
            Meas_master_gain_error.U_grid.c = fabsf((Meas_master_gain.U_grid.c/SD_card.calibration.Meas_master_gain.U_grid.c) - 1.0f);

            if (Meas_master_gain_error.U_grid.a > 0.03f ||
                Meas_master_gain_error.U_grid.b > 0.03f ||
                Meas_master_gain_error.U_grid.c > 0.03f)
            {
                status_master.calibration_procedure_error = 1;
            }
            else
            {
                status_master.calibration_procedure_error = 0;
                Machine.state = state_calibrate_DC_voltage_gain;
            }
        }
    }
}

void Machine_class::calibrate_DC_voltage_gain()
{
    static Uint16 calib_rdy;
    static Uint16 calib_rdy_last;
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;
        calib_rdy = 0;
        calib_rdy_last = 0;
    }

    if(Machine.ONOFF_last != Machine.ONOFF)
    {
        GPIO_SET(LED2_CM);
        float U_cal = 28.0f;

        if(fabsf(Meas_master.U_dc) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.U_dc;
            DELAY_US(100000);
            Meas_master_gain.U_dc = fabsf(Meas_master_gain.U_dc * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<0;
        }

        if(fabsf(Meas_master.U_dc_n) > U_cal)
        {
            CIC2_calibration_input.ptr = &Meas_master.U_dc_n;
            DELAY_US(100000);
            Meas_master_gain.U_dc_n = fabsf(Meas_master_gain.U_dc_n * 30.0f / CIC2_calibration.out);
            calib_rdy |= 1<<1;
        }

        if(calib_rdy == calib_rdy_last)
        {
            status_master.calibration_procedure_error = 1;
        }
        calib_rdy_last = calib_rdy;

        if(calib_rdy == 3)
        {
            Meas_master_gain_error.U_dc = fabsf((Meas_master_gain.U_dc/SD_card.calibration.Meas_master_gain.U_dc) - 1.0f);
            Meas_master_gain_error.U_dc_n = fabsf((Meas_master_gain.U_dc_n/SD_card.calibration.Meas_master_gain.U_dc_n) - 1.0f);

            if (Meas_master_gain_error.U_dc_n > 0.03f ||
                Meas_master_gain_error.U_dc > 0.03f)
            {
                status_master.calibration_procedure_error = 1;
            }
            else
            {
                status_master.calibration_procedure_error = 0;
                memcpy(&SD_card.calibration.Meas_master_gain, &Meas_master_gain, sizeof(Meas_master_gain));
                memcpy(&SD_card.calibration.Meas_master_offset, &Meas_master_offset, sizeof(Meas_master_offset));
                SD_card.save_calibration_data();
                Machine.state = state_init;
                control_master.triggers.bit.CPU_reset = 1;
            }
        }
        else
        {
            status_master.calibration_procedure_error = 1;
        }
    }

}

void Machine_class::start()
{
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;

        if(Machine.error_retry) Machine.recent_error = 1;
        if(++Machine.error_retry >= 16) Machine.error_retry = 15;
        status_master.error_retry = Machine.error_retry;
        error_retry_FLASH.save();

        memset(&Timer_total, 0, sizeof(Timer_total));
        timer_update(&Timer_total, 0);

        Conv.tangens_range_local_prefilter[0].a.out =
        Conv.tangens_range_local_prefilter[0].b.out =
        Conv.tangens_range_local_prefilter[0].c.out =
        Conv.tangens_range_local_prefilter[1].a.out =
        Conv.tangens_range_local_prefilter[1].b.out =
        Conv.tangens_range_local_prefilter[1].c.out =
        Conv.Q_set_local_prefilter.a.out =
        Conv.Q_set_local_prefilter.b.out =
        Conv.Q_set_local_prefilter.c.out =
        Conv.version_Q_comp_local_prefilter.a.out =
        Conv.version_Q_comp_local_prefilter.b.out =
        Conv.version_Q_comp_local_prefilter.c.out =
        Conv.enable_Q_comp_local_prefilter.a.out =
        Conv.enable_Q_comp_local_prefilter.b.out =
        Conv.enable_Q_comp_local_prefilter.c.out =
        Conv.version_P_sym_local_prefilter.out =
        Conv.enable_P_sym_local_prefilter.out =
        Conv.Q_set_local.a =
        Conv.Q_set_local.b =
        Conv.Q_set_local.c =
        Conv.enable_Q_comp_local.a =
        Conv.enable_Q_comp_local.b =
        Conv.enable_Q_comp_local.c =
        Conv.enable_P_sym_local =
        Conv.enable_H_comp_local = 0.0f;

        Conv.PI_U_dc.integrator =
        Conv.PI_Id[0].integrator =
        Conv.PI_Id[1].integrator =
        Conv.PI_Id[2].integrator =
        Conv.PI_Iq[0].integrator =
        Conv.PI_Iq[1].integrator =
        Conv.PI_Iq[2].integrator = 0.0f;

        Init.clear_alarms();

        DELAY_US(100);

        Scope_start();

        if( mosfet_ctrl_app.getState() != MosfetCtrlApp::state_idle ){
            uint16_t mosfet_errors = mosfet_ctrl_app.getErrorsGetStatus();
            mosfet_errors |= mosfet_ctrl_app.getErrorsConfig();
            mosfet_errors |= mosfet_ctrl_app.getErrorsAtPoint(MosfetCtrlApp::logpoint_softreset);

            if( (mosfet_errors & 0xFF) == 0 )
                mosfet_errors = 0xFF;

            alarm_master.bit.Driver_soft_error = 1;
        }

        CIC2_calibration_input.ptr = &Meas_master.I_conv.a;
        DELAY_US(50000);
        Meas_master_offset.I_conv.a += CIC2_calibration.out / Meas_master_gain.I_conv.a;

        CIC2_calibration_input.ptr = &Meas_master.I_conv.b;
        DELAY_US(50000);
        Meas_master_offset.I_conv.b += CIC2_calibration.out / Meas_master_gain.I_conv.b;

        CIC2_calibration_input.ptr = &Meas_master.I_conv.c;
        DELAY_US(50000);
        Meas_master_offset.I_conv.c += CIC2_calibration.out / Meas_master_gain.I_conv.c;

        Conv.enable = 1;
    }

    if(Conv.RDY2)
    {
        if(status_master.CT_connection_a != 1 || status_master.CT_connection_b != 2 || status_master.CT_connection_c != 3)
            Machine.state = state_CT_test;
        else if(status_master.L_grid_measured)
            Machine.state = state_operational;
        else
            Machine.state = state_Lgrid_meas;
    }

    if(!Machine.ONOFF) Machine.state = state_idle;
    if(alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]) Machine.state = state_cleanup;
}

void Machine_class::CT_test()
{
    static struct CT_test_struct
    {
        struct abc_struct Iq_pos[3], Iq_neg[3], Iq_diff[3];
        struct abc_struct Id_pos[3], Id_neg[3], Id_diff[3];
        struct abc_struct CT_gain[3];
    }CT_test_startup;
    static float C_dc_meas[6];

    static Uint64 timer_old;
    static Uint16 CT_test_state;
    static Uint16 CT_test_state_last;
    static Uint16 repeat_counter;
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;

        timer_old = ReadIpcTimer();
        CT_test_state_last = 1;
        CT_test_state = 0;
        repeat_counter = 0;

        Conv.Q_set_local.a =
        Conv.Q_set_local.b =
        Conv.Q_set_local.c =
        Conv.enable_P_sym_local =
        Conv.enable_H_comp_local = 0.0f;
        Conv.enable_Q_comp_local.a =
        Conv.enable_Q_comp_local.b =
        Conv.enable_Q_comp_local.c =
        Conv.version_Q_comp_local.a =
        Conv.version_Q_comp_local.b =
        Conv.version_Q_comp_local.c = 1.0f;
    }
    Uint64 elapsed_time = ReadIpcTimer() - timer_old;

    switch(CT_test_state)
    {
    case 0:
    {
        if(CT_test_state_last != CT_test_state)
        {
            CT_test_state_last = CT_test_state;
            if(repeat_counter == 0) Conv.Q_set_local.a = Grid.U_grid_1h.a * 8.0f;
            else if(repeat_counter == 1) Conv.Q_set_local.b = Grid.U_grid_1h.b * 8.0f;
            else Conv.Q_set_local.c = Grid.U_grid_1h.c * 8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            CT_test_startup.Iq_pos[repeat_counter].a = Conv.iq_load.a - Conv.iq_conv.a;
            CT_test_startup.Iq_pos[repeat_counter].b = Conv.iq_load.b - Conv.iq_conv.b;
            CT_test_startup.Iq_pos[repeat_counter].c = Conv.iq_load.c - Conv.iq_conv.c;
            CT_test_startup.Id_pos[repeat_counter].a = Conv.id_load.a - Conv.id_conv.a;
            CT_test_startup.Id_pos[repeat_counter].b = Conv.id_load.b - Conv.id_conv.b;
            CT_test_startup.Id_pos[repeat_counter].c = Conv.id_load.c - Conv.id_conv.c;
            C_dc_meas[repeat_counter] = Conv.C_dc_meas;
            CT_test_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 1:
    {
        if(CT_test_state_last != CT_test_state)
        {
            CT_test_state_last = CT_test_state;
            if(repeat_counter == 0) Conv.Q_set_local.a = Grid.U_grid_1h.a * -8.0f;
            else if(repeat_counter == 1) Conv.Q_set_local.b = Grid.U_grid_1h.b * -8.0f;
            else Conv.Q_set_local.c = Grid.U_grid_1h.c * -8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            CT_test_startup.Iq_neg[repeat_counter].a = Conv.iq_load.a - Conv.iq_conv.a;
            CT_test_startup.Iq_neg[repeat_counter].b = Conv.iq_load.b - Conv.iq_conv.b;
            CT_test_startup.Iq_neg[repeat_counter].c = Conv.iq_load.c - Conv.iq_conv.c;
            CT_test_startup.Id_neg[repeat_counter].a = Conv.id_load.a - Conv.id_conv.a;
            CT_test_startup.Id_neg[repeat_counter].b = Conv.id_load.b - Conv.id_conv.b;
            CT_test_startup.Id_neg[repeat_counter].c = Conv.id_load.c - Conv.id_conv.c;
            C_dc_meas[repeat_counter+3] = Conv.C_dc_filter.out;
            Conv.Q_set_local.a =
            Conv.Q_set_local.b =
            Conv.Q_set_local.c = 0.0f;
            if(++repeat_counter < 3) CT_test_state = 0;
            else CT_test_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 2:
    {
        if(CT_test_state_last != CT_test_state)
        {
            CT_test_state_last = CT_test_state;

            qsort(C_dc_meas, 6, sizeof(float), compare_float);
            Conv.C_dc_measured = (C_dc_meas[2]+C_dc_meas[3]) * 0.5f;

            CT_test_startup.Iq_diff[0].a = fabsf(CT_test_startup.Iq_pos[0].a - CT_test_startup.Iq_neg[0].a);
            CT_test_startup.Iq_diff[0].b = fabsf(CT_test_startup.Iq_pos[0].b - CT_test_startup.Iq_neg[0].b);
            CT_test_startup.Iq_diff[0].c = fabsf(CT_test_startup.Iq_pos[0].c - CT_test_startup.Iq_neg[0].c);
            CT_test_startup.Iq_diff[1].a = fabsf(CT_test_startup.Iq_pos[1].a - CT_test_startup.Iq_neg[1].a);
            CT_test_startup.Iq_diff[1].b = fabsf(CT_test_startup.Iq_pos[1].b - CT_test_startup.Iq_neg[1].b);
            CT_test_startup.Iq_diff[1].c = fabsf(CT_test_startup.Iq_pos[1].c - CT_test_startup.Iq_neg[1].c);
            CT_test_startup.Iq_diff[2].a = fabsf(CT_test_startup.Iq_pos[2].a - CT_test_startup.Iq_neg[2].a);
            CT_test_startup.Iq_diff[2].b = fabsf(CT_test_startup.Iq_pos[2].b - CT_test_startup.Iq_neg[2].b);
            CT_test_startup.Iq_diff[2].c = fabsf(CT_test_startup.Iq_pos[2].c - CT_test_startup.Iq_neg[2].c);

            CT_test_startup.Id_diff[0].a = fabsf(CT_test_startup.Id_pos[0].a - CT_test_startup.Id_neg[0].a);
            CT_test_startup.Id_diff[0].b = fabsf(CT_test_startup.Id_pos[0].b - CT_test_startup.Id_neg[0].b);
            CT_test_startup.Id_diff[0].c = fabsf(CT_test_startup.Id_pos[0].c - CT_test_startup.Id_neg[0].c);
            CT_test_startup.Id_diff[1].a = fabsf(CT_test_startup.Id_pos[1].a - CT_test_startup.Id_neg[1].a);
            CT_test_startup.Id_diff[1].b = fabsf(CT_test_startup.Id_pos[1].b - CT_test_startup.Id_neg[1].b);
            CT_test_startup.Id_diff[1].c = fabsf(CT_test_startup.Id_pos[1].c - CT_test_startup.Id_neg[1].c);
            CT_test_startup.Id_diff[2].a = fabsf(CT_test_startup.Id_pos[2].a - CT_test_startup.Id_neg[2].a);
            CT_test_startup.Id_diff[2].b = fabsf(CT_test_startup.Id_pos[2].b - CT_test_startup.Id_neg[2].b);
            CT_test_startup.Id_diff[2].c = fabsf(CT_test_startup.Id_pos[2].c - CT_test_startup.Id_neg[2].c);

            CT_test_startup.CT_gain[0].a = 16.0f / CT_test_startup.Iq_diff[0].a;
            CT_test_startup.CT_gain[0].b = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[0].b + 0.5f * CT_test_startup.Iq_diff[0].b);
            CT_test_startup.CT_gain[0].c = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[0].c + 0.5f * CT_test_startup.Iq_diff[0].c);
            CT_test_startup.CT_gain[1].a = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[1].a + 0.5f * CT_test_startup.Iq_diff[1].a);
            CT_test_startup.CT_gain[1].b = 16.0f / CT_test_startup.Iq_diff[1].b;
            CT_test_startup.CT_gain[1].c = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[1].c + 0.5f * CT_test_startup.Iq_diff[1].c);
            CT_test_startup.CT_gain[2].a = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[2].a + 0.5f * CT_test_startup.Iq_diff[2].a);
            CT_test_startup.CT_gain[2].b = 16.0f / (MATH_SQRT3_2 * CT_test_startup.Id_diff[2].b + 0.5f * CT_test_startup.Iq_diff[2].b);
            CT_test_startup.CT_gain[2].c = 16.0f / CT_test_startup.Iq_diff[2].c;

            if(fabsf(CT_test_startup.CT_gain[0].a-1.0f) < 0.5f)
                status_master.CT_connection_a = 1;
            else if(fabsf(CT_test_startup.CT_gain[1].a-1.0f) < 0.5f)
                status_master.CT_connection_a = 2;
            else if(fabsf(CT_test_startup.CT_gain[2].a-1.0f) < 0.5f)
                status_master.CT_connection_a = 3;
            else
                status_master.CT_connection_a = 0;

            if(fabsf(CT_test_startup.CT_gain[0].b-1.0f) < 0.5f)
                status_master.CT_connection_b = 1;
            else if(fabsf(CT_test_startup.CT_gain[1].b-1.0f) < 0.5f)
                status_master.CT_connection_b = 2;
            else if(fabsf(CT_test_startup.CT_gain[2].b-1.0f) < 0.5f)
                status_master.CT_connection_b = 3;
            else
                status_master.CT_connection_b = 0;

            if(fabsf(CT_test_startup.CT_gain[0].c-1.0f) < 0.5f)
                status_master.CT_connection_c = 1;
            else if(fabsf(CT_test_startup.CT_gain[1].c-1.0f) < 0.5f)
                status_master.CT_connection_c = 2;
            else if(fabsf(CT_test_startup.CT_gain[2].c-1.0f) < 0.5f)
                status_master.CT_connection_c = 3;
            else
                status_master.CT_connection_c = 0;


            if(status_master.CT_connection_a == 1) status_master.no_CT_connected_a = 0;
            else status_master.no_CT_connected_a = 1;
            if(status_master.CT_connection_b == 2) status_master.no_CT_connected_b = 0;
            else status_master.no_CT_connected_b = 1;
            if(status_master.CT_connection_c == 3) status_master.no_CT_connected_c = 0;
            else status_master.no_CT_connected_c = 1;
        }

        if(elapsed_time > 50000000ULL)
        {
            Conv.compensation2 = 4000.0f * Saturation(L_grid_meas.L_grid_previous[0], 50e-6, 800e-6) + 1.8f;
            if(status_master.L_grid_measured || status_master.CT_connection_a != 1 || status_master.CT_connection_b != 2 || status_master.CT_connection_c != 3)
                Machine.state = state_operational;
            else
                Machine.state = state_Lgrid_meas;
        }
        break;
    }
    }

    timer_update(&Timer_total, 1);

    if(!Machine.ONOFF) Machine.state = state_idle;
    if(alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]) Machine.state = state_cleanup;
}

void Machine_class::Lgrid_meas()
{
    static Uint64 timer_old;
    static Uint16 Lgrid_meas_state;
    static Uint16 Lgrid_meas_state_last;
    static Uint16 repeat_counter;
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;

        timer_old = ReadIpcTimer();
        Lgrid_meas_state_last = 1;
        Lgrid_meas_state = 0;
        repeat_counter = 0;

        Conv.enable_P_sym_local =
        Conv.enable_H_comp_local = 0.0f;
        Conv.enable_Q_comp_local.a =
        Conv.enable_Q_comp_local.b =
        Conv.enable_Q_comp_local.c =
        Conv.version_Q_comp_local.a =
        Conv.version_Q_comp_local.b =
        Conv.version_Q_comp_local.c = 1.0f;
    }
    Uint64 elapsed_time = ReadIpcTimer() - timer_old;

    switch(Lgrid_meas_state)
    {
    case 0:
    {
        if(Lgrid_meas_state_last != Lgrid_meas_state)
        {
            Lgrid_meas_state_last = Lgrid_meas_state;
            Conv.Q_set_local.a = Grid.U_grid_1h.a * 8.0f;
            Conv.Q_set_local.b = Grid.U_grid_1h.b * 8.0f;
            Conv.Q_set_local.c = Grid.U_grid_1h.c * 8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            L_grid_meas.Iq_pos[repeat_counter].a = Conv.iq_load.a - Conv.iq_conv.a;
            L_grid_meas.Iq_pos[repeat_counter].b = Conv.iq_load.b - Conv.iq_conv.b;
            L_grid_meas.Iq_pos[repeat_counter].c = Conv.iq_load.c - Conv.iq_conv.c;
            L_grid_meas.U_pos[repeat_counter].a = Grid.U_grid.a;
            L_grid_meas.U_pos[repeat_counter].b = Grid.U_grid.b;
            L_grid_meas.U_pos[repeat_counter].c = Grid.U_grid.c;
            Lgrid_meas_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 1:
    {
        if(Lgrid_meas_state_last != Lgrid_meas_state)
        {
            Lgrid_meas_state_last = Lgrid_meas_state;
            Conv.Q_set_local.a = Grid.U_grid_1h.a * -8.0f;
            Conv.Q_set_local.b = Grid.U_grid_1h.b * -8.0f;
            Conv.Q_set_local.c = Grid.U_grid_1h.c * -8.0f;
        }

        if(elapsed_time > 50000000ULL)
        {
            L_grid_meas.Iq_neg[repeat_counter].a = Conv.iq_load.a - Conv.iq_conv.a;
            L_grid_meas.Iq_neg[repeat_counter].b = Conv.iq_load.b - Conv.iq_conv.b;
            L_grid_meas.Iq_neg[repeat_counter].c = Conv.iq_load.c - Conv.iq_conv.c;
            L_grid_meas.U_neg[repeat_counter].a = Grid.U_grid.a;
            L_grid_meas.U_neg[repeat_counter].b = Grid.U_grid.b;
            L_grid_meas.U_neg[repeat_counter].c = Grid.U_grid.c;
            if(++repeat_counter < 5) Lgrid_meas_state = 0;
            else Lgrid_meas_state++;
            timer_old = ReadIpcTimer();
        }
        break;
    }

    case 2:
    {
        Conv.Q_set_local.a =
        Conv.Q_set_local.b =
        Conv.Q_set_local.c = 0.0f;
        L_grid_meas.Iq_diff_a[0] = L_grid_meas.Iq_pos[0].a - L_grid_meas.Iq_neg[0].a;
        L_grid_meas.Iq_diff_b[0] = L_grid_meas.Iq_pos[0].b - L_grid_meas.Iq_neg[0].b;
        L_grid_meas.Iq_diff_c[0] = L_grid_meas.Iq_pos[0].c - L_grid_meas.Iq_neg[0].c;
        L_grid_meas.Iq_diff_a[1] = L_grid_meas.Iq_pos[1].a - L_grid_meas.Iq_neg[1].a;
        L_grid_meas.Iq_diff_b[1] = L_grid_meas.Iq_pos[1].b - L_grid_meas.Iq_neg[1].b;
        L_grid_meas.Iq_diff_c[1] = L_grid_meas.Iq_pos[1].c - L_grid_meas.Iq_neg[1].c;
        L_grid_meas.Iq_diff_a[2] = L_grid_meas.Iq_pos[2].a - L_grid_meas.Iq_neg[2].a;
        L_grid_meas.Iq_diff_b[2] = L_grid_meas.Iq_pos[2].b - L_grid_meas.Iq_neg[2].b;
        L_grid_meas.Iq_diff_c[2] = L_grid_meas.Iq_pos[2].c - L_grid_meas.Iq_neg[2].c;
        L_grid_meas.Iq_diff_a[3] = L_grid_meas.Iq_pos[3].a - L_grid_meas.Iq_neg[3].a;
        L_grid_meas.Iq_diff_b[3] = L_grid_meas.Iq_pos[3].b - L_grid_meas.Iq_neg[3].b;
        L_grid_meas.Iq_diff_c[3] = L_grid_meas.Iq_pos[3].c - L_grid_meas.Iq_neg[3].c;
        L_grid_meas.Iq_diff_a[4] = L_grid_meas.Iq_pos[4].a - L_grid_meas.Iq_neg[4].a;
        L_grid_meas.Iq_diff_b[4] = L_grid_meas.Iq_pos[4].b - L_grid_meas.Iq_neg[4].b;
        L_grid_meas.Iq_diff_c[4] = L_grid_meas.Iq_pos[4].c - L_grid_meas.Iq_neg[4].c;
        L_grid_meas.L_grid[0].a = (L_grid_meas.U_pos[0].a - L_grid_meas.U_neg[0].a) / (L_grid_meas.Iq_diff_a[0] * Conv.w_filter);
        L_grid_meas.L_grid[0].b = (L_grid_meas.U_pos[0].b - L_grid_meas.U_neg[0].b) / (L_grid_meas.Iq_diff_b[0] * Conv.w_filter);
        L_grid_meas.L_grid[0].c = (L_grid_meas.U_pos[0].c - L_grid_meas.U_neg[0].c) / (L_grid_meas.Iq_diff_c[0] * Conv.w_filter);
        L_grid_meas.L_grid[1].a = (L_grid_meas.U_pos[1].a - L_grid_meas.U_neg[1].a) / (L_grid_meas.Iq_diff_a[1] * Conv.w_filter);
        L_grid_meas.L_grid[1].b = (L_grid_meas.U_pos[1].b - L_grid_meas.U_neg[1].b) / (L_grid_meas.Iq_diff_b[1] * Conv.w_filter);
        L_grid_meas.L_grid[1].c = (L_grid_meas.U_pos[1].c - L_grid_meas.U_neg[1].c) / (L_grid_meas.Iq_diff_c[1] * Conv.w_filter);
        L_grid_meas.L_grid[2].a = (L_grid_meas.U_pos[2].a - L_grid_meas.U_neg[2].a) / (L_grid_meas.Iq_diff_a[2] * Conv.w_filter);
        L_grid_meas.L_grid[2].b = (L_grid_meas.U_pos[2].b - L_grid_meas.U_neg[2].b) / (L_grid_meas.Iq_diff_b[2] * Conv.w_filter);
        L_grid_meas.L_grid[2].c = (L_grid_meas.U_pos[2].c - L_grid_meas.U_neg[2].c) / (L_grid_meas.Iq_diff_c[2] * Conv.w_filter);
        L_grid_meas.L_grid[3].a = (L_grid_meas.U_pos[3].a - L_grid_meas.U_neg[3].a) / (L_grid_meas.Iq_diff_a[3] * Conv.w_filter);
        L_grid_meas.L_grid[3].b = (L_grid_meas.U_pos[3].b - L_grid_meas.U_neg[3].b) / (L_grid_meas.Iq_diff_b[3] * Conv.w_filter);
        L_grid_meas.L_grid[3].c = (L_grid_meas.U_pos[3].c - L_grid_meas.U_neg[3].c) / (L_grid_meas.Iq_diff_c[3] * Conv.w_filter);
        L_grid_meas.L_grid[4].a = (L_grid_meas.U_pos[4].a - L_grid_meas.U_neg[4].a) / (L_grid_meas.Iq_diff_a[4] * Conv.w_filter);
        L_grid_meas.L_grid[4].b = (L_grid_meas.U_pos[4].b - L_grid_meas.U_neg[4].b) / (L_grid_meas.Iq_diff_b[4] * Conv.w_filter);
        L_grid_meas.L_grid[4].c = (L_grid_meas.U_pos[4].c - L_grid_meas.U_neg[4].c) / (L_grid_meas.Iq_diff_c[4] * Conv.w_filter);
        memcpy(L_grid_meas.L_grid_sorted, L_grid_meas.L_grid, sizeof(L_grid_meas.L_grid_sorted));
        qsort(L_grid_meas.L_grid_sorted, 15, sizeof(float), compare_float);
        L_grid_meas.L_grid_new = fabsf(L_grid_meas.L_grid_sorted[7]);
        Conv.compensation2 = 2.0f * 4000.0f * Saturation(L_grid_meas.L_grid_new, 50e-6, 800e-6) + 1.8f;

        qsort(L_grid_meas.Iq_diff_a, 5, sizeof(float), compare_float);
        qsort(L_grid_meas.Iq_diff_b, 5, sizeof(float), compare_float);
        qsort(L_grid_meas.Iq_diff_c, 5, sizeof(float), compare_float);
        L_grid_meas.CT_gain.a = 16.0f / L_grid_meas.Iq_diff_a[2];
        L_grid_meas.CT_gain.b = 16.0f / L_grid_meas.Iq_diff_b[2];
        L_grid_meas.CT_gain.c = 16.0f / L_grid_meas.Iq_diff_c[2];
        L_grid_meas.CT_gain_rounded.a = (float)__f32toi16r(L_grid_meas.CT_gain.a * 1.0f) * 1.0f;
        L_grid_meas.CT_gain_rounded.b = (float)__f32toi16r(L_grid_meas.CT_gain.b * 1.0f) * 1.0f;
        L_grid_meas.CT_gain_rounded.c = (float)__f32toi16r(L_grid_meas.CT_gain.c * 1.0f) * 1.0f;

        Uint16 save = 0;
        if(L_grid_meas.CT_gain_rounded.a < 0.0f) CT_char_vars.calibration.Meas_master_gain.I_grid.a *= -1.0f, save = 1;
        if(L_grid_meas.CT_gain_rounded.b < 0.0f) CT_char_vars.calibration.Meas_master_gain.I_grid.b *= -1.0f, save = 1;
        if(L_grid_meas.CT_gain_rounded.c < 0.0f) CT_char_vars.calibration.Meas_master_gain.I_grid.c *= -1.0f, save = 1;

        if(save)
        {
            memcpy(&SD_card.calibration.Meas_master_gain, &CT_char_vars.calibration.Meas_master_gain, sizeof(SD_card.calibration.Meas_master_gain));
            SD_card.save_calibration_data();
        }

        L_grid_meas.L_grid_previous[9] = L_grid_meas.L_grid_previous[8];
        L_grid_meas.L_grid_previous[8] = L_grid_meas.L_grid_previous[7];
        L_grid_meas.L_grid_previous[7] = L_grid_meas.L_grid_previous[6];
        L_grid_meas.L_grid_previous[6] = L_grid_meas.L_grid_previous[5];
        L_grid_meas.L_grid_previous[5] = L_grid_meas.L_grid_previous[4];
        L_grid_meas.L_grid_previous[4] = L_grid_meas.L_grid_previous[3];
        L_grid_meas.L_grid_previous[3] = L_grid_meas.L_grid_previous[2];
        L_grid_meas.L_grid_previous[2] = L_grid_meas.L_grid_previous[1];
        L_grid_meas.L_grid_previous[1] = L_grid_meas.L_grid_previous[0];
        L_grid_meas.L_grid_previous[0] = L_grid_meas.L_grid_new;
        L_grid_FLASH.save();

        Lgrid_meas_state++;
        break;
    }

    case 3:
    {
        if(elapsed_time > 75000000ULL)
        {
            Machine.state = state_operational;
            status_master.L_grid_measured = 1;
        }
        break;
    }
    }

    timer_update(&Timer_total, 1);

    if(!Machine.ONOFF) Machine.state = state_idle;
    if(alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]) Machine.state = state_cleanup;
}

void Machine_class::operational()
{
    static struct test_CT_struct
    {
        Uint16 state, state_last;
        float I_grid_val;
        float test_delay_timer_Kahan[4];
        float test_delay_timer[4];
        float test_delay_timer_compare;
        float test_request_integrator[7];
        float test_request_filtered[7];
        float test_request_period_counter;
        float test_request_period;
        struct abc_struct Q_grid_last;
        struct abc_struct Q_conv_step;
        struct abc_struct tested_current;
        struct abc_struct I_grid_filter_last;
        union
        {
            Uint16 all;
            struct
            {
                Uint16 I_grid_back_a : 1;
                Uint16 I_grid_back_b : 1;
                Uint16 I_grid_back_c : 1;
                Uint16 I_grid_under_val_a : 1;
                Uint16 I_grid_under_val_b : 1;
                Uint16 I_grid_under_val_c : 1;
                Uint16 In_limit : 1;
            }bit;
        }test_request_flags;
    }CT_test_online;

    Uint64 timer_new = ReadIpcTimer();
    Uint32 timer_new32 = timer_new;
    static Uint32 timer_last32;

    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;

        timer_last32 = timer_new32;

        memset(&CT_test_online, 0, sizeof(CT_test_online));
        CT_test_online.state_last = 1;
        CT_test_online.I_grid_val = 1.0f;
        CT_test_online.test_request_period = 2.0f;
        CT_test_online.test_delay_timer_compare = 900.0f;
        CT_test_online.test_delay_timer[0] =
        CT_test_online.test_delay_timer[1] =
        CT_test_online.test_delay_timer[2] =
        CT_test_online.test_delay_timer[3] = CT_test_online.test_delay_timer_compare;
    }

    switch (CT_test_online.state)
    {
    case 0:
    {
        if (CT_test_online.state != CT_test_online.state_last)
        {
            CT_test_online.state_last = CT_test_online.state;
            CT_test_online.test_request_flags.all = 0;
        }

        float time_delay = (float)(timer_new32 - timer_last32) * (1.0f/200000000.0f);
        timer_last32 = timer_new32;
        CT_test_online.test_request_period_counter += time_delay;

        float timer_last;
        float y;
        CT_test_online.test_delay_timer[0] = (timer_last = CT_test_online.test_delay_timer[0]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[0]);
        CT_test_online.test_delay_timer_Kahan[0] = (CT_test_online.test_delay_timer[0] - timer_last) - y;

        CT_test_online.test_delay_timer[1] = (timer_last = CT_test_online.test_delay_timer[1]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[1]);
        CT_test_online.test_delay_timer_Kahan[1] = (CT_test_online.test_delay_timer[1] - timer_last) - y;

        CT_test_online.test_delay_timer[2] = (timer_last = CT_test_online.test_delay_timer[2]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[2]);
        CT_test_online.test_delay_timer_Kahan[2] = (CT_test_online.test_delay_timer[2] - timer_last) - y;

        CT_test_online.test_delay_timer[3] = (timer_last = CT_test_online.test_delay_timer[3]) + (y = time_delay - CT_test_online.test_delay_timer_Kahan[3]);
        CT_test_online.test_delay_timer_Kahan[3] = (CT_test_online.test_delay_timer[3] - timer_last) - y;

        if (Grid_filter.I_grid_1h.a < CT_test_online.I_grid_val && CT_test_online.test_delay_timer[0] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[0] += time_delay;
        if (Grid_filter.I_grid_1h.b < CT_test_online.I_grid_val && CT_test_online.test_delay_timer[1] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[1] += time_delay;
        if (Grid_filter.I_grid_1h.c < CT_test_online.I_grid_val && CT_test_online.test_delay_timer[2] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[2] += time_delay;

        Uint16 in_limit = status_master.in_limit_Q && (Conv.version_Q_comp_local.a + Conv.version_Q_comp_local.b + Conv.version_Q_comp_local.c < 3.0f);
        in_limit = in_limit || status_master.in_limit_H || status_master.in_limit_P;
        Uint16 test_val =
                Grid_filter.I_grid_1h.a < CT_test_online.I_grid_val &&
                Grid_filter.I_grid_1h.b < CT_test_online.I_grid_val &&
                Grid_filter.I_grid_1h.c < CT_test_online.I_grid_val;

        if (test_val && in_limit && CT_test_online.test_delay_timer[3] > CT_test_online.test_delay_timer_compare)
            CT_test_online.test_request_integrator[3] += time_delay;

        if (Grid_filter.I_grid_1h.a > CT_test_online.I_grid_val && status_master.no_CT_connected_a)
            CT_test_online.test_request_integrator[4] += time_delay;
        if (Grid_filter.I_grid_1h.b > CT_test_online.I_grid_val && status_master.no_CT_connected_b)
            CT_test_online.test_request_integrator[5] += time_delay;
        if (Grid_filter.I_grid_1h.c > CT_test_online.I_grid_val && status_master.no_CT_connected_c)
            CT_test_online.test_request_integrator[6] += time_delay;

        if(CT_test_online.test_request_period_counter > CT_test_online.test_request_period)
        {
            CT_test_online.test_request_filtered[0] = CT_test_online.test_request_integrator[0] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[1] = CT_test_online.test_request_integrator[1] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[2] = CT_test_online.test_request_integrator[2] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[3] = CT_test_online.test_request_integrator[3] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[4] = CT_test_online.test_request_integrator[4] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[5] = CT_test_online.test_request_integrator[5] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_filtered[6] = CT_test_online.test_request_integrator[6] / CT_test_online.test_request_period_counter;
            CT_test_online.test_request_integrator[0] =
            CT_test_online.test_request_integrator[1] =
            CT_test_online.test_request_integrator[2] =
            CT_test_online.test_request_integrator[3] =
            CT_test_online.test_request_integrator[4] =
            CT_test_online.test_request_integrator[5] =
            CT_test_online.test_request_integrator[6] =
            CT_test_online.test_request_period_counter = 0.0f;
        }

        if (CT_test_online.test_request_filtered[4] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_back_a = 1;
        else if (CT_test_online.test_request_filtered[5] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_back_b = 1;
        else if (CT_test_online.test_request_filtered[6] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_back_c = 1;
        else if (CT_test_online.test_request_filtered[0] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_under_val_a = 1;
        else if (CT_test_online.test_request_filtered[1] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_under_val_b = 1;
        else if (CT_test_online.test_request_filtered[2] > 0.9f)
            CT_test_online.test_request_flags.bit.I_grid_under_val_c = 1;
        else if (CT_test_online.test_request_filtered[3] > 0.9f)
            CT_test_online.test_request_flags.bit.In_limit = 1;

        if(CT_test_online.test_request_flags.all)
            CT_test_online.state = 1;

        ////////////////////////////////////////////////////////////////////

        Conv.tangens_range_local[0].a = Saturation(control_master.tangens_range[0].a, -1.0f, 1.0f);
        Conv.tangens_range_local[0].b = Saturation(control_master.tangens_range[0].b, -1.0f, 1.0f);
        Conv.tangens_range_local[0].c = Saturation(control_master.tangens_range[0].c, -1.0f, 1.0f);
        Conv.tangens_range_local[1].a = Saturation(control_master.tangens_range[1].a, -1.0f, 1.0f);
        Conv.tangens_range_local[1].b = Saturation(control_master.tangens_range[1].b, -1.0f, 1.0f);
        Conv.tangens_range_local[1].c = Saturation(control_master.tangens_range[1].c, -1.0f, 1.0f);

        Conv.version_P_sym_local = control_master.flags.bit.version_P_sym;
        Conv.enable_Q_comp_local.a = control_master.flags.bit.enable_Q_comp_a;
        Conv.enable_Q_comp_local.b = control_master.flags.bit.enable_Q_comp_b;
        Conv.enable_Q_comp_local.c = control_master.flags.bit.enable_Q_comp_c;

        if(status_master.no_CT_connected_a)
        {
            Conv.Q_set_local.a = Saturation(control_master.Q_set.a + Grid.U_grid_1h.a * Grid.U_grid_1h.a * Conv.w_filter * (12.5e-6 + 4.4e-6), -10000.0f, 10000.0f);
            Conv.version_Q_comp_local.a = 1.0f;
        }
        else
        {
            Conv.Q_set_local.a = Saturation(control_master.Q_set.a, -10000.0f, 10000.0f);
            Conv.version_Q_comp_local.a = control_master.flags.bit.version_Q_comp_a;
        }

        if(status_master.no_CT_connected_b)
        {
            Conv.Q_set_local.b = Saturation(control_master.Q_set.b + Grid.U_grid_1h.b * Grid.U_grid_1h.b * Conv.w_filter * (12.5e-6 + 4.4e-6), -10000.0f, 10000.0f);
            Conv.version_Q_comp_local.b = 1.0f;
        }
        else
        {
            Conv.Q_set_local.b = Saturation(control_master.Q_set.b, -10000.0f, 10000.0f);
            Conv.version_Q_comp_local.b = control_master.flags.bit.version_Q_comp_b;
        }

        if(status_master.no_CT_connected_c)
        {
            Conv.Q_set_local.c = Saturation(control_master.Q_set.c + Grid.U_grid_1h.c * Grid.U_grid_1h.c * Conv.w_filter * (12.5e-6 + 4.4e-6), -10000.0f, 10000.0f);
            Conv.version_Q_comp_local.c = 1.0f;
        }
        else
        {
            Conv.Q_set_local.c = Saturation(control_master.Q_set.c, -10000.0f, 10000.0f);
            Conv.version_Q_comp_local.c = control_master.flags.bit.version_Q_comp_c;
        }

        if (status_master.no_CT_connected_a || status_master.no_CT_connected_b || status_master.no_CT_connected_c)
        {
            Conv.enable_P_sym_local = 0.0f;
            Conv.enable_H_comp_local = 0.0f;
        }
        else
        {
            Conv.enable_P_sym_local = control_master.flags.bit.enable_P_sym;
            Conv.enable_H_comp_local = control_master.flags.bit.enable_H_comp;
        }

        break;
    }
    case 1:
    {
        static Uint32 timer_last32;
        static float step;

        if (CT_test_online.state != CT_test_online.state_last)
        {
            CT_test_online.state_last = CT_test_online.state;

            timer_last32 = timer_new32;

            CT_test_online.I_grid_filter_last.a = Grid_filter.I_grid_1h.a;
            CT_test_online.I_grid_filter_last.b = Grid_filter.I_grid_1h.b;
            CT_test_online.I_grid_filter_last.c = Grid_filter.I_grid_1h.c;

            CT_test_online.Q_grid_last.a = Grid.Q_grid_1h.a;
            CT_test_online.Q_grid_last.b = Grid.Q_grid_1h.b;
            CT_test_online.Q_grid_last.c = Grid.Q_grid_1h.c;

            step = fminf(2.0f * CT_test_online.I_grid_val, Conv.I_lim);
            CT_test_online.Q_conv_step.a = Grid.U_grid_1h.a * step;
            CT_test_online.Q_conv_step.b = Grid.U_grid_1h.b * step;
            CT_test_online.Q_conv_step.c = Grid.U_grid_1h.c * step;
            if (Grid.Q_conv_1h.a < 0.0f) CT_test_online.Q_conv_step.a = -CT_test_online.Q_conv_step.a;
            if (Grid.Q_conv_1h.b < 0.0f) CT_test_online.Q_conv_step.b = -CT_test_online.Q_conv_step.b;
            if (Grid.Q_conv_1h.c < 0.0f) CT_test_online.Q_conv_step.c = -CT_test_online.Q_conv_step.c;

            Conv.Q_set_local.a = CT_test_online.Q_conv_step.a - Grid.Q_conv_1h.a;
            Conv.Q_set_local.b = CT_test_online.Q_conv_step.b - Grid.Q_conv_1h.b;
            Conv.Q_set_local.c = CT_test_online.Q_conv_step.c - Grid.Q_conv_1h.c;
            Conv.enable_Q_comp_local.a =
            Conv.enable_Q_comp_local.b =
            Conv.enable_Q_comp_local.c =
            Conv.version_Q_comp_local.a =
            Conv.version_Q_comp_local.b =
            Conv.version_Q_comp_local.c = 1.0f;
        }

        if (timer_new32 - timer_last32 > 30000000UL)
        {
            CT_test_online.tested_current.a = fabsf(Grid.Q_grid_1h.a - CT_test_online.Q_grid_last.a - CT_test_online.Q_conv_step.a) / Grid.U_grid_1h.a;
            CT_test_online.tested_current.b = fabsf(Grid.Q_grid_1h.b - CT_test_online.Q_grid_last.b - CT_test_online.Q_conv_step.b) / Grid.U_grid_1h.b;
            CT_test_online.tested_current.c = fabsf(Grid.Q_grid_1h.c - CT_test_online.Q_grid_last.c - CT_test_online.Q_conv_step.c) / Grid.U_grid_1h.c;

            Uint16 test_result[3];
            test_result[0] = CT_test_online.tested_current.a > step * 0.5f;
            test_result[1] = CT_test_online.tested_current.b > step * 0.5f;
            test_result[2] = CT_test_online.tested_current.c > step * 0.5f;

            CT_test_online.test_request_filtered[0] =
            CT_test_online.test_request_filtered[1] =
            CT_test_online.test_request_filtered[2] =
            CT_test_online.test_request_filtered[3] =
            CT_test_online.test_request_filtered[4] =
            CT_test_online.test_request_filtered[5] =
            CT_test_online.test_request_filtered[6] =
            CT_test_online.test_request_period_counter = 0.0f;

            if (CT_test_online.test_request_flags.bit.I_grid_back_a | CT_test_online.test_request_flags.bit.I_grid_back_b | CT_test_online.test_request_flags.bit.I_grid_back_c)
            {
                if(CT_test_online.I_grid_filter_last.a < CT_test_online.I_grid_val)
                {
                    status_master.no_CT_connected_a = test_result[0];
                    CT_test_online.test_delay_timer[0] = 0.0f;
                }
                else
                {
                    status_master.no_CT_connected_a = 0;
                    CT_test_online.test_delay_timer[0] = CT_test_online.test_delay_timer_compare;
                }

                if(CT_test_online.I_grid_filter_last.b < CT_test_online.I_grid_val)
                {
                    status_master.no_CT_connected_b = test_result[1];
                    CT_test_online.test_delay_timer[1] = 0.0f;
                }
                else
                {
                    status_master.no_CT_connected_b = 0;
                    CT_test_online.test_delay_timer[1] = CT_test_online.test_delay_timer_compare;
                }

                if(CT_test_online.I_grid_filter_last.c < CT_test_online.I_grid_val)
                {
                    status_master.no_CT_connected_c = test_result[2];
                    CT_test_online.test_delay_timer[2] = 0.0f;
                }
                else
                {
                    status_master.no_CT_connected_c = 0;
                    CT_test_online.test_delay_timer[2] = CT_test_online.test_delay_timer_compare;
                }

                CT_test_online.test_delay_timer[3] = CT_test_online.test_delay_timer_compare;
            }
            else if (CT_test_online.test_request_flags.bit.I_grid_under_val_a || CT_test_online.test_request_flags.bit.I_grid_under_val_b || CT_test_online.test_request_flags.bit.I_grid_under_val_c)
            {
                if(CT_test_online.I_grid_filter_last.a < CT_test_online.I_grid_val || CT_test_online.test_request_flags.bit.I_grid_under_val_a)
                {
                    status_master.no_CT_connected_a = test_result[0];
                    CT_test_online.test_delay_timer[0] = 0.0f;
                }

                if(CT_test_online.I_grid_filter_last.b < CT_test_online.I_grid_val || CT_test_online.test_request_flags.bit.I_grid_under_val_b)
                {
                    status_master.no_CT_connected_b = test_result[1];
                    CT_test_online.test_delay_timer[1] = 0.0f;
                }

                if(CT_test_online.I_grid_filter_last.c < CT_test_online.I_grid_val || CT_test_online.test_request_flags.bit.I_grid_under_val_c)
                {
                    status_master.no_CT_connected_c = test_result[2];
                    CT_test_online.test_delay_timer[2] = 0.0f;
                }
            }
            else if (CT_test_online.test_request_flags.bit.In_limit)
            {
                if(test_result[0])
                    status_master.no_CT_connected_a = 1;
                if(test_result[1])
                    status_master.no_CT_connected_b = 1;
                if(test_result[2])
                    status_master.no_CT_connected_c = 1;

                if(test_result[0] + test_result[1] + test_result[2])
                    CT_test_online.test_delay_timer[3] = CT_test_online.test_delay_timer_compare;
                else
                    CT_test_online.test_delay_timer[3] = 0.0f;
            }

            CT_test_online.state = 0;
        }
        break;
    }
    }

    timer_update(&Timer_total, 1);

    if(!Machine.ONOFF) Machine.state = state_idle;
    if(alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]) Machine.state = state_cleanup;
}

void Machine_class::cleanup()
{
    if(Machine.state_last != Machine.state)
    {
        Machine.state_last = Machine.state;
        Conv.enable = 0;
        Scope_trigger_unc();
        Machine.recent_error = 1;
        if(mosfet_ctrl_app.getState() == MosfetCtrlApp::state_error){
            //gdy wystapil jakis blad to proba zrestartowania maszyn stanowych
            mosfet_ctrl_app.process_event(MosfetCtrlApp::event_restart, NULL);
        }
        mosfet_ctrl_app.process_event( MosfetCtrlApp::event_get_status );
    }

    if(Scope.finished_sorting && !status_master.Scope_snapshot_pending && !control_master.triggers.bit.Scope_snapshot
            && mosfet_ctrl_app.finished())
    {
        SD_card.save_state();
        if(Machine.error_retry == 4) control_master.triggers.bit.CPU_reset = 1;
        Machine.state = state_idle;
    }
}
