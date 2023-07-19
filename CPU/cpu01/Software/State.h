//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_STATE_H_
#define SOFTWARE_STATE_H_

#include "stdafx.h"

#define WBUF_SIZE 100
#define CT_CHARACTERISTIC_POINTS 60

struct CT_characteristic_struct
{
    Uint16 available;
    Uint16 number_of_elements;
    float set_current[CT_CHARACTERISTIC_POINTS];
    float CT_ratio_a[CT_CHARACTERISTIC_POINTS];
    float CT_ratio_b[CT_CHARACTERISTIC_POINTS];
    float CT_ratio_c[CT_CHARACTERISTIC_POINTS];
    float phase_a[CT_CHARACTERISTIC_POINTS];
    float phase_b[CT_CHARACTERISTIC_POINTS];
    float phase_c[CT_CHARACTERISTIC_POINTS];
};

struct harmonics_struct
{
    Uint16 available;
    float on_off_odd_a[25];
    float on_off_odd_b[25];
    float on_off_odd_c[25];
    float on_off_even_a[25];
    float on_off_even_b[25];
    float on_off_even_c[25];
};

struct calibration_struct
{
    Uint16 available;
    struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain;
    struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset;
};

struct settings_struct
{
    Uint16 available;
    struct CONTROL_ACDC control;
    float Baudrate;
    float modbus_ext_server_id;
    float wifi_on;
    float C_dc;
    float L_conv;
    float I_lim;
};

struct meter_struct
{
    Uint16 available;
    struct Energy_meter_upper_struct Energy_meter;
};

struct L_grid_meas_struct
{
    struct abc_struct Iq_pos[5], Iq_neg[5];
    struct abc_struct U_pos[5], U_neg[5];
    float Iq_diff_a[5];
    float Iq_diff_b[5];
    float Iq_diff_c[5];
    struct abc_struct L_grid[5];
    float L_grid_sorted[15];
    float L_grid_new;
    float L_grid_previous[10];
    struct abc_struct CT_gain;
    struct abc_struct CT_gain_rounded;
};

struct Settings_struct
{
    struct CT_characteristic_struct CT_char;
    struct harmonics_struct harmonics;
    struct calibration_struct calibration;
    struct settings_struct settings;
};

struct timer_struct
{
    Uint32 seconds : 6;
    Uint32 minutes : 6;
    Uint32 hours : 5;
    Uint32 days : 13;
    Uint32 magic : 2;
    Uint32 counter : 32;
    Uint32 integrator : 32;
};

extern struct timer_struct Timer_total;

void timer_update(struct timer_struct *Timer, Uint16 enable_counting);

class Machine_master_class
{
    public:

    static void Main();

    enum state_enum
    {
        state_init,
        state_idle,
        state_start,
        state_CT_test,
        state_Lgrid_meas,
        state_operational,
        state_max,
        __dummybig_state = 300000
    };

    enum state_enum state, state_last;

    Machine_master_class()
    {
        for(Uint16 i = 0; i < state_max; i++)
            Machine_master_class::state_pointers[i] = NULL;

        state_pointers[state_init] = &Machine_master_class::init;
        state_pointers[state_idle] = &Machine_master_class::idle;
        state_pointers[state_start] = &Machine_master_class::start;
        state_pointers[state_CT_test] = &Machine_master_class::CT_test;
        state_pointers[state_Lgrid_meas] = &Machine_master_class::Lgrid_meas;
        state_pointers[state_operational] = &Machine_master_class::operational;

        for(Uint16 i = 0; i < state_max; i++)
            if(Machine_master_class::state_pointers[i] == NULL) ESTOP0;
    }

    private:
    static void init();
    static void idle();
    static void start();
    static void CT_test();
    static void Lgrid_meas();
    static void operational();

    static void (*state_pointers[state_max])();
};

class Machine_slave_class
{
    public:

    Uint16 harmonics_odd, harmonics_odd_last;
    Uint16 harmonics_even, harmonics_even_last;

    float switch_timer;
    Uint16 ONOFF_FLASH;
    Uint16 ONOFF_temp;
    Uint16 ONOFF_switch, ONOFF_switch_last;
    Uint16 ONOFF, ONOFF_last;
    Uint16 save_to_RTC;
    Uint16 error_retry;
    Uint16 recent_error;
    Uint16 padding;

    static void Main();
    static void Background();

    enum state_enum
    {
        state_init,
        state_idle,
        state_calibrate_offsets,
        state_calibrate_curent_gain,
        state_calibrate_AC_voltage_gain,
        state_calibrate_DC_voltage_gain,
        state_start,
        state_operational,
        state_cleanup,
        state_max,
        __dummybig_state = 300000
    };

    enum state_enum state, state_last;

    Machine_slave_class()
    {
        for(Uint16 i = 0; i < state_max; i++)
            Machine_slave_class::state_pointers[i] = NULL;

        state_pointers[state_init] = &Machine_slave_class::init;
        state_pointers[state_idle] = &Machine_slave_class::idle;
        state_pointers[state_calibrate_offsets] = &Machine_slave_class::calibrate_offsets;
        state_pointers[state_calibrate_curent_gain] = &Machine_slave_class::calibrate_curent_gain;
        state_pointers[state_calibrate_AC_voltage_gain] = &Machine_slave_class::calibrate_AC_voltage_gain;
        state_pointers[state_calibrate_DC_voltage_gain] = &Machine_slave_class::calibrate_DC_voltage_gain;
        state_pointers[state_start] = &Machine_slave_class::start;
        state_pointers[state_operational] = &Machine_slave_class::operational;
        state_pointers[state_cleanup] = &Machine_slave_class::cleanup;

        for(Uint16 i = 0; i < state_max; i++)
            if(Machine_slave_class::state_pointers[i] == NULL) ESTOP0;
    }

    private:
    static void init();
    static void idle();
    static void calibrate_offsets();
    static void calibrate_curent_gain();
    static void calibrate_AC_voltage_gain();
    static void calibrate_DC_voltage_gain();
    static void start();
    static void operational();
    static void cleanup();

    static void (*state_pointers[state_max])();
};

struct CT_calc_struct
{
    struct calibration_struct calibration;
    struct CT_characteristic_struct CT_char;
    Uint16 CT_char_index[3];
    float CT_phase[3];
};

#include "time_bcd_struct.h"

extern struct L_grid_meas_struct L_grid_meas;
extern struct CT_calc_struct CT_char_vars;
extern class Machine_slave_class Machine_slave;
extern class Machine_master_class Machine_master;
extern struct time_BCD_struct RTC_current_time;
extern struct time_BCD_struct RTC_new_time;

void CT_char_calc();
void convert_harmonics_to_bits();
void convert_harmonics_to_floats();
void ONOFF_switch_interrupt();

#endif /* SOFTWARE_STATE_H_ */
