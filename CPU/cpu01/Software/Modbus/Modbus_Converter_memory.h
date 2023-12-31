//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_MODBUS_CONVERTER_MEMORY_H_
#define SOFTWARE_MODBUS_CONVERTER_MEMORY_H_

#include <limits.h>

#include "stdafx.h"
#include "State_background.h"
#include "Modbus_ADU_slave.h"

struct Modbus_Converter_memory_struct
{
    struct Modbus_slave_info_struct info;

    struct
    {
        Uint16 dummy:1;
    }discrete_inputs;

    struct
    {
        Uint16 dummy:1;
    }coils;

    struct
    {
        Uint16 FatFS_response[128];
        Uint32 dummy[2];
//        union ALARM_master alarm_master;
//        union ALARM_master alarm_master_snapshot;
        struct STATUS_ACDC status_ACDC;
        Uint32 Machine_slave_state;//32bit
        Uint32 Converter_state;//32bit
        int16 Temp1;
        int16 Temp2;
        int16 Temp3;
        int16 Temp_CPU;
        float L_grid_previous[10];
        Uint16 file_number_errors;
        struct time_BCD_struct RTC_current_time;//48bit
        Uint16 file_number_logs;
        Uint16 reserved_02; //kompilator wyrownuje nastepna unie do parzystego adresu (chyba dlatego, ze jest w niej Uint32 all[2])
        Uint16 padding1[128-32-2*2-sizeof(struct STATUS_ACDC)-2];
        struct Grid_parameters_struct Grid_filter;
        float frequency;
        Uint16 rtu_port_id;
        Uint16 reserved_01;

        struct Energy_meter_upper_struct Energy_meter;
        Uint16 padding2[256-sizeof(struct Grid_parameters_struct)-sizeof(struct Energy_meter_upper_struct)-4];
        struct
        {
            float I_grid_a[FPGA_KALMAN_STATES];
            float I_grid_b[FPGA_KALMAN_STATES];
            float I_grid_c[FPGA_KALMAN_STATES];
            float U_grid_a[FPGA_KALMAN_STATES];
            float U_grid_b[FPGA_KALMAN_STATES];
            float U_grid_c[FPGA_KALMAN_STATES];
        }harmonic_rms_values;
        struct
        {
            float I_grid_a[FPGA_KALMAN_STATES];
            float I_grid_b[FPGA_KALMAN_STATES];
            float I_grid_c[FPGA_KALMAN_STATES];
            float U_grid_a[FPGA_KALMAN_STATES];
            float U_grid_b[FPGA_KALMAN_STATES];
            float U_grid_c[FPGA_KALMAN_STATES];
        }harmonic_THD_individual;

        struct Energy_meter_algebraic_sum_struct
        {
            Uint64 P_p;
            Uint64 P_n;
            Uint64 QI;
            Uint64 QII;
            Uint64 QIII;
            Uint64 QIV;
        }Energy_meter_algebraic_sum;
    }input_registers;

    struct
    {
        Uint16 FatFS_request[128];
        struct CONTROL_ACDC control_ACDC;
        struct time_BCD_struct RTC_new_time;//48bit
        Uint16 even_address_padding;
        union CONTROL_EXT_MODBUS control_ext_modbus;
        float PWM_phase_shift;
    }holding_registers;
};

extern struct Modbus_Converter_memory_struct Modbus_Converter;

#endif /* SOFTWARE_MODBUS_CONVERTER_MEMORY_H_ */
