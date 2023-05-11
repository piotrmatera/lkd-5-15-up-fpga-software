;Tomasz Swiechowicz swiechowicz.tomasz@gmail.com

; C prototype:
; extern void Kalman_calc_CPUasm(struct Kalman_struct *Kalman, struct trigonometric_struct sincos[], float input);
; argument 1 = *Kalman : structure of internal variables [XAR4]
; argument 2 = input : input to be decomposed [R0H]
; (6+1+8)*N + 33 instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Kalman_calc_CPUasm, _Kalman_calc_CPUasm
		.endif

        .global _Kalman_calc_CPUasm
        .text

_Kalman_calc_CPUasm:
;		ESTOP0
        MOV32    *SP++, R4H                ;save R4H on stack
        MOV32    *SP++, R5H                ;save R5H on stack
        MOV32    *SP++, R6H                ;save R6H on stack
        MOV32    *SP++, R7H                ;save R7H on stack

        MOV32    *SP++, R0H                ;save error on stack

        MOVL     XAR7, XAR4                      ;save Kalman_struct in XAR7
        MOV      ACC, #Kalman_struct.states
        ADDL     XAR4, ACC
        MOVL     XAR6, XAR4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; end of preparation
;R0 x0
;R1 x1
;R2 cos
;R3 sin
;R4 x0*sin
;R5 x1*sin
;R6 x0*cos
;R7 x1*cos

;R6 cos*x0 - sin*x1 ;new x0
;R7 cos*x1 + sin*x0 ;new x1

;XAR4 states
;XAR5 sincos
;XAR6 states
;XAR7 -
        MOV32    R6H, *XAR4++
        MOV32    R7H, *XAR4++
        MOV32    R0H, *XAR4++
        MOV32    R1H, *XAR4++
        MOV32    R2H, *XAR5++
        MOV32    R3H, *XAR5++

        .loop KALMAN_HARMONICS-1
        MPYF32   R6H, R2H, R0H
||      MOV32    *XAR6++, R6H
        MPYF32   R7H, R2H, R1H
||      MOV32    *XAR6++, R7H
        MPYF32   R5H, R3H, R1H
||      MOV32    R2H, *XAR5++
        MPYF32   R4H, R3H, R0H
||      MOV32    R3H, *XAR5++
        SUBF32   R6H, R6H, R5H
||      MOV32    R0H, *XAR4++
        ADDF32   R7H, R7H, R4H
||      MOV32    R1H, *XAR4++
        .endloop

        MOV32    *XAR6++, R6H
        MOV32    *XAR6++, R7H
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; end of prediction calculation
		NOP		 *, ARP4
        MOVL     XAR0, #4
        MOVL     XAR4, XAR7
        MOV32    R0H,*0++
        MOV32    R1H,*0++
        MOV32    R2H,*0++

        .loop KALMAN_HARMONICS/2-1
        ADDF32   R0H, R0H, R1H
||      MOV32    R3H, *0++
        ADDF32   R2H, R2H, R3H
||      MOV32    R1H, *0++
        .endloop

        MOVL     XAR4, XAR7
        ADDF32   R0H, R2H, R0H
||      MOV32    R1H, *--SP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; end of output prediction calculation
;R0 error
;R1 sqrt
;R2 new x0
;R3 new x1
;R4 x0 gain
;R5 x0
;R6 x1 gain
;R7 x1

;XAR4 states
;XAR5 gain
;XAR6 states
;XAR7 amplitudes
        MOVL     XAR0, #Kalman_struct.estimate
        MOV32    *+XAR4[AR0], R0H

        MOVL     XAR0, #Kalman_struct.gain
        MOVL     XAR5, *+XAR4[AR0]

        MOV      ACC, #Kalman_struct.rms_values
        ADDL     XAR7, ACC

        MOV      ACC, #Kalman_struct.states
        ADDL     XAR4, ACC

		MOVW     DP, #_MATH_1_SQRT2

        SUBF32   R0H, R1H, R0H          ;error
||      MOV32    R4H, *XAR5++
        MOVL     XAR6, XAR4
        MPYF32   R4H, R4H, R0H
||      MOV32    R6H, *XAR5++
        MPYF32   R6H, R6H, R0H
||      MOV32    R5H, *XAR4++
        ADDF32   R2H, R5H, R4H
||      MOV32    R7H, *XAR4++
        ZERO     R7H
        ADDF32   R3H, R7H, R6H
||      MOV32    R4H, *XAR5++

        MPYF32   R2H, R2H, R2H
||      MOV32    *XAR6++, R2H
        MPYF32   R3H, R3H, R3H
||      MOV32    *XAR6++, R3H
        MPYF32   R4H, R4H, R0H
||      MOV32    R6H, *XAR5++
        ADDF32   R3H, R2H, R3H
||      MOV32    R5H, *XAR4++
        MPYF32   R6H, R6H, R0H
||      MOV32    R7H, *XAR4++
        ADDF32   R3H, R3H, R3H
        ADDF32   R2H, R5H, R4H
        NOP

        SQRTF32  R1H,R3H
        ADDF32   R3H, R7H, R6H
||      MOV32    R4H, *XAR5++

        .loop KALMAN_HARMONICS-2
        MPYF32   R2H, R2H, R2H
||      MOV32    *XAR6++, R2H
        MPYF32   R3H, R3H, R3H
||      MOV32    *XAR6++, R3H
        MPYF32   R4H, R4H, R0H
||      MOV32    R6H, *XAR5++
        ADDF32   R3H, R2H, R3H
||		MOV32	 R2H, @_MATH_1_SQRT2
        MPYF32   R1H, R1H, R2H
||      MOV32    R5H, *XAR4++
        MPYF32   R6H, R6H, R0H
||      MOV32    R7H, *XAR4++
        ADDF32   R2H, R5H, R4H
||      MOV32    *XAR7++, R1H
        SQRTF32  R1H,R3H
        ADDF32   R3H, R7H, R6H
||      MOV32    R4H, *XAR5++
        .endloop

        MPYF32   R2H, R2H, R2H
||      MOV32    *XAR6++, R2H
        MPYF32   R3H, R3H, R3H
||      MOV32    *XAR6++, R3H
		MOV32	 R0H, @_MATH_1_SQRT2
		MPYF32   R1H, R1H, R0H
||      ADDF32   R3H, R2H, R3H
        NOP
        MOV32    *XAR7++, R1H
        SQRTF32  R1H,R3H

;Restore the utilized save on entry register
        MOV32    R7H, *--SP                ;restore R7H from stack
        MOV32    R6H, *--SP                ;restore R6H from stack
        MOV32    R5H, *--SP                ;restore R5H from stack
		MOV32	 R0H, @_MATH_1_SQRT2
		MPYF32   R1H, R1H, R0H
        MOV32    R4H, *--SP                ;restore R4H from stack
        MOV32    *XAR7++, R1H

;Finish up
        LRETR                           ;Return

;end of function Kalman_calc_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
