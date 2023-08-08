//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_STATE_SLAVE_H_
#define SOFTWARE_STATE_SLAVE_H_

#include "stdafx.h"

class Machine_slave_class
{
    public:

    Uint16 error_retry;
    Uint16 recent_error;

    static void Main();

    enum state_enum
    {
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

extern class Machine_slave_class Machine_slave;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern class FLASH_class error_retry_FLASH;

#endif /* SOFTWARE_STATE_SLAVE_H_ */
