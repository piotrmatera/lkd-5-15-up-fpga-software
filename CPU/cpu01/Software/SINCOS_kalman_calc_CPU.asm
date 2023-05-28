;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void SINCOS_kalman_calc_CPUasm(float (struct trigonometric_struct *sincos_table, float angle);
; argument 1 = *sinecos_table[KALMAN_HARMONICS] : destination of sine+cosine calculation [XAR4]
; argument 2 = angle : 32-bit floating point radian angle <> [R0H]
; (KALMAN_HARMONICS+2)*4 instructions without pre-jump

        .cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg SINCOS_kalman_calc_CPUasm, _SINCOS_kalman_calc_CPUasm
		.endif

        .global _SINCOS_kalman_calc_CPUasm
        .text

_SINCOS_kalman_calc_CPUasm:
        DIV2PIF32 R0H, R0H
	    MOV32 *SP++, R7H                ;save R7H on stack
        MOV32 *SP++, R6H                ;save R6H on stack
        MOV32 *SP++, R5H                ;save R5H on stack
        MOV32 *SP++, R4H                ;save R4H on stack

        MPYF32 R1H, R0H, #3.0f

        COSPUF32 R7H, R0H
        SINPUF32 R6H, R0H
		ADDF32  R0H, R0H, R0H
		NOP
		ADDF32  R2H, R1H, R0H

        COSPUF32 R5H, R1H
        SINPUF32 R4H, R1H
        ADDF32  R1H, R2H, R0H
||      MOV32 *XAR4++, R7H
        MOV32 *XAR4++, R6H

;;;;;;;;;;;;;;;;;;;

        .loop SINCOS_HARMONICS/2-1
        COSPUF32 R7H, R2H
        SINPUF32 R6H, R2H
        ADDF32  R2H, R1H, R0H
||      MOV32 *XAR4++, R5H
        MOV32 *XAR4++, R4H

        COSPUF32 R5H, R1H
        SINPUF32 R4H, R1H
        ADDF32  R1H, R2H, R0H
||      MOV32 *XAR4++, R7H
        MOV32 *XAR4++, R6H
        .endloop

        MOV32 *XAR4++, R5H
        MOV32 *XAR4++, R4H
;Restore the utilized save on entry register
        MOV32 R5H, *--SP                ;restore R5H from stack
        MOV32 R7H, *--SP                ;restore R7H from stack
        MOV32 R6H, *--SP                ;restore R6H from stack
        MOV32 R4H, *--SP                ;restore R4H from stack

;Finish up
        LRETR                           ;Return

;end of function SINCOS_kalman_calc_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
