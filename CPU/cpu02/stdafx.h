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
#include "Converter.h"
#include "PLL.h"
#include "CPU_shared.h"
#include "IO.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Defines
//

struct Timer_PWM_struct
{
    Uint16 CLA_START_TASK;
    Uint16 CLA_IERR;
    Uint16 CLA_CONV;
    Uint16 CLA_PLL;
    Uint16 CLA_ENDTASK;
    Uint16 CPU_SD;
    Uint16 CPU_MEAS;
    Uint16 CPU_IERR;
    Uint16 CPU_MR_START;
    Uint16 CPU_PWM;
    Uint16 CPU_SD_END;
};

extern struct Timer_PWM_struct Timer_PWM;

#define TIMESTAMP_PWM EPwm5Regs.TBCTR

//
//Task 1 (C) Variables
//

extern struct CLA1toCLA2_struct CLA1toCLA2;

extern struct Measurements_ACDC_struct Meas_ACDC;
extern struct Measurements_ACDC_gain_offset_struct Meas_ACDC_gain;
extern struct Measurements_ACDC_gain_offset_struct Meas_ACDC_offset;
extern struct EMIF_SD_struct EMIF_CLA;

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
