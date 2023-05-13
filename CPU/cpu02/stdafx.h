#ifndef _CLA_SHARED_H_
#define _CLA_SHARED_H_

#define SINCOS_HARMONICS 50

#define CIC_upsample1 5
#define CIC_upsample2 50

//
// Included Files
//

#include "F28x_Project.h"

#include "F2837xD_Cla_defines.h"
#include "CLAmath.h"
#include <stdint.h>

#include "Controllers.h"
#include "Kalman.h"
#include "CPU_shared.h"
#include "Grid_analyzer.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Defines
//

struct Timer_PWM_struct
{
    Uint16 CLA_START_TASK;
    Uint16 CLA_GRID;
    Uint16 CLA_GRID_FILTER;
    Uint16 CLA_ENDTASK;
    Uint16 CPU_SD;
    Uint16 CPU_MEAS;
    Uint16 CPU_KALMAN_U;
    Uint16 CPU_GRID;
    Uint16 CPU_SD_end;
};

extern struct Timer_PWM_struct Timer_PWM;

#define TIMESTAMP_PWM EPwm10Regs.TBCTR

//
//Task 1 (C) Variables
//

extern struct CPU1toCPU2_struct CPU1toCPU2;
extern struct CPU2toCPU1_struct CPU2toCPU1;
extern struct CLA1toCLA2_struct CLA1toCLA2;

extern struct Measurements_master_struct Meas_master;

extern struct trigonometric_struct sincos_table[SINCOS_HARMONICS];
extern struct CIC1_adaptive_global_struct CIC1_adaptive_global__50Hz;

extern void Fast_copy21_CPUasm();
extern void SINCOS_calc_CPUasm(struct trigonometric_struct *sincos_table, float angle);
extern void SINCOS_kalman_calc_CPUasm(struct trigonometric_struct *sincos_table, float angle);
extern void Kalman_calc_CPUasm(struct Kalman_struct *Kalman, struct trigonometric_struct sincos[], float input);
extern void Kalman_THD_calc_CPUasm(struct Kalman_struct *Kalman);

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