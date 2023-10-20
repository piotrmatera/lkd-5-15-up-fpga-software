#ifndef _CLA_SHARED_H_
#define _CLA_SHARED_H_

#define PWM_CLOCK 125000000

#define CONV_FREQUENCY 31250
#define CONTROL_RATE 4.0
#define SCOPE_BUFFER (Uint32)(0.02 * (float)CONV_FREQUENCY * 2.0 / CONTROL_RATE * 4)
//#define SCOPE_BUFFER 1250
#define SCOPE_CHANNEL 12
typedef float scope_data_type;

#define SINCOS_HARMONICS 50
#define CIC_upsample1 10
#define CIC_upsample2 100

//
// Included Files
//
#include <stdint.h>

#include "F28x_Project.h"

#include "F2837xD_Cla_defines.h"
#include "CLAmath.h"

#include "Controllers.h"
#include "Converter.h"
#include "CPU_shared.h"
#include "Node_shared.h"
#include "Kalman_gains.h"
#include "Kalman.h"
#include "Grid_analyzer.h"

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
    Uint16 CLA_GRID1_TASK1;
    Uint16 CLA_GRID2_TASK1;
    Uint16 CLA_CONV_TASK1;
    Uint16 CLA_THERM_TASK1;
    Uint16 CLA_END_TASK1;
    Uint16 CPU_SD;
    Uint16 CPU_COPY1;
    Uint16 CPU_COPY2;
    Uint16 CPU_ERROR;
    Uint16 CPU_SCOPE;
    Uint16 CPU_COMM;
    Uint16 CPU_GRID;
    Uint16 CPU_END;
};

extern struct Timer_PWM_struct Timer_PWM;

#define TIMESTAMP_PWM EPwm4Regs.TBCTR

struct Thermistor_struct
{
    float Divider_supply;
    float R_divider;
    float B;
    float T_0;
    float R25;
    float DIV_Rinf;
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
extern struct Thermistor_struct Therm;
extern struct Thermistor_struct Therm_module;

extern struct Measurements_ACDC_struct Meas_ACDC;
extern struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain_error;
extern struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset_error;
extern struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain;
extern struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset;
extern struct Measurements_ACDC_alarm_struct Meas_ACDC_alarm_H;
extern struct Measurements_ACDC_alarm_struct Meas_ACDC_alarm_L;
extern struct EMIF_SD_struct EMIF_CLA;

extern union CONTROL_EXT_MODBUS control_ext_modbus;

extern struct CONTROL_ACDC control_ACDC;
extern struct STATUS_ACDC status_ACDC;
extern struct STATUS_ACDC status_ACDC_master;
extern union ALARM_ACDC alarm_ACDC;
extern union ALARM_ACDC alarm_ACDC_snapshot;

extern struct CIC2_struct CIC2_calibration;
extern CLA_FPTR CIC2_calibration_input;
extern struct CIC1_adaptive_global_struct CIC1_adaptive_global__50Hz;
extern struct CIC1_adaptive2_global_struct CIC1_adaptive2_global__50Hz;
extern struct CIC1_global_struct CIC1_global__50Hz;

extern struct trigonometric_struct sincos_table[SINCOS_HARMONICS];
extern struct trigonometric_struct sincos_table_comp[SINCOS_HARMONICS];
extern struct trigonometric_struct sincos_table_comp2[SINCOS_HARMONICS];
extern struct trigonometric_struct sincos_table_Kalman[SINCOS_HARMONICS];

extern void Fast_copy_modbus_CPUasm();
extern void Energy_meter_CPUasm();
extern void DINT_copy_CPUasm(Uint16 *dst, Uint16 *src, Uint16 size);
extern void SINCOS_calc_CPUasm(struct trigonometric_struct *sincos_table, float angle);
extern void SINCOS_kalman_calc_CPUasm(struct trigonometric_struct *sincos_table, float angle);
extern void Kalman_THD_calc_CPUasm(struct Kalman_struct *Kalman, int32 *EMIF_Kalman);
extern float CIC1_adaptive_filter_CPUasm(struct CIC1_adaptive_global_struct *CIC_global, struct CIC1_adaptive_struct *CIC, float input);

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
