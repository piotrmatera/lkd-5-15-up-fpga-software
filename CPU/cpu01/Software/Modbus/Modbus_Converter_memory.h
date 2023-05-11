//Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

#ifndef SOFTWARE_MODBUS_CONVERTER_MEMORY_H_
#define SOFTWARE_MODBUS_CONVERTER_MEMORY_H_

#include <limits.h>
#include "Modbus_ADU_slave.h"
#include "stdafx.h"
#include "State.h"
#include "ff.h"

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
        union ALARM_master alarm_master;
        union ALARM_master alarm_master_snapshot;
        struct STATUS_master status_master;
        Uint32 Machine_state;//32bit
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
        union ALARM_slave alarm_slave;
        union ALARM_slave alarm_slave_snapshot;
        struct STATUS_slave status_slave;
        Uint16 padding1[128-32-2*sizeof(union ALARM_master)-sizeof(struct STATUS_master)-2-2*sizeof(union ALARM_slave)-sizeof(struct STATUS_slave)];
        struct Grid_parameters_struct Grid_filter;
        float frequency;
        Uint16 rtu_port_id;
        Uint16 reserved_01;

        struct Energy_meter_upper_struct Energy_meter;
        Uint16 padding2[256-sizeof(struct Grid_parameters_struct)-sizeof(struct Energy_meter_upper_struct)-4];
        struct
        {
            float I_grid_a[KALMAN_HARMONICS];
            float I_grid_b[KALMAN_HARMONICS];
            float I_grid_c[KALMAN_HARMONICS];
            float U_grid_a[KALMAN_HARMONICS];
            float U_grid_b[KALMAN_HARMONICS];
            float U_grid_c[KALMAN_HARMONICS];
        }harmonic_rms_values;
        struct
        {
            float I_grid_a[KALMAN_HARMONICS];
            float I_grid_b[KALMAN_HARMONICS];
            float I_grid_c[KALMAN_HARMONICS];
            float U_grid_a[KALMAN_HARMONICS];
            float U_grid_b[KALMAN_HARMONICS];
            float U_grid_c[KALMAN_HARMONICS];
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
        struct CONTROL_master control_master;
        struct time_BCD_struct RTC_new_time;//48bit
        Uint16 even_address_padding;
        union CONTROL_EXT_MODBUS control_ext_modbus;
        struct abc_struct Id;
        struct abc_struct Iq;
        float control_type;
        float C_conv;
    }holding_registers;
};

extern struct Modbus_Converter_memory_struct Modbus_Converter;

#endif /* SOFTWARE_MODBUS_CONVERTER_MEMORY_H_ */
