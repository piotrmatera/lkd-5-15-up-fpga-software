//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_STATE_MASTER_H_
#define SOFTWARE_STATE_MASTER_H_

#include "stdafx.h"

class Machine_master_class
{
    public:

    static void Main();

    enum state_enum
    {
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

        state_pointers[state_idle] = &Machine_master_class::idle;
        state_pointers[state_start] = &Machine_master_class::start;
        state_pointers[state_CT_test] = &Machine_master_class::CT_test;
        state_pointers[state_Lgrid_meas] = &Machine_master_class::Lgrid_meas;
        state_pointers[state_operational] = &Machine_master_class::operational;

        for(Uint16 i = 0; i < state_max; i++)
            if(Machine_master_class::state_pointers[i] == NULL) ESTOP0;
    }

    private:
    static void idle();
    static void start();
    static void CT_test();
    static void Lgrid_meas();
    static void operational();

    static void (*state_pointers[state_max])();
};

extern class Machine_master_class Machine_master;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

extern const class FLASH_class L_grid_FLASH;
extern struct L_grid_meas_struct L_grid_meas;

#endif /* SOFTWARE_STATE_MASTER_H_ */
