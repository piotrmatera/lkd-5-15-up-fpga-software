/*
 * State_ramfunc.cpp
 *
 *  Created on: 6 wrz 2023
 *      Author: Piotr
 */

#include "stdafx.h"

#include <math.h>
#include <stdlib.h>     /* qsort */
#include <string.h>
#include "State.h"



#pragma CODE_SECTION(".TI.ramfunc_unsecure");
void ONOFF_switch_interrupt()
{
    float switch_timer_temp = Machine.switch_timer;
    if(GPIO_READ(ON_OFF_CM)) Machine.switch_timer = 0.0f;
    else Machine.switch_timer += Conv.Ts;

    Machine.ONOFF_switch_last = Machine.ONOFF_switch;
    if(Machine.switch_timer > 0.05f) Machine.ONOFF_switch = 1;
    else Machine.ONOFF_switch = 0;

    if( Machine.switch_timer >= 10.0f && switch_timer_temp < 10.0f ){
        status_master.wifi_on ^= 1;
        GPIO_WRITE( LED5_CM, status_master.wifi_on );
        control_master.triggers.bit.SD_save_settings = 1;
    }

    if(Machine.ONOFF == Machine.ONOFF_temp)
    {
        if(switch_timer_temp > 30.0f && switch_timer_temp < 1e6)
        {
            control_master.triggers.bit.CPU_reset = 1;
        }
        else if(switch_timer_temp > 2.0f)
        {
            Machine.recent_error = 0;
        }
        else if(Machine.ONOFF_switch_last && !Machine.ONOFF_switch)
        {
            Machine.ONOFF_temp = Machine.ONOFF ^ 1;
            if(Machine.ONOFF_temp) Machine.recent_error = 0;
        }
    }
}
