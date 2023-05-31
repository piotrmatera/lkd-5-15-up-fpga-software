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
    Conv.U_dc_kalman = (float)EMIF_mem.read.Kalman_DC[0].harmonic[0].x1 * modifier1;

    Conv.w_filter = CPU2toCPU1.w_filter;
    Conv.f_filter = CPU2toCPU1.f_filter;
    Conv.sign = CPU2toCPU1.sign;
    Conv.PLL_RDY = CPU2toCPU1.PLL_RDY;
    Conv.sag = CPU2toCPU1.sag;

    CPU1toCPU2.CLA1toCLA2.enable = Conv.RDY;
    CPU1toCPU2.CLA1toCLA2.L_conv = Conv.L_conv;
    CPU1toCPU2.CLA1toCLA2.Kp_I = Conv.Kp_I;
    CPU1toCPU2.CLA1toCLA2.id_ref.a = Conv.id_lim.a;
    CPU1toCPU2.CLA1toCLA2.id_ref.b = Conv.id_lim.b;
    CPU1toCPU2.CLA1toCLA2.id_ref.c = Conv.id_lim.c;
    CPU1toCPU2.CLA1toCLA2.iq_ref.a = Conv.iq_lim.a;
    CPU1toCPU2.CLA1toCLA2.iq_ref.b = Conv.iq_lim.b;
    CPU1toCPU2.CLA1toCLA2.iq_ref.c = Conv.iq_lim.c;

    Timer_PWM.CPU_COPY1 = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    register float modifier2 = Conv.range_modifier_Kalman_values;
    EMIF_mem.write.Kalman[0].input[0] = Meas_master.U_grid.a * modifier2;
    EMIF_mem.write.Kalman[0].input[1] = Meas_master.U_grid.b * modifier2;
    EMIF_mem.write.Kalman[0].input[2] = Meas_master.U_grid.c * modifier2;
    EMIF_mem.write.Kalman[1].input[0] = Meas_master.I_grid.a * modifier2;
    EMIF_mem.write.Kalman[1].input[1] = Meas_master.I_grid.b * modifier2;
    EMIF_mem.write.Kalman[1].input[2] = Meas_master.I_grid.c * modifier2;
    EMIF_mem.write.Kalman_DC.input[0] = Meas_master.U_dc * modifier2;
    EMIF_mem.write.DSP_start = 0b11100;

    EMIF_mem.write.Resonant[0].series[0].harmonics =
    EMIF_mem.write.Resonant[0].series[1].harmonics =
    EMIF_mem.write.Resonant[0].series[2].harmonics = Conv.resonant_odd_number;
    EMIF_mem.write.Resonant[1].series[0].harmonics =
    EMIF_mem.write.Resonant[1].series[1].harmonics =
    EMIF_mem.write.Resonant[1].series[2].harmonics = Conv.resonant_even_number;

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
            if(!Conv.PLL_RDY) alarm_master.bit.PLL_UNSYNC = 1;

            if(Grid.U_grid.a < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_a_L = 1;
            if(Grid.U_grid.b < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_b_L = 1;
            if(Grid.U_grid.c < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_c_L = 1;
        }

        status_master.PLL_sync = Conv.PLL_RDY;
        float compare_U_rms = Meas_alarm_L.U_grid_rms + 10.0f;
        if(Grid_filter.U_grid_1h.a > compare_U_rms && Grid_filter.U_grid_1h.b > compare_U_rms && Grid_filter.U_grid_1h.c > compare_U_rms)
            status_master.Grid_present = 1;
        else status_master.Grid_present = 0;

        if(Meas_master.Supply_24V < 22.0) alarm_master.bit.FLT_SUPPLY_MASTER = 1;

        if(Meas_master.U_dc < Meas_alarm_L.U_dc) alarm_master.bit.U_dc_L = 1;
        if(Meas_master.U_dc > Meas_alarm_H.U_dc) alarm_master.bit.U_dc_H = 1;
        if(fabsf(Meas_master.U_dc - 2.0f * Meas_master.U_dc_n) > Meas_alarm_H.U_dc_balance) alarm_master.bit.U_dc_balance = 1;

        if(Grid.I_conv.a > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_a = 1;
        if(Grid.I_conv.b > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_b = 1;
        if(Grid.I_conv.c > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_c = 1;
        if(Grid.I_conv.n > Meas_alarm_H.I_conv_rms) alarm_master.bit.I_conv_rms_n = 1;

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

        static volatile float duty_temp[4];
        duty_temp[0] = CPU2toCPU1.duty[0];
        duty_temp[1] = CPU2toCPU1.duty[1];
        duty_temp[2] = CPU2toCPU1.duty[2];
        duty_temp[3] = CPU2toCPU1.duty[3];

        if(!first)
        {
            first = 1;

            Scope.acquire_before_trigger = SCOPE_BUFFER / 2;

            Scope.data_in[0] = &Meas_master.U_grid.a;
            Scope.data_in[1] = &Meas_master.U_grid.b;
            Scope.data_in[2] = &Meas_master.U_grid.c;
            Scope.data_in[3] = (float *)&duty_temp[0];
            Scope.data_in[4] = (float *)&duty_temp[1];
            Scope.data_in[5] = (float *)&duty_temp[2];
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

    Grid_analyzer_calc();
    Energy_meter_CPUasm();

    Timer_PWM.CPU_GRID = TIMESTAMP_PWM;


    GPIO_CLEAR(TRIGGER_CM);

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    Timer_PWM.CPU_END = TIMESTAMP_PWM;
}
