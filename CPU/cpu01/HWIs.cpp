/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <math.h>
#include "stdafx.h"
#include "HWIs.h"
#include "State.h"
#include "SD_card.h"
#include "Modbus_devices.h"
#include "Modbus_Converter_memory.h"
#include "version.h"

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void SD_AVG_NT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

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

    Energy_meter_CPUasm();

    Timer_PWM.CPU_COPY1 = TIMESTAMP_PWM;
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

        if(Meas_master.U_dc_avg < Meas_alarm_L.U_dc) alarm_master.bit.U_dc_L = 1;
        if(Meas_master.U_dc_avg > Meas_alarm_H.U_dc) alarm_master.bit.U_dc_H = 1;
        if(fabsf(Meas_master.U_dc_avg - 2.0f * Meas_master.U_dc_n_avg) > Meas_alarm_H.U_dc_balance) alarm_master.bit.U_dc_balance = 1;

        if(Grid.parameters.I_conv.a > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_a = 1;
        if(Grid.parameters.I_conv.b > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_b = 1;
        if(Grid.parameters.I_conv.c > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_c = 1;
        if(Grid.parameters.I_conv.n > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_n = 1;

//        if(Conv.enable)
//        {
//            if(fabs(Meas_master.U_grid.a) > CPU1toCPU2.Meas_alarm_H.U_grid_abs) alarm_master.bit.U_grid_abs_a_H = 1;
//            if(fabs(Meas_master.U_grid.b) > CPU1toCPU2.Meas_alarm_H.U_grid_abs) alarm_master.bit.U_grid_abs_b_H = 1;
//            if(fabs(Meas_master.U_grid.c) > CPU1toCPU2.Meas_alarm_H.U_grid_abs) alarm_master.bit.U_grid_abs_c_H = 1;
//        }
//
//        if(Meas_master.I_conv.a < CPU1toCPU2.Meas_alarm_L.I_conv) alarm_master.bit.I_conv_a_L = 1;
//        if(Meas_master.I_conv.a > CPU1toCPU2.Meas_alarm_H.I_conv) alarm_master.bit.I_conv_a_H = 1;
//        if(Meas_master.I_conv.b < CPU1toCPU2.Meas_alarm_L.I_conv) alarm_master.bit.I_conv_b_L = 1;
//        if(Meas_master.I_conv.b > CPU1toCPU2.Meas_alarm_H.I_conv) alarm_master.bit.I_conv_b_H = 1;
//        if(Meas_master.I_conv.c < CPU1toCPU2.Meas_alarm_L.I_conv) alarm_master.bit.I_conv_c_L = 1;
//        if(Meas_master.I_conv.c > CPU1toCPU2.Meas_alarm_H.I_conv) alarm_master.bit.I_conv_c_H = 1;
//        if(Meas_master.I_conv.n < CPU1toCPU2.Meas_alarm_L.I_conv) alarm_master.bit.I_conv_n_L = 1;
//        if(Meas_master.I_conv.n > CPU1toCPU2.Meas_alarm_H.I_conv) alarm_master.bit.I_conv_n_H = 1;
//
        static volatile float Temp_max = 0;
        Temp_max = fmaxf(Meas_master.Temperature1, fmaxf(Meas_master.Temperature2, Meas_master.Temperature3));
        if(Temp_max > Meas_alarm_H.Temp) alarm_master.bit.Temperature_H = 1;
        if(Temp_max < Meas_alarm_L.Temp) alarm_master.bit.Temperature_L = 1;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Timer_PWM.CPU_ERROR = TIMESTAMP_PWM;//3us

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

    Timer_PWM.CPU_SCOPE = TIMESTAMP_PWM;//4us

//    Grid.parameters.parameters.THD_U_grid.a = Kalman_U_grid[0].THD_total;
//    Grid.parameters.parameters.THD_U_grid.b = Kalman_U_grid[1].THD_total;
//    Grid.parameters.parameters.THD_U_grid.c = Kalman_U_grid[2].THD_total;
//    Grid.parameters.parameters.THD_I_grid.a = Kalman_I_grid[0].THD_total;
//    Grid.parameters.parameters.THD_I_grid.b = Kalman_I_grid[1].THD_total;
//    Grid.parameters.parameters.THD_I_grid.c = Kalman_I_grid[2].THD_total;

    GPIO_SET(TRIGGER_CM);

    Timer_PWM.CPU_TX_MSG2 = TIMESTAMP_PWM;
//
//    Modbus_slave_LCD.RTU->interrupt_task();
//    Modbus_slave_LCD_OLD.RTU->interrupt_task();
//    Modbus_slave_EXT.RTU->interrupt_task();

    Timer_PWM.CPU_COMM = TIMESTAMP_PWM;//1us

    static volatile Uint32 Kalman_WIP;
    Kalman_WIP = EMIF_mem.read.flags.Kalman1_WIP;

    static volatile struct
    {
        struct trigonometric_struct harmonic[FPGA_KALMAN_STATES];
        float estimate;
        float error;
    }Kalman_mem;
    int32 *pointer_src = (int32 *)&EMIF_mem.read.Kalman[0];
    float *pointer_dst = (float *)&Kalman_mem;
    for(int i=0; i<sizeof(Kalman_mem)>>1; i++)
    {
        *pointer_dst++ = (float)*pointer_src++ / (float)(1UL<<31);
    }

    static float angle = 0.0f;
    angle += Conv.Ts * MATH_2PI * 50.0f;
    angle -= (float)((int32)(angle * MATH_1_PI)) * MATH_2PI;
    static volatile int32 zmienna_int;
    static volatile float zmienna;
    zmienna = 0.02f * (0.0f + sinf(angle));
    zmienna_int = zmienna * (float)(1UL<<31);
//    zmienna = 0.002f;
    EMIF_mem.write.Kalman[0].series[0].input = zmienna_int;
    EMIF_mem.write.DSP_start = 4UL;

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

    static Uint64 benchmark_timer_HWI;
    static volatile float benchmark_HWI;
    benchmark_HWI = (float)(ReadIpcTimer() - benchmark_timer_HWI)*(1.0f/200000000.0f);
    benchmark_timer_HWI = ReadIpcTimer();

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    Timer_PWM.CPU_END = TIMESTAMP_PWM;
}
