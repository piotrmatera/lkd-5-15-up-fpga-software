#ifndef _CLA_SHARED_H_
#define _CLA_SHARED_H_

#define SCOPE_BUFFER 1250
#define SCOPE_CHANNEL 12
typedef float scope_data_type;

#define SINCOS_HARMONICS 50
#define CIC_upsample1 10
#define CIC_upsample2 100

//
// Included Files
//
#include "F28x_Project.h"

#include "F2837xD_Cla_defines.h"
#include "CLAmath.h"
#include <stdint.h>

#include "Controllers.h"
#include "PLL.h"
#include "Converter.h"
#include "CPU_shared.h"
#include "Node_shared.h"
#include "Kalman.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Defines
//

struct Timer_PWM_struct
{
    Uint16 CLA_START_TASK1;
    Uint16 CLA_MEAS_TASK1;
    Uint16 CLA_PLL_TASK1;
    Uint16 CLA_CONV_TASK1;
    Uint16 CLA_END_TASK1;
    Uint16 CPU_SD;
    Uint16 CPU_COPY1;
    Uint16 CPU_COPY2;
    Uint16 CPU_ERROR;
    Uint16 CPU_SCOPE;
    Uint16 CPU_TX_MSG2;
    Uint16 CPU_COMM;
    Uint16 CPU_END;
};

extern struct Timer_PWM_struct Timer_PWM;

#define TIMESTAMP_PWM EPwm4Regs.TBCTR

struct Measurements_alarm_struct
{
    float U_grid_abs;
    float U_grid_rms;
    float I_grid;
    float Temp;
};

union FPGA_master_sync_flags_union
{
    Uint32 all;
    struct
    {
        Uint16 rx_ok_0:1;
        Uint16 rx_ok_1:1;
        Uint16 rx_ok_2:1;
        Uint16 rx_ok_3:1;
        Uint16 rx_ok_4:1;
        Uint16 rx_ok_5:1;
        Uint16 rx_ok_6:1;
        Uint16 rx_ok_7:1;
        Uint16 sync_ok_0:1;
        Uint16 sync_ok_1:1;
        Uint16 sync_ok_2:1;
        Uint16 sync_ok_3:1;
        Uint16 sync_ok_4:1;
        Uint16 sync_ok_5:1;
        Uint16 sync_ok_6:1;
        Uint16 sync_ok_7:1;
        Uint16 slave_rdy_0:1;
        Uint16 slave_rdy_1:1;
        Uint16 slave_rdy_2:1;
        Uint16 slave_rdy_3:1;
        Uint16 slave_rdy_4:1;
        Uint16 slave_rdy_5:1;
        Uint16 slave_rdy_6:1;
        Uint16 slave_rdy_7:1;
        Uint16 rsvd:8;
    }bit;
};

union EMIF_union
{
    struct
    {
        union COMM_flags_union tx_wip;
        union COMM_flags_union rx_rdy;
        int16 U_grid_a;
        int16 U_grid_b;
        int16 U_grid_c;
        int16 I_grid_a;
        int16 I_grid_b;
        int16 I_grid_c;
        int16 U_dc;
        int16 U_dc_n;
        int16 I_conv_a;
        int16 I_conv_b;
        int16 I_conv_c;
        int16 I_conv_n_dummy;
        union FPGA_master_flags_union FPGA_flags;
        union FPGA_master_sync_flags_union Sync_flags;
        int16 clock_offsets[8];
        int16 comm_delays[8];
        Uint32 SD_sync_val;
        Uint32 dsc;
        Uint32 Scope_data_out1;
        Uint32 Scope_data_out2;
        Uint32 Scope_depth;
        Uint32 Scope_width_mult;
        Uint32 Scope_rdy;
        Uint32 Scope_index_last;
        Uint32 mux_rsvd[1024-26];
        Uint32 rx1_lopri_msg[8][32];
        Uint32 rx1_hipri_msg[8][32];
        Uint32 rx2_lopri_msg[8][32];
        Uint32 rx2_hipri_msg[8][32];
    }read;
    struct
    {
        union COMM_flags_union tx_start;
        union COMM_flags_union rx_ack;
        Uint32 SD_sync_val;
        Uint32 Scope_address;
        Uint32 Scope_acquire_before_trigger;
        Uint32 Scope_trigger;
        int16 duty[4];
        Uint32 double_pulse;
        Uint32 DSP_start;
        Uint32 PWM_control;
        Uint32 mux_rsvd[1024-11];
        Uint32 tx1_lopri_msg[8][32];
        Uint32 tx1_hipri_msg[8][32];
        Uint32 tx2_lopri_msg[8][32];
        Uint32 tx2_hipri_msg[8][32];
    }write;
};

