
/*
 *  ======== main.c ========
 */
#include <string.h>
#include <math.h>

#include "stdafx.h"
#include "Init.h"
#include "HWIs.h"

void main()
{
    InitFlash();

    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    EALLOW;
    PieVectTable.XINT1_INT = &SD_FAST_INT;
    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    IER |= M_INT1;

    Init.PWMs();

    Init.CLA();

    Init.Variables();

    EALLOW;
    Cla1Regs.MIER.bit.INT1 = 1;
    EDIS;

    EINT;

    while(1)
    {
        static Uint32 timer_new = 0;
        static Uint32 timer_old = 0;
        timer_new = ReadIpcTimer();
        if(timer_new - timer_old > 20000000UL)
        {
            timer_old = timer_new;

            Conv.Kr_I = CPU1toCPU2.Kr_I;
            Conv.Kp_I = CPU1toCPU2.Kp_I;
            Conv.compensation2 = CPU1toCPU2.compensation2;
            Conv.L_conv = CPU1toCPU2.L_conv;

            SINCOS_calc_CPUasm(sincos_table, PLL.w_filter * PLL.Ts);
            SINCOS_calc_CPUasm(sincos_table_comp, PLL.w_filter * PLL.Ts * Conv.compensation2);

            for(Uint16 i = 0; i < FPGA_RESONANT_STATES; i++)
            {
                EMIF_mem.write.Resonant[0].harmonic[i].cosine_A = sincos_table[2 * i].cosine * Conv.range_modifier;
                EMIF_mem.write.Resonant[0].harmonic[i].sine_A = sincos_table[2 * i].sine * Conv.range_modifier;
                EMIF_mem.write.Resonant[0].harmonic[i].cosine_B = (sincos_table[2 * i].cosine - 1.0f) / (float)(2 * i + 1) * Conv.Kr_I * Conv.range_modifier;
                EMIF_mem.write.Resonant[0].harmonic[i].sine_B = sincos_table[2 * i].sine / (float)(2 * i + 1) * Conv.Kr_I * Conv.range_modifier;
                EMIF_mem.write.Resonant[0].harmonic[i].cosine_C = sincos_table_comp[2 * i].cosine * Conv.range_modifier;
                EMIF_mem.write.Resonant[0].harmonic[i].sine_C = sincos_table_comp[2 * i].sine * Conv.range_modifier;

                EMIF_mem.write.Resonant[1].harmonic[i].cosine_A = sincos_table[2 * i + 1].cosine * Conv.range_modifier;
                EMIF_mem.write.Resonant[1].harmonic[i].sine_A = sincos_table[2 * i + 1].sine * Conv.range_modifier;
                EMIF_mem.write.Resonant[1].harmonic[i].cosine_B = (sincos_table[2 * i + 1].cosine - 1.0f) / (float)(2 * i + 2) * Conv.Kr_I * Conv.range_modifier;
                EMIF_mem.write.Resonant[1].harmonic[i].sine_B = sincos_table[2 * i + 1].sine / (float)(2 * i + 2) * Conv.Kr_I * Conv.range_modifier;
                EMIF_mem.write.Resonant[1].harmonic[i].cosine_C = sincos_table_comp[2 * i + 1].cosine * Conv.range_modifier;
                EMIF_mem.write.Resonant[1].harmonic[i].sine_C = sincos_table_comp[2 * i + 1].sine * Conv.range_modifier;
            }
        }
    }
}
