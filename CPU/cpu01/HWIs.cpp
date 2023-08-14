/*
 * HWIs.cpp
 *
 *  Created on: 22 lis 2019
 *      Author: Mr.Tea
 */

#include <math.h>

#include "stdafx.h"

#include "State_master.h"
#include "State_background.h"

#include "HWIs.h"
#include "Scope.h"
#include "SD_card.h"
#include "Modbus_devices.h"

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void SD_AVG_INT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

    {
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
        src = (Uint32 *)&EMIF_mem.read.Temperature_module_pos;
        dest = (Uint32 *)&EMIF_CPU;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
    }

    Cla1ForceTask1();

    static volatile struct trigonometric_struct U_dc_100Hz, I_dc_100Hz;
    register float modifier1 = Conv.div_range_modifier_Kalman_values;
    Kalman_U_grid[0].states[2] = (float)EMIF_mem.read.Kalman.series[0].states[1].x1 * modifier1;
    Conv.U_dc_kalman = (float)EMIF_mem.read.Kalman_DC.series[0].states[0].x1 * modifier1;
    U_dc_100Hz.cosine = (float)EMIF_mem.read.Kalman_DC.series[0].states[1].x1 * modifier1;
    I_dc_100Hz.cosine = (float)EMIF_mem.read.Kalman_DC.series[1].states[1].x1 * modifier1;
    U_dc_100Hz.sine = (float)EMIF_mem.read.Kalman_DC.series[0].states[1].x2 * modifier1;
    I_dc_100Hz.sine = (float)EMIF_mem.read.Kalman_DC.series[1].states[1].x2 * modifier1;
    Conv.U_dc_100Hz_sqr = U_dc_100Hz.cosine * U_dc_100Hz.cosine + U_dc_100Hz.sine * U_dc_100Hz.sine;
    Conv.Q_100Hz = U_dc_100Hz.cosine * I_dc_100Hz.sine - U_dc_100Hz.sine * I_dc_100Hz.cosine;
    Conv.C_dc_meas = Saturation(Conv.Q_100Hz / (Conv.U_dc_100Hz_sqr * Conv.w_filter * 2.0f), 0.0f, 5e-3);

