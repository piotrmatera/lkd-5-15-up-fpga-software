;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Energy_meter_CPUasm();
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Energy_meter_CPUasm, _Energy_meter_CPUasm
		.endif

        .global _Energy_meter_CPUasm
        .sect ".interrupt_code"

_Energy_meter_CPUasm:

		MOV		T, #32
		MOVL	XAR4, #_Energy_meter.upper
		MOVL	XAR5, #_Energy_meter.lower
		MOVL	XAR6, #_Energy_meter_params.input_P_p
		ADDB SP, #2

		ZAPA
		RPTB 	Summation_end, #(24-1)

		MOVL	ACC, *XAR6++
		ADDL	*XAR5++, ACC

		MOVL	ACC, *XAR4
		ADDCL	ACC, P
		MOVL	*XAR4++, ACC

		MOVL	ACC, *XAR4
		ADDCL	ACC, P
		MOVL	*XAR4++, ACC
Summation_end:

		SUBB SP, #2

        LRETR                           ;Return

;end of function Energy_meter_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
