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

union FPGA_master_sync_flags_union Sync_flags;
union COMM_flags_union Comm_flags;
union FPGA_master_flags_union FPGA_flags;
Uint32 zeroes[3] = {0,0,0};

#pragma CODE_SECTION(".TI.ramfunc");
interrupt void SD_INT()
{
    Timer_PWM.CPU_SD = TIMESTAMP_PWM;

    Sync_flags.all = EMIF_mem.read.Sync_flags.all;
    Comm_flags.all = EMIF_mem.read.rx_rdy.all;

    //miejsce odczytu niebezpieczne
    union COMM_flags_union flags_temp;
    flags_temp.all = 0;
    flags_temp.bit.port1_hipri_msg = Comm_flags.bit.port1_hipri_msg;
    EMIF_mem.write.rx_ack.all = flags_temp.all;

    register Uint32 *src;
    register Uint32 *dest;

    dest = (Uint32 *)&EMIF_CLA.slave_meas[0];
    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx1_hipri_msg[0] + offsetof(COMM_slave_sync_msg_struct, slave_meas));
    if(!Sync_flags.bit.slave_rdy_0)
    {
        src = zeroes;
        Conv.I_lim_slave[0] = 0.0f;
    }
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx1_hipri_msg[1] + offsetof(COMM_slave_sync_msg_struct, slave_meas));
    if(!Sync_flags.bit.slave_rdy_1)
    {
        src = zeroes;
        Conv.I_lim_slave[1] = 0.0f;
    }
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx1_hipri_msg[2] + offsetof(COMM_slave_sync_msg_struct, slave_meas));
    if(!Sync_flags.bit.slave_rdy_2)
    {
        src = zeroes;
        Conv.I_lim_slave[2] = 0.0f;
    }
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    src = (Uint32 *)((Uint16 *)EMIF_mem.read.rx1_hipri_msg[3] + offsetof(COMM_slave_sync_msg_struct, slave_meas));
    if(!Sync_flags.bit.slave_rdy_3)
    {
        src = zeroes;
        Conv.I_lim_slave[3] = 0.0f;
    }
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    Cla1ForceTask1();

    src = (Uint32 *)&EMIF_mem.read.U_grid_a;
    dest = (Uint32 *)&EMIF_CLA.U_grid_a;

    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;

    GPIO_SET(TRIGGER1_CS);

    static Uint16 decimator_cpu2 = 0;

    decimator = decimator_cpu2;
    U_x0.a = Kalman_U_grid[0].states[2];
    U_x0.b = Kalman_U_grid[1].states[2];
    U_x0.c = Kalman_U_grid[2].states[2];
    U_x1.a = Kalman_U_grid[0].states[3];
    U_x1.b = Kalman_U_grid[1].states[3];
    U_x1.c = Kalman_U_grid[2].states[3];

    if(decimator_cpu2++)
    {
        decimator_cpu2 = 0;

        CPU1toCPU2.CLA1toCLA2.w_filter = PLL.w_filter;
        CPU1toCPU2.CLA1toCLA2.I_lim = Conv.I_lim_avg_prefilter;

        src = (Uint32 *)&Meas_slave_avg.I_conv_avg;
        dest = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.Meas_slave.I_conv_avg;

        while(!PieCtrlRegs.PIEIFR11.bit.INTx1);
        PieCtrlRegs.PIEIFR11.bit.INTx1 = 0;

        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;

        IpcRegs.IPCSET.bit.IPC3 = 1;
        IpcRegs.IPCCLR.bit.IPC3 = 1;

        static volatile Uint32 loop_count = 23;
        Uint32 loop_counter = loop_count;
        while(loop_counter--) asm(" NOP");

        Timer_PWM.CPU_COPY1 = TIMESTAMP_PWM;
    }
    else
    {
        //copying last grid measurements to compensate for slave measurements delay
        src = (Uint32 *)&Meas_master.U_grid_avg;
        dest = (Uint32 *)&CPU1toCPU2.CLA1toCLA2.Meas_master.U_grid_avg;

        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;

        Energy_meter_CPUasm();

        Timer_PWM.CPU_COPY2 = TIMESTAMP_PWM;
    }

    GPIO_CLEAR(TRIGGER1_CS);

    register Uint16 number_of_slaves = Sync_flags.bit.sync_ok_0 + Sync_flags.bit.sync_ok_1 + Sync_flags.bit.sync_ok_2 + Sync_flags.bit.sync_ok_3;
    status_master.incorrect_number_of_slaves = number_of_slaves != status_master.expected_number_of_slaves;
    status_master.slave_any_sync = Sync_flags.bit.sync_ok_0 || Sync_flags.bit.sync_ok_1 || Sync_flags.bit.sync_ok_2 || Sync_flags.bit.sync_ok_3;

    status_master.slave_rdy_0 = Sync_flags.bit.slave_rdy_0;
    status_master.slave_rdy_1 = Sync_flags.bit.slave_rdy_1;
    status_master.slave_rdy_2 = Sync_flags.bit.slave_rdy_2;
    status_master.slave_rdy_3 = Sync_flags.bit.slave_rdy_3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(!status_master.Init_done) Conv.enable = 0;
    else
    {
        ONOFF_switch_interrupt();

        if(Conv.enable && !status_master.slave_any_sync) alarm_master.bit.no_sync = 1;

        status_master.PLL_sync = PLL.RDY;
        float compare_U_rms = Meas_alarm_L.U_grid_rms + 10.0f;
        if(CLA2toCLA1.Grid_filter.U_grid_1h.a > compare_U_rms && CLA2toCLA1.Grid_filter.U_grid_1h.b > compare_U_rms && CLA2toCLA1.Grid_filter.U_grid_1h.c > compare_U_rms)
            status_master.Grid_present = 1;
        else status_master.Grid_present = 0;

        FPGA_flags.all = EMIF_mem.read.FPGA_flags.all;
        if(FPGA_flags.bit.rx1_crc_error) alarm_master.bit.rx1_crc_error = 1;
        if(FPGA_flags.bit.rx1_overrun_error) alarm_master.bit.rx1_overrun_error = 1;
        if(FPGA_flags.bit.rx1_frame_error) alarm_master.bit.rx1_frame_error = 1;
//        if(FPGA_flags.bit.rx2_crc_error) alarm_master.bit.rx2_crc_error = 1;
//        if(FPGA_flags.bit.rx2_overrun_error) alarm_master.bit.rx2_overrun_error = 1;
//        if(FPGA_flags.bit.rx2_frame_error) alarm_master.bit.rx2_frame_error = 1;
        if(FPGA_flags.bit.rx1_port_nrdy) alarm_master.bit.rx1_port_nrdy = 1;
//        if(FPGA_flags.bit.rx2_port_nrdy) alarm_master.bit.rx2_port_nrdy = 1;
        if(FPGA_flags.bit.sed_err) alarm_master.bit.sed_err = 1;
        if(FPGA_flags.bit.fault_supply) alarm_master.bit.FLT_SUPPLY_MASTER = 1;

        if(Conv.enable)
        {
            if(!PLL.RDY) alarm_master.bit.PLL_UNSYNC = 1;

            if(CLA2toCLA1.Grid.U_grid.a < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_a_L = 1;
            if(CLA2toCLA1.Grid.U_grid.b < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_b_L = 1;
            if(CLA2toCLA1.Grid.U_grid.c < Meas_alarm_L.U_grid_rms) alarm_master.bit.U_grid_rms_c_L = 1;

            if(fabs(Meas_master.U_grid_avg.a) > Meas_alarm_H.U_grid_abs) alarm_master.bit.U_grid_abs_a_H = 1;
            if(fabs(Meas_master.U_grid_avg.b) > Meas_alarm_H.U_grid_abs) alarm_master.bit.U_grid_abs_b_H = 1;
            if(fabs(Meas_master.U_grid_avg.c) > Meas_alarm_H.U_grid_abs) alarm_master.bit.U_grid_abs_c_H = 1;
        }

        if(alarm_master.all && !alarm_master_snapshot.all)
        {
            if(Machine.look_for_errors) status_master.scope_trigger_request = 1;
            alarm_master_snapshot = alarm_master;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Timer_PWM.CPU_ERROR = TIMESTAMP_PWM;

    {
        static Uint16 first = 0;

        static float trigger_temp;
        trigger_temp = Machine.state == Machine_class::state_Lgrid_meas;
        static float* volatile trigger_pointer = &trigger_temp;
        static volatile float trigger_val = (float)Machine_class::state_Lgrid_meas;
        static volatile float edge = 1;
        static volatile float trigger_last;

        if(!first)
        {
            first = 1;

            scope_global.decimation_ratio = 1;
            scope_global.acquire_before_trigger = 2000;


            Scope.data_in[0] = &Meas_master.U_grid.a;
            Scope.data_in[1] = &Meas_master.U_grid.b;
            Scope.data_in[2] = &Meas_master.U_grid.c;
            Scope.data_in[3] = &Meas_master.I_grid.a;
            Scope.data_in[4] = &Meas_master.I_grid.b;
            Scope.data_in[5] = &Meas_master.I_grid.c;
            Scope.acquire_counter = -1;

            trigger_last = *trigger_pointer;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(scope_global.zero_counter)
        {
            Scope.acquire_before_trigger = scope_global.acquire_before_trigger;

            Scope_task();
        }

        if(SD_card.Scope_snapshot_state == 1 && Scope_trigger(Kalman_U_grid[0].states[2], &SD_card.Scope_input_last, 0.0f, 1))
            scope_global.scope_trigger = 1;

        if((Sync_flags.bit.scope_trigger_request || status_master.scope_trigger_request) && !scope_global.scope_trigger)
        {
            SD_card_class::save_error_state = 1;
            scope_global.scope_trigger = 1;
            EMIF_mem.write.Scope_trigger = 1;
            status_master.scope_trigger_request = 0;
        }

//        if(fabs(Meas_master.U_grid_avg.a - Kalman_U_grid[0].estimate) > 10.0f ||
//           fabs(Meas_master.U_grid_avg.b - Kalman_U_grid[1].estimate) > 10.0f ||
//           fabs(Meas_master.U_grid_avg.c - Kalman_U_grid[2].estimate) > 10.0f) status_master.scope_trigger_request = 1;
//        status_master.scope_trigger_request |= Scope_trigger(*trigger_pointer, (float *)&trigger_last, trigger_val, 1);

        if(scope_global.scope_trigger)
            Scope_trigger_unc();
        else
            Scope_start();

        if (++Scope.decimation >= scope_global.decimation_ratio)
        {
            scope_global.zero_counter = 1;
            if(!Scope.index && !scope_global.scope_trigger) scope_global.sync_index = 1;
            else scope_global.sync_index = 0;
            Scope.decimation = 0;
        }
        else scope_global.zero_counter = 0;

    }

    Timer_PWM.CPU_SCOPE = TIMESTAMP_PWM;

    GPIO_SET(TRIGGER0_CS);

    struct COMM_header_struct comm_header;
    comm_header.length = sizeof(COMM_master_sync_msg2_union)-1;
    comm_header.rsvd = 0;
    comm_header.destination_mailbox = 2;
    Uint32 onoff = Machine.switch_timer != 0;
    Uint32 header_temp = *(Uint16 *)&comm_header;
    header_temp |= onoff << 16;

    dest = (Uint32 *)&EMIF_mem.write.tx1_hipri_msg[2];
    *dest++ = header_temp;

//    src = (Uint32 *)&fw_descriptor.dsc;
//    *dest++ = *src++;
    *dest++ = SW_ID;
    *dest++ = alarm_master.all;
    src = (Uint32 *)&scope_global;
    *dest++ = *src++;

    flags_temp.all = 0;
    flags_temp.bit.port1_hipri_msg = 1 << 2;
    EMIF_mem.write.tx_start.all = flags_temp.all;

    src = (Uint32 *)&Conv.ratio;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    src = (Uint32 *)&Conv.I_ref;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    src = (Uint32 *)&Conv.U_coupl;
    *dest++ = *src++;
    *dest++ = *src++;
    *dest++ = *src++;
    src = (Uint32 *)&PLL.trig_table[0].cosine;
    *dest++ = *src++, src++;
    *dest++ = *src++, src++;
    *dest++ = *src++, src++;
    src = (Uint32 *)&Kalman_U_grid[0].estimate;
    *dest++ = *src++;
    src = (Uint32 *)&Kalman_U_grid[1].estimate;
    *dest++ = *src++;
    src = (Uint32 *)&Kalman_U_grid[2].estimate;
    *dest++ = *src++;

    Timer_PWM.CPU_TX_MSG2 = TIMESTAMP_PWM;

    Modbus_slave_LCD.RTU->interrupt_task();
    Modbus_slave_LCD_OLD.RTU->interrupt_task();
    Modbus_slave_EXT.RTU->interrupt_task();

    Timer_PWM.CPU_COMM = TIMESTAMP_PWM;

    Fast_copy21_CPUasm();

    GPIO_CLEAR(TRIGGER0_CS);

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    Timer_PWM.CPU_END = TIMESTAMP_PWM;
}