//    {
//        static volatile struct
//        {
//            struct abc_struct harmonic[5];
//            float estimate;
//            float error;
//        }Kalman_mem;
//        int32 *pointer_src = (int32 *)&EMIF_mem.read.Kalman.series[0].states[0];
//        float *pointer_dst = (float *)&Kalman_mem;
//        for(int i=0; i<5; i++)
//        {
//            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Kalman_values;
//            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Kalman_values;
//            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Kalman_values_square;
//        }
//
//        pointer_src = (int32 *)&EMIF_mem.read.Kalman.series[1].estimate;
//        *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Kalman_values;
//        *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Kalman_values;
//    }

    {
        static volatile struct
        {
            float harmonic[5][5];
            float sum;
            float error;
        }Resonant_mem;
        int32 *pointer_src = (int32 *)&EMIF_mem.read.Resonant[0].series[0].states[0];
        float *pointer_dst = (float *)&Resonant_mem;
        for(int i=0; i<5; i++)
        {
            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
            *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
        }

        pointer_src = (int32 *)&EMIF_mem.read.Resonant[0].series[0].SUM;
        *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
        *pointer_dst++ = (float)*pointer_src++ * Conv.div_range_modifier_Resonant_values;
    }

    Conv.w_filter = CPU2toCPU1.w_filter;
    Conv.f_filter = CPU2toCPU1.f_filter;
    Conv.sign = CPU2toCPU1.sign;
    Conv.PLL_RDY = CPU2toCPU1.PLL_RDY;
    Conv.sag = CPU2toCPU1.sag;

    CPU1toCPU2.CLA1toCLA2.no_neutral = Conv.no_neutral;
    CPU1toCPU2.CLA1toCLA2.enable = Conv.RDY;
    CPU1toCPU2.CLA1toCLA2.L_conv = Conv.L_conv;
    CPU1toCPU2.CLA1toCLA2.Kp_I = Conv.Kp_I;
    CPU1toCPU2.CLA1toCLA2.id_ref.a = Conv.PI_Id[0].out;
    CPU1toCPU2.CLA1toCLA2.id_ref.b = Conv.PI_Id[1].out;
    CPU1toCPU2.CLA1toCLA2.id_ref.c = Conv.PI_Id[2].out;
    CPU1toCPU2.CLA1toCLA2.iq_ref.a = Conv.PI_Iq[0].out;
    CPU1toCPU2.CLA1toCLA2.iq_ref.b = Conv.PI_Iq[1].out;
    CPU1toCPU2.CLA1toCLA2.iq_ref.c = Conv.PI_Iq[2].out;
    CPU1toCPU2.CLA1toCLA2.select_modulation = Conv.select_modulation;

    Timer_PWM.CPU_COPY1 = TIMESTAMP_PWM;

    while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
    PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

    static volatile float duty_temp[4];
    duty_temp[0] = CPU2toCPU1.duty[0];
    duty_temp[1] = CPU2toCPU1.duty[1];
    duty_temp[2] = CPU2toCPU1.duty[2];
    duty_temp[3] = CPU2toCPU1.duty[3];
    Conv.P_conv = Meas_ACDC.U_grid.a * Meas_ACDC.I_conv.a + Meas_ACDC.U_grid.b * Meas_ACDC.I_conv.b + Meas_ACDC.U_grid.c * Meas_ACDC.I_conv.c;
    Conv.I_dc = -(duty_temp[0] * Meas_ACDC.I_conv.a + duty_temp[1] * Meas_ACDC.I_conv.b + duty_temp[2] * Meas_ACDC.I_conv.c + duty_temp[3] * Meas_ACDC.I_conv.n) / fmaxf(2.0f * EMIF_mem.read.cycle_period, 1.0f);