struct EMIF_CLA_struct
{
    int16 U_grid_a;
    int16 U_grid_b;
    int16 U_grid_c;
    int16 I_grid_a;
    int16 I_grid_b;
    int16 I_grid_c;
    int16 U_dc;
    int16 U_dc_n;
    int16 I_conv_a;
    int16 I_conv_b;
    int16 I_conv_c;
    int16 I_conv_n;
};

struct Energy_meter_upper_struct
{
    Uint64 P_p[3];
    Uint64 P_n[3];
    Uint64 QI[3];
    Uint64 QII[3];
    Uint64 QIII[3];
    Uint64 QIV[3];
    struct
    {
        Uint64 P_p;
        Uint64 P_n;
        Uint64 QI;
        Uint64 QII;
        Uint64 QIII;
        Uint64 QIV;
    }sum;
};

struct Energy_meter_lower_struct
{
    Uint32 P_p[3];
    Uint32 P_n[3];
    Uint32 QI[3];
    Uint32 QII[3];
    Uint32 QIII[3];
    Uint32 QIV[3];
    struct
    {
        Uint32 P_p;
        Uint32 P_n;
        Uint32 QI;
        Uint32 QII;
        Uint32 QIII;
        Uint32 QIV;
    }sum;
};

struct Energy_meter_struct
{
    struct Energy_meter_upper_struct upper;
    struct Energy_meter_lower_struct lower;
};

union CONTROL_EXT_MODBUS
{
    Uint16 all[2];
    struct
    {
        Uint16 baudrate;
        Uint16 ext_server_id;
    }fields;
};
//
//Task 1 (C) Variables
//
extern struct Energy_meter_struct Energy_meter;

extern struct CPU1toCPU2_struct CPU1toCPU2;
extern struct CPU2toCPU1_struct CPU2toCPU1;
extern struct CLA2toCLA1_struct CLA2toCLA1;

extern struct Measurements_master_struct Meas_master;
extern struct Measurements_master_gain_offset_struct Meas_master_gain_error;
extern struct Measurements_master_gain_offset_struct Meas_master_offset_error;
extern struct Measurements_master_gain_offset_struct Meas_master_gain;
extern struct Measurements_master_gain_offset_struct Meas_master_offset;
extern struct Measurements_alarm_struct Meas_alarm_H;
extern struct Measurements_alarm_struct Meas_alarm_L;

extern struct abc_struct U_x0, U_x1;
extern float decimator;

extern struct SCOPE_global scope_global;

extern union CONTROL_EXT_MODBUS control_ext_modbus;

extern struct CONTROL_master control_master;
extern struct STATUS_master status_master;
extern union ALARM_master alarm_master;
extern union ALARM_master alarm_master_snapshot;

extern volatile union EMIF_union EMIF_mem;
extern struct EMIF_CLA_struct EMIF_CLA;

extern struct CIC2_struct CIC2_calibration;
extern CLA_FPTR CIC2_calibration_input;

extern struct trigonometric_struct sincos_table[SINCOS_HARMONICS];

extern void Fast_copy_modbus_CPUasm();
extern void Fast_copy21_CPUasm();
extern void Energy_meter_CPUasm();
extern void DINT_copy_CPUasm(Uint16 *dst, Uint16 *src, Uint16 size);
extern void SINCOS_calc_CPUasm(struct trigonometric_struct *sincos_table, float angle);


//
//Task 2 (C) Variables
//

//
//Task 3 (C) Variables
//

//
//Task 4 (C) Variables
//

//
//Task 5 (C) Variables
//

//
//Task 6 (C) Variables
//

//
//Task 7 (C) Variables
//

//
//Task 8 (C) Variables
//

//
// Function Prototypes
//
// The following are symbols defined in the CLA assembly code
// Including them in the shared header file makes them
// .global and the main CPU can make use of them.
//
__interrupt void Cla1Task1();
__interrupt void Cla1Task2();
__interrupt void Cla1Task3();
__interrupt void Cla1Task4();
__interrupt void Cla1Task5();
__interrupt void Cla1Task6();
__interrupt void Cla1Task7();
__interrupt void Cla1Task8();

#ifdef __cplusplus
}
#endif // extern "C"

#endif //end of _CLA_DIVIDE_SHARED_H_ definition

//
// End of file
//
