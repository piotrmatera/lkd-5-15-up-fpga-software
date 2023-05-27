/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <math.h>
#include "stdafx.h"
#include "HWIs.h"
#include "Scope.h"
#include "State.h"
#include "SD_card.h"
#include "Modbus_devices.h"
#include "Modbus_Converter_memory.h"
#include "version.h"

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void SD_AVG_NT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

    GPIO_SET(TRIGGER_CM);

    register Uint32 *src;
    register Uint32 *dest;

    src = (Uint32 *)&EMIF_mem.read.SD_avg;
    dest = (Uint32 *)&EMIF_CLA;

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    Cla1ForceTask1();

    register float modifier1 = Conv.div_range_modifier_Kalman_values;
    Conv.Kalman_U_grid.a = (float)EMIF_mem.read.Kalman[0].series[0].estimate * modifier1;
    Conv.Kalman_U_grid.b = (float)EMIF_mem.read.Kalman[0].series[1].estimate * modifier1;
    Conv.Kalman_U_grid.c = (float)EMIF_mem.read.Kalman[0].series[2].estimate * modifier1;
    Conv.U_dc_kalman = (float)EMIF_mem.read.Kalman_DC.series[0].estimate * modifier1;

//    Grid.parameters.parameters.THD_U_grid.a = Kalman_U_grid[0].THD_total;
//    Grid.parameters.parameters.THD_U_grid.b = Kalman_U_grid[1].THD_total;
//    Grid.parameters.parameters.THD_U_grid.c = Kalman_U_grid[2].THD_total;
//    Grid.parameters.parameters.THD_I_grid.a = Kalman_I_grid[0].THD_total;
//    Grid.parameters.parameters.THD_I_grid.b = Kalman_I_grid[1].THD_total;
//    Grid.parameters.parameters.THD_I_grid.c = Kalman_I_grid[2].THD_total;

    Timer_PWM.CPU_COPY1 = TIMESTAMP_PWM;

    Energy_meter_CPUasm();

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    register float modifier2 = Conv.range_modifier_Kalman_values;
    EMIF_mem.write.Kalman[0].series[0].input = Meas_master.U_grid.a * modifier2;
    EMIF_mem.write.Kalman[0].series[1].input = Meas_master.U_grid.b * modifier2;
    EMIF_mem.write.Kalman[0].series[2].input = Meas_master.U_grid.c * modifier2;
    EMIF_mem.write.Kalman[1].series[0].input = Meas_master.I_grid.a * modifier2;
    EMIF_mem.write.Kalman[1].series[1].input = Meas_master.I_grid.b * modifier2;
    EMIF_mem.write.Kalman[1].series[2].input = Meas_master.I_grid.c * modifier2;
    EMIF_mem.write.Kalman_DC.series[0].input = Meas_master.U_dc * modifier2;
    EMIF_mem.write.DSP_start = 0b1110;

    Timer_PWM.CPU_COPY2 = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(!status_master.Init_done) Conv.enable = 0;
    else
    {
        ONOFF_switch_interrupt();

        if(EPwm4Regs.TZFLG.bit.OST) alarm_master.bit.TZ_CPU1 = 1;
        if(EPwm4Regs.TZOSTFLG.bit.OST5) alarm_master.bit.TZ_CLOCKFAIL_CPU1 = 1;
        if(EPwm4Regs.TZOSTFLG.bit.OST6) alarm_master.bit.TZ_EMUSTOP_CPU1 = 1;

        alarm_master.bit.FPGA_errors.all = EMIF_mem.read.FPGA_flags.all;

        if(Conv.enable)
        {
            if(!CPU2toCPU1.PLL_RDY) alarm_master.bit.PLL_UNSYNC = 1;

            if(Grid.parameters.U_grid.a < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_a_L = 1;
            if(Grid.parameters.U_grid.b < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_b_L = 1;
            if(Grid.parameters.U_grid.c < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_c_L = 1;
        }

        status_master.PLL_sync = CPU2toCPU1.PLL_RDY;
        float compare_U_rms = Meas_alarm_L.U_grid_rms + 10.0f;
        if(Grid_filter.parameters.U_grid_1h.a > compare_U_rms && Grid_filter.parameters.U_grid_1h.b > compare_U_rms && Grid_filter.parameters.U_grid_1h.c > compare_U_rms)
            status_master.Grid_present = 1;
        else status_master.Grid_present = 0;

        if(Meas_master.Supply_24V < 22.0) alarm_master.bit.FLT_SUPPLY_MASTER = 1;

        if(Meas_master.U_dc < Meas_alarm_L.U_dc) alarm_master.bit.U_dc_L = 1;
        if(Meas_master.U_dc > Meas_alarm_H.U_dc) alarm_master.bit.U_dc_H = 1;
        if(fabsf(Meas_master.U_dc - 2.0f * Meas_master.U_dc_n) > Meas_alarm_H.U_dc_balance) alarm_master.bit.U_dc_balance = 1;

        if(Grid.parameters.I_conv.a > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_a = 1;
        if(Grid.parameters.I_conv.b > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_b = 1;
        if(Grid.parameters.I_conv.c > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_c = 1;
        if(Grid.parameters.I_conv.n > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_n = 1;

        static volatile float Temp_max = 0;
        Temp_max = fmaxf(Meas_master.Temperature1, fmaxf(Meas_master.Temperature2, Meas_master.Temperature3));
        if(Temp_max > Meas_alarm_H.Temp) alarm_master.bit.Temperature_H = 1;
        if(Temp_max < Meas_alarm_L.Temp) alarm_master.bit.Temperature_L = 1;

        alarm_master.all[0] |= CPU2toCPU1.alarm_master.all[0];
        alarm_master.all[1] |= CPU2toCPU1.alarm_master.all[1];
        alarm_master.all[2] |= CPU2toCPU1.alarm_master.all[2];

        if((alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]) && !(alarm_master_snapshot.all[0] | alarm_master_snapshot.all[1] | alarm_master_snapshot.all[2]))
        {
            EALLOW;
            EPwm4Regs.TZFRC.bit.OST = 1;
            EDIS;
            alarm_master_snapshot.all[0] = alarm_master.all[0];
            alarm_master_snapshot.all[1] = alarm_master.all[1];
            alarm_master_snapshot.all[2] = alarm_master.all[2];
        }
    }

    Timer_PWM.CPU_ERROR = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        static Uint16 first = 0;

        static volatile Uint16 decimation_ratio = 1;
        static Uint16 decimation = 0;

        static float trigger_temp;
        trigger_temp = Machine.state == Machine_class::state_Lgrid_meas;
        static float* volatile trigger_pointer = &trigger_temp;
        static volatile float trigger_val = (float)Machine_class::state_Lgrid_meas;
        static volatile float edge = 1;
        static volatile float trigger_last;

        if(!first)
        {
            first = 1;

            Scope.acquire_before_trigger = SCOPE_BUFFER / 2;

            Scope.data_in[0] = &Meas_master.U_grid.a;
            Scope.data_in[1] = &Meas_master.U_grid.b;
            Scope.data_in[2] = &Meas_master.U_grid.c;
            Scope.data_in[3] = &Meas_master.I_grid.a;
            Scope.data_in[4] = &Meas_master.I_grid.b;
            Scope.data_in[5] = &Meas_master.I_grid.c;
            Scope.data_in[6] = &Meas_master.U_dc;
            Scope.data_in[7] = &Meas_master.U_dc_n;
            Scope.data_in[8] = &Meas_master.I_conv.a;
            Scope.data_in[9] = &Meas_master.I_conv.b;
            Scope.data_in[10] = &Meas_master.I_conv.c;
            Scope.data_in[11] = &Meas_master.I_conv.n;
            Scope.acquire_counter = -1;

            trigger_last = *trigger_pointer;
        }

        if (++decimation >= decimation_ratio)
        {
            if(status_master.Scope_snapshot_pending)
            {
                if(SD_card.Scope_snapshot_state == 1) Scope_trigger(Kalman_U_grid[0].states[2], &SD_card.Scope_input_last, 0.0f, 1);
            }
            else
            {
                if(alarm_master.all[0] | alarm_master.all[1] | alarm_master.all[2]) Scope_trigger_unc();
            }

            Scope_task();
            decimation = 0;
        }

    }

    Timer_PWM.CPU_SCOPE = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Modbus_slave_LCD.RTU->interrupt_task();
    Modbus_slave_EXT.RTU->interrupt_task();

    Timer_PWM.CPU_COMM = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        Therm.Divider_supply += 0.001*( ((float)AdcdResultRegs.ADCRESULT0 + (float)AdcdResultRegs.ADCRESULT1)*(2e3/1e3)*(3.3/4096.0/2.0) - Therm.Divider_supply);
        Meas_master.Supply_24V = ((float)AdcdResultRegs.ADCRESULT8 + (float)AdcdResultRegs.ADCRESULT9)*(11e3/1e3)*(3.3/4096.0/2.0);

        static float index;
        register float Thermistor;
        register Uint16 *adcresult = (Uint16 *)&AdcdResultRegs.ADCRESULT2 + (Uint16)(0x2*index);
        Thermistor = ( (float)*adcresult + (float)*(adcresult + 1) )*(3.3/256.0/16.0/2.0);
        Thermistor = (Therm.R_divider * Thermistor) / (3.3 - Thermistor);
        Thermistor = Therm.B/logf(Thermistor * Therm.DIV_Rinf) - Therm.T_0;
        register float *Temperature = &Meas_master.Temperature1 + (Uint16)index;
        *Temperature += 0.02*(Thermistor - *Temperature);
        if(++index >= 3.0f) index = 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static volatile union double_pulse_union
    {
       Uint32 u32;
       Uint16 u16[2];
    }double_pulse = {.u16 = {620, 15}};
    EMIF_mem.write.double_pulse = double_pulse.u32;

    static volatile float counter = 0;
    if(Machine.ONOFF != Machine.ONOFF_last)
    {
        counter = 0;
        GPIO_SET(PWM_EN_CM);
    }
    counter += Conv.Ts;
    if(counter >= 1.0f)
    {
       GPIO_CLEAR(PWM_EN_CM);
    }

//    GPIO_SET(RST_CM);
//    EMIF_mem.write.PWM_control = 0xAA;

    GPIO_CLEAR(TRIGGER_CM);

//    static Uint64 benchmark_timer_HWI;
//    static volatile float benchmark_HWI;
//    benchmark_HWI = (float)(ReadIpcTimer() - benchmark_timer_HWI)*(1.0f/200000000.0f);
//    benchmark_timer_HWI = ReadIpcTimer();

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    Timer_PWM.CPU_END = TIMESTAMP_PWM;
}