//    static float angle = 0.0f;
//    angle += Conv.Ts * Conv.w_filter;
//    angle -= (float)((int32)(angle * MATH_1_PI)) * MATH_2PI;
//    static volatile float zmienna;
//    zmienna = 20.0f * (1.0f + sinf(angle) + sinf(7.0f * angle));

    static Uint16 indexer = 0;
    if(++indexer >= 6) indexer = 0;
    if(indexer < 3) Kalman_THD_calc_CPUasm(&Kalman_U_grid[indexer], (int32 *)&EMIF_mem.read.Kalman.series[indexer].states[0].A);
    else Kalman_THD_calc_CPUasm(&Kalman_I_grid[indexer-3], (int32 *)&EMIF_mem.read.Kalman.series[indexer].states[0].A);

    register float modifier2 = Conv.range_modifier_Kalman_values;
    EMIF_mem.write.Kalman.input[0] = Meas_ACDC.U_grid.a * modifier2;
    EMIF_mem.write.Kalman.input[1] = Meas_ACDC.U_grid.b * modifier2;
    EMIF_mem.write.Kalman.input[2] = Meas_ACDC.U_grid.c * modifier2;
    EMIF_mem.write.Kalman.input[3] = Meas_ACDC.I_grid.a * modifier2;
    EMIF_mem.write.Kalman.input[4] = Meas_ACDC.I_grid.b * modifier2;
    EMIF_mem.write.Kalman.input[5] = Meas_ACDC.I_grid.c * modifier2;
    EMIF_mem.write.Kalman_DC.input[0] = Meas_ACDC.U_dc * modifier2;
    EMIF_mem.write.Kalman_DC.input[1] = Conv.I_dc * modifier2;
    EMIF_mem.write.DSP_start = 0b11000000;

    EMIF_mem.write.Resonant[0].series[0].HC =
    EMIF_mem.write.Resonant[1].series[0].HC =
    EMIF_mem.write.Resonant[2].series[0].HC = fmaxf(Conv.resonant_odd_number, 0.0f);
    EMIF_mem.write.Resonant[0].series[0].HG = *(Uint32 *)&control_ACDC.H_odd_a;
    EMIF_mem.write.Resonant[1].series[0].HG = *(Uint32 *)&control_ACDC.H_odd_b;
    EMIF_mem.write.Resonant[2].series[0].HG = *(Uint32 *)&control_ACDC.H_odd_c;
    EMIF_mem.write.Resonant[3].series[0].HC =
    EMIF_mem.write.Resonant[4].series[0].HC =
    EMIF_mem.write.Resonant[5].series[0].HC = fmaxf(Conv.resonant_even_number, 0.0f);
    EMIF_mem.write.Resonant[3].series[0].HG = *(Uint32 *)&control_ACDC.H_even_a;
    EMIF_mem.write.Resonant[4].series[0].HG = *(Uint32 *)&control_ACDC.H_even_b;
    EMIF_mem.write.Resonant[5].series[0].HG = *(Uint32 *)&control_ACDC.H_even_c;
    register float modifier3 = Conv.range_modifier_Resonant_coefficients;
    EMIF_mem.write.Resonant[3].series[0].RT =
    EMIF_mem.write.Resonant[0].series[0].RT = Conv.PI_I_harm_ratio[0].out * modifier3;
    EMIF_mem.write.Resonant[4].series[0].RT =
    EMIF_mem.write.Resonant[1].series[0].RT = Conv.PI_I_harm_ratio[1].out * modifier3;
    EMIF_mem.write.Resonant[5].series[0].RT =
    EMIF_mem.write.Resonant[2].series[0].RT = Conv.PI_I_harm_ratio[2].out * modifier3;

    Timer_PWM.CPU_COPY2 = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(!status_ACDC.Init_done) Conv.enable = 0;
    else
    {
        ONOFF_switch_interrupt();

        if(EPwm4Regs.TZFLG.bit.OST) alarm_ACDC.bit.TZ_CPU1 = 1;
        if(EPwm4Regs.TZOSTFLG.bit.OST5) alarm_ACDC.bit.TZ_CLOCKFAIL_CPU1 = 1;
        if(EPwm4Regs.TZOSTFLG.bit.OST6) alarm_ACDC.bit.TZ_EMUSTOP_CPU1 = 1;

        alarm_ACDC.bit.FPGA_errors.all = EMIF_mem.read.FPGA_flags.all;

        if(Conv.enable)
        {
            if(!Conv.PLL_RDY) alarm_ACDC.bit.PLL_UNSYNC = 1;

            if(Grid.U_grid.a < Meas_ACDC_alarm_L.U_grid_rms) alarm_ACDC.bit.U_grid_rms_a_L = 1;
            if(Grid.U_grid.b < Meas_ACDC_alarm_L.U_grid_rms) alarm_ACDC.bit.U_grid_rms_b_L = 1;
            if(Grid.U_grid.c < Meas_ACDC_alarm_L.U_grid_rms) alarm_ACDC.bit.U_grid_rms_c_L = 1;
        }

        status_ACDC.PLL_sync = Conv.PLL_RDY;
        float compare_U_rms = Meas_ACDC_alarm_L.U_grid_rms + 10.0f;
        if(Grid_filter.U_grid_1h.a > compare_U_rms && Grid_filter.U_grid_1h.b > compare_U_rms && Grid_filter.U_grid_1h.c > compare_U_rms)
            status_ACDC.Grid_present = 1;
        else status_ACDC.Grid_present = 0;

        if(Meas_ACDC.Supply_24V < 22.0) alarm_ACDC.bit.FLT_SUPPLY_MASTER = 1;

        if(Meas_ACDC.U_dc_avg < Meas_ACDC_alarm_L.U_dc) alarm_ACDC.bit.U_dc_L = 1;
        if(Meas_ACDC.U_dc_avg > Meas_ACDC_alarm_H.U_dc) alarm_ACDC.bit.U_dc_H = 1;
        if(Meas_ACDC.U_dc_n_avg < Meas_ACDC_alarm_L.U_dc*0.5f) alarm_ACDC.bit.U_dc_n_L = 1;
        if(Meas_ACDC.U_dc_n_avg > Meas_ACDC_alarm_H.U_dc*0.5f) alarm_ACDC.bit.U_dc_n_H = 1;
        if(fabsf(Meas_ACDC.U_dc - 2.0f * Meas_ACDC.U_dc_n) > Meas_ACDC_alarm_H.U_dc_balance) alarm_ACDC.bit.U_dc_balance = 1;

        if(Grid.I_conv.a > Meas_ACDC_alarm_H.I_conv_rms) alarm_ACDC.bit.I_conv_rms_a = 1;
        if(Grid.I_conv.b > Meas_ACDC_alarm_H.I_conv_rms) alarm_ACDC.bit.I_conv_rms_b = 1;
        if(Grid.I_conv.c > Meas_ACDC_alarm_H.I_conv_rms) alarm_ACDC.bit.I_conv_rms_c = 1;
        if(Grid.I_conv.n > Meas_ACDC_alarm_H.I_conv_rms) alarm_ACDC.bit.I_conv_rms_n = 1;

        static volatile float Temp_max = 0;
        Temp_max = fmaxf(Meas_ACDC.Temperature1, fmaxf(Meas_ACDC.Temperature2, Meas_ACDC.Temperature3));
        if(Temp_max > Meas_ACDC_alarm_H.Temp) alarm_ACDC.bit.Temperature_H = 1;
        if(Temp_max < Meas_ACDC_alarm_L.Temp) alarm_ACDC.bit.Temperature_L = 1;

        alarm_ACDC.all[0] |= CPU2toCPU1.alarm_master.all[0];
        alarm_ACDC.all[1] |= CPU2toCPU1.alarm_master.all[1];
        alarm_ACDC.all[2] |= CPU2toCPU1.alarm_master.all[2];

        if((alarm_ACDC.all[0] | alarm_ACDC.all[1] | alarm_ACDC.all[2]) && !(alarm_ACDC_snapshot.all[0] | alarm_ACDC_snapshot.all[1] | alarm_ACDC_snapshot.all[2]))
        {
            EALLOW;
            EPwm4Regs.TZFRC.bit.OST = 1;
            EDIS;
            EMIF_mem.write.Scope_trigger = 1;
            alarm_ACDC_snapshot.all[0] = alarm_ACDC.all[0];
            alarm_ACDC_snapshot.all[1] = alarm_ACDC.all[1];
            alarm_ACDC_snapshot.all[2] = alarm_ACDC.all[2];
        }
    }

    Timer_PWM.CPU_ERROR = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        static Uint16 first = 0;

        static volatile Uint16 decimation_ratio = 1;
        static Uint16 decimation = 0;

        static float trigger_temp;
        trigger_temp = Machine_master.state == Machine_master_class::state_Lgrid_meas;
        static float* volatile trigger_pointer = &trigger_temp;
        static volatile float trigger_val = (float)Machine_master_class::state_Lgrid_meas;
        static volatile float edge = 1;
        static volatile float trigger_last;

        if(!first)
        {
            first = 1;

            Scope.acquire_before_trigger = SCOPE_BUFFER / 2;

            Scope.data_in[0] = &Meas_ACDC.U_grid.a;
            Scope.data_in[1] = &Meas_ACDC.U_grid.b;
            Scope.data_in[2] = &Meas_ACDC.U_grid.c;
            Scope.data_in[3] = &Meas_ACDC.I_grid.a;
            Scope.data_in[4] = &Meas_ACDC.I_grid.b;
            Scope.data_in[5] = &Meas_ACDC.I_grid.c;
            Scope.data_in[6] = &Meas_ACDC.U_dc;
            Scope.data_in[7] = &Meas_ACDC.U_dc_n;
            Scope.data_in[8] = &Meas_ACDC.I_conv.a;
            Scope.data_in[9] = &Meas_ACDC.I_conv.b;
            Scope.data_in[10] = &Meas_ACDC.I_conv.c;
            Scope.data_in[11] = &Meas_ACDC.I_conv.n;
            Scope.acquire_counter = -1;

            trigger_last = *trigger_pointer;
        }

        if (++decimation >= decimation_ratio)
        {
            if(status_ACDC.Scope_snapshot_pending)
            {
                if(SD_card.Scope_snapshot_state == 1) Scope_trigger(Kalman_U_grid[0].states[2], &SD_card.Scope_input_last, 0.0f, 1);
            }
            else
            {
                if(alarm_ACDC.all[0] | alarm_ACDC.all[1] | alarm_ACDC.all[2]) Scope_trigger_unc();
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
    Grid_analyzer_filter_calc();
    Energy_meter_calc();
    Energy_meter_CPUasm();

    Timer_PWM.CPU_GRID = TIMESTAMP_PWM;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static Uint32 zeroes[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

    static volatile union FPGA_ACDC_sync_flags_union Sync_flags;
    Sync_flags.all = EMIF_mem.read.Sync_flags.all;
    status_ACDC.master_slave_selector = !Sync_flags.bit.rx1_port_rdy;
    status_ACDC.master_rdy = Sync_flags.bit.master_rdy;
    status_ACDC.slave_rdy_0 = Sync_flags.bit.slave_rdy_0;
    status_ACDC.slave_rdy_1 = Sync_flags.bit.slave_rdy_1;
    status_ACDC.slave_rdy_2 = Sync_flags.bit.slave_rdy_2;
    status_ACDC.slave_rdy_3 = Sync_flags.bit.slave_rdy_3;
    status_ACDC.rx1_port_rdy = Sync_flags.bit.rx1_port_rdy;
    status_ACDC.rx2_port_rdy = Sync_flags.bit.rx2_port_rdy;
    register Uint16 number_of_slaves = Sync_flags.bit.slave_rdy_0 + Sync_flags.bit.slave_rdy_1 + Sync_flags.bit.slave_rdy_2 + Sync_flags.bit.slave_rdy_3;
    status_ACDC.incorrect_number_of_slaves = number_of_slaves != status_ACDC.expected_number_of_slaves;

    register Uint32 *src;
    register Uint32 *dest;

    if(status_ACDC.master_slave_selector)
    {
        struct COMM_header_struct comm_header;
        comm_header.length = sizeof(COMM_master_sync_msg1_struct)-1;
        comm_header.rsvd = 0;
        comm_header.destination_mailbox = 1;
        Uint32 header_temp = *(Uint16 *)&comm_header;

        dest = (Uint32 *)&EMIF_mem.write.tx2_hipri_msg[1];
        src = (Uint32 *)&Conv.master.total.id_lim;
        while(EMIF_mem.read.tx_wip.bit.port2_hipri_msg & (1<<1));
        *dest++ = header_temp;

        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        src = (Uint32 *)&Conv.master.total.iq_lim;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *(Uint32 *)&Conv.master.slave[0].ratio;
        *dest++ = *(Uint32 *)&Conv.master.slave[1].ratio;
        *dest++ = *(Uint32 *)&Conv.master.slave[2].ratio;
        *dest++ = *(Uint32 *)&Conv.master.slave[3].ratio;
        src = (Uint32 *)&status_ACDC;
        *dest++ = *src++;
        *dest++ = *src++;

        union COMM_flags_union flags_temp;
        flags_temp.all = 0;
        flags_temp.bit.port2_hipri_msg = 1 << 1;
        EMIF_mem.write.tx_start.all = flags_temp.all;
    }
    else
    {
        register Uint32 *src;
        register Uint32 *dest;

        dest = (Uint32 *)((Uint16 *)EMIF_mem.write.tx1_hipri_msg[0] + offsetof(COMM_slave_sync_msg_struct, P_conv_1h));
        src = (Uint32 *)&Grid.P_conv_1h;
        while(EMIF_mem.read.tx_wip.bit.port1_hipri_msg & (1<<0));
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        src = (Uint32 *)&Grid.Q_conv_1h;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        src = (Uint32 *)&Conv.I_lim;
        *dest++ = *src++;

        dest = (Uint32 *)&Conv.slave.from_master;
        src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx1_hipri_msg[1] + offsetof(COMM_master_sync_msg1_struct, id_ref));
        if(status_ACDC.master_rdy)
        {
            while(EMIF_mem.read.rx_wip.bit.port1_hipri_msg & (1<<1));
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            *dest++ = *src++;
            dest = (Uint32 *)&status_ACDC_master;
            *dest++ = *src++;
            *dest++ = *src++;

            Conv.slave.ratio_local = Conv.slave.from_master.ratio[Sync_flags.bit.node_number];
        }
    }

    //////////////////////////////
    //poza warunkami, aby zawsze bylo zerowanie

    dest = (Uint32 *)&Conv.master.from_slave[3];
    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx2_hipri_msg[3] + offsetof(COMM_slave_sync_msg_struct, P_conv_1h));
    if(!status_ACDC.slave_rdy_3 || !status_ACDC.master_slave_selector) src = zeroes;
    while(EMIF_mem.read.rx_wip.bit.port2_hipri_msg & (1<<3));
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    dest = (Uint32 *)&Conv.master.from_slave[2];
    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx2_hipri_msg[2] + offsetof(COMM_slave_sync_msg_struct, P_conv_1h));
    if(!status_ACDC.slave_rdy_2 || !status_ACDC.master_slave_selector) src = zeroes;
    while(EMIF_mem.read.rx_wip.bit.port2_hipri_msg & (1<<2));
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    dest = (Uint32 *)&Conv.master.from_slave[1];
    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx2_hipri_msg[1] + offsetof(COMM_slave_sync_msg_struct, P_conv_1h));
    if(!status_ACDC.slave_rdy_1 || !status_ACDC.master_slave_selector) src = zeroes;
    while(EMIF_mem.read.rx_wip.bit.port2_hipri_msg & (1<<1));
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    dest = (Uint32 *)&Conv.master.from_slave[0];
    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx2_hipri_msg[0] + offsetof(COMM_slave_sync_msg_struct, P_conv_1h));
    if(!status_ACDC.slave_rdy_0 || !status_ACDC.master_slave_selector) src = zeroes;
    while(EMIF_mem.read.rx_wip.bit.port2_hipri_msg & (1<<0));
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //    static volatile union double_pulse_union
    //    {
    //       Uint32 u32;
    //       Uint16 u16[2];
    //    }double_pulse = {.u16 = {700, 15}};
    //    EMIF_mem.write.double_pulse = double_pulse.u32;
    //
    //    static volatile float counter = 0;
    //    if(Machine.ONOFF != Machine.ONOFF_last)
    //    {
    //        counter = 0;
    //        GPIO_SET(PWM_EN_CM);
    //    }
    //    counter += Conv.Ts;
    //    if(counter >= 1.0f)
    //    {
    //       GPIO_CLEAR(PWM_EN_CM);
    //    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    Timer_PWM.CPU_END = TIMESTAMP_PWM;

    static volatile Uint16 Timer_max;
    Uint16 timestamp = TIMESTAMP_PWM;
    if(timestamp < 2500) timestamp += 5000;
    if(timestamp > Timer_max) Timer_max = timestamp;
}

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void NMI_INT()
{
    ESTOP0;
}
