;Tomasz Swiechowicz swiechowicz.tomasz@gmail.com

; C prototype:
; extern void Kalman_THD_calc_CPUasm(struct Kalman_struct *Kalman);
; argument 1 = *Kalman : structure of internal variables [XAR4]
; argument 2 = *EMIF_Kalman : structure of EMIF variables [XAR5]
; (6+1+8)*N + 33 instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Kalman_THD_calc_CPUasm, _Kalman_THD_calc_CPUasm
		.endif

        .global _Kalman_THD_calc_CPUasm
        .sect ".TI.ramfunc"

_Kalman_THD_calc_CPUasm:
;		ESTOP0
        MOV32    *SP++, R4H                ;save R4H on stack
        MOV32    *SP++, R5H                ;save R5H on stack
        MOV32    *SP++, R6H                ;save R6H on stack
        MOV32    *SP++, R7H                ;save R7H on stack
;copy amplitudes
;XAR4 EMIF_mem
;XAR4 Kalman_struct
;XAR5 amplitudes
        MOVL     XAR7, XAR5
        MOVL     XAR0, #6

        MOVL     XAR5, XAR4
        MOV      ACC, #Kalman_struct.rms_values
        ADDL     XAR5, ACC

		MOVW     DP, #_Conv.div_range_modifier_Kalman_values_square
		MOV32	 R7H, @_Conv.div_range_modifier_Kalman_values_square

		SETC AMODE ; Change to AMODE = 1
		.lp_amode ; Tell assembler to check for AMODE = 1 syntax

		NOP		 *, ARP7
        I32TOF32 R4H,*0++, ARP7
        I32TOF32 R5H,*0++, ARP7
        MPYF32   R2H, R4H, R7H

        I32TOF32 R4H,*0++, ARP7
        MPYF32   R3H, R5H, R7H
		SQRTF32  R0H,R2H
        I32TOF32 R5H,*0++, ARP7
        MPYF32   R2H, R4H, R7H
		SQRTF32  R1H,R3H

        .loop FPGA_KALMAN_STATES/2-1
        I32TOF32 R4H,*0++, ARP5
        MPYF32   R3H, R5H, R7H
||        MOV32    *++, R0H, ARP7
		SQRTF32  R0H,R2H
        I32TOF32 R5H,*0++, ARP5
        MPYF32   R2H, R4H, R7H
||        MOV32    *++, R1H, ARP7
		SQRTF32  R1H,R3H
        .endloop
		NOP
		NOP
		NOP		 *, ARP5
        MOV32    *XAR5++, R0H, ARP5
        MOV32    *XAR5++, R1H, ARP5

		CLRC AMODE ; Revert back to AMODE = 0
		.c28_amode ; Tell assembler to check for AMODE = 1 syntax
;calculate total THD
;XAR4 Kalman_struct
;XAR5 amplitudes
;XAR6 THD_individual
        MOVL     XAR6, XAR4
        MOVL     XAR5, XAR4
        MOV      ACC, #Kalman_struct.rms_values
        ADDL     XAR5, ACC
        MOV      ACC, #Kalman_struct.THD_individual
        ADDL     XAR6, ACC

        MOV32    R4H, *XAR5++
        MOV32    R5H, *XAR5++
        MOV32    R0H, R5H

		DIVF32	 R4H, R4H, R0H
		DIVF32	 R5H, R5H, R0H
        MOV32    R6H, *XAR5++
        MOV32    R7H, *XAR5++
		DIVF32	 R6H, R6H, R0H
		DIVF32	 R7H, R7H, R0H
        MOV32    *XAR6++, R4H
        MOV32    *XAR6++, R5H
        MOV32    R4H, *XAR5++
        MOV32    R5H, *XAR5++

        .loop FPGA_KALMAN_STATES/4
		DIVF32	 R4H, R4H, R0H
		DIVF32	 R5H, R5H, R0H
        MOV32    *XAR6++, R6H
        MOV32    *XAR6++, R7H
        MOV32    R6H, *XAR5++
        MOV32    R7H, *XAR5++
		DIVF32	 R6H, R6H, R0H
		DIVF32	 R7H, R7H, R0H
        MOV32    *XAR6++, R4H
        MOV32    *XAR6++, R5H
        MOV32    R4H, *XAR5++
        MOV32    R5H, *XAR5++

        .endloop
        ;MOV32    *XAR6++, R6H
        ;MOV32    *XAR6++, R7H

;calculate total THD
        MOVL     XAR5, XAR4
        MOV      ACC, #Kalman_struct.rms_values
        ADDL     XAR5, ACC

		MOV32    R0H, *XAR5++
		MOV32    R1H, *XAR5++
		ZERO     R3H
		ZERO     R7H
		MOV32    R0H, *XAR5++
		MPYF32   R2H, R0H, R0H
||		MOV32    R0H, *XAR5++
		MPYF32   R6H, R0H, R0H
||		MOV32    R0H, *XAR5++

        .loop FPGA_KALMAN_STATES/2-1
		MACF32 R3H, R2H, R2H, R0H, R0H
||		MOV32 R0H, *XAR5++
		MACF32 R7H, R6H, R6H, R0H, R0H
||		MOV32 R0H, *XAR5++
        .endloop

		MOVIZF32 R2H, #1.0
		MPYF32   R3H, R1H, R1H
||      ADDF32	 R0H, R3H, R7H
		DIVF32	 R1H, R2H, R1H
		ADDF32	 R3H, R3H, R0H
        SQRTF32  R0H,R0H
;Restore the utilized save on entry register
        MOV32    R7H, *--SP                ;restore R7H from stack
        SQRTF32  R3H,R3H
        MOV32    R6H, *--SP                ;restore R6H from stack
        MOV32    R5H, *--SP                ;restore R5H from stack
        MOV32    R4H, *--SP                ;restore R4H from stack

        MOVL     XAR0, #Kalman_struct.harmonic_RMS
		MPYF32   R0H, R1H, R0H
||      MOV32    *+XAR4[AR0], R0H
        MOVL     XAR0, #Kalman_struct.total_RMS
        MOV32    *+XAR4[AR0], R3H
        MOVL     XAR0, #Kalman_struct.THD_total
        MOV32    *+XAR4[AR0], R0H


;Finish up
        LRETR                           ;Return

;end of function Kalman_THD_calc_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
