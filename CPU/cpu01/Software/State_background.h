//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef STATE_BACKGROUND_H_
#define STATE_BACKGROUND_H_

#include "stdafx.h"
#include "time_bcd_struct.h"
#include "hw_info.h"

class Background_class
{
    public:

    Uint16 harmonics_odd, harmonics_odd_last;
    Uint16 harmonics_even, harmonics_even_last;

    struct hw_info_struct hw_info;

    static void init();
    static void Main();

    private:
};

extern class Background_class Background;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void convert_harmonics_to_bits();
void convert_harmonics_to_floats();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    float C_conv;
    float I_lim;
    float number_of_slaves;
    float no_neutral;
};

struct Settings_struct
{
    struct CT_characteristic_struct CT_char;
    struct harmonics_struct harmonics;
    struct calibration_struct calibration;
    struct settings_struct settings;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct meter_struct
{
    Uint16 available;
    struct Energy_meter_upper_struct Energy_meter;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CT_calc_struct
{
    struct calibration_struct calibration;
    struct CT_characteristic_struct CT_char;
    Uint16 CT_char_index[3];
    float CT_phase[3];
};

extern struct CT_calc_struct CT_char_vars;

void CT_char_calc();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct time_BCD_struct RTC_current_time;
extern struct time_BCD_struct RTC_new_time;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ONOFF_struct
{
    float switch_timer;
    Uint16 ONOFF_FLASH;
    Uint16 ONOFF_temp;
    Uint16 ONOFF_switch, ONOFF_switch_last;
    Uint16 ONOFF, ONOFF_last;
};

extern struct ONOFF_struct ONOFF;
extern const class FLASH_class switch_FLASH;

void ONOFF_switch_interrupt();

#endif /* STATE_BACKGROUND_H_ */
